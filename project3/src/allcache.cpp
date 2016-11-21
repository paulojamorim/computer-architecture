/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2016 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
//
// @ORIGINAL_AUTHOR: Artur Klauser
//

/*! @file
 *  This file contains an ISA-portable PIN tool for functional simulation of
 *  instruction+data TLB+cache hieraries
 */


#include <iostream>
#include <fstream>

#include "pin.H"
#include "pinplay.H"
#include "cache.H"
#include "pin_cache.H"

//typedef UINT32 CACHE_STATS; // type of cache hit/miss counters

PINPLAY_ENGINE pinplay;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE, "pintool", "log", "0", "Activate the pinplay logger");
KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE, "pintool", "replay", "0", "Activate the pinplay replayer");
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");

ofstream OutFile;

FILE * trace;
static bool is_pin_play = false; //run tranditional mode (with out pinplay)

const int L1I = 0;
const int L1D = 1;
const int L2U = 2;
const int L3U = 3;

int CCL; //current cache level


namespace ITLB
{
    // instruction TLB: 4 kB pages, 32 entries, fully associative
    const UINT32 lineSize = 4*KILO;
    const UINT32 cacheSize = 32 * lineSize;
    const UINT32 associativity = 32;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALFUN ITLB::CACHE itlb("ITLB", ITLB::cacheSize, ITLB::lineSize, ITLB::associativity);

namespace DTLB
{
    // data TLB: 4 kB pages, 32 entries, fully associative
    const UINT32 lineSize = 4*KILO;
    const UINT32 cacheSize = 32 * lineSize;
    const UINT32 associativity = 32;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR DTLB::CACHE dtlb("DTLB", DTLB::cacheSize, DTLB::lineSize, DTLB::associativity);

namespace IL1
{
    // 1st level instruction cache: 32 kB, 64 B lines, 4-way associative (paper conf.)
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 4;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR IL1::CACHE il1("L1 Instruction Cache", IL1::cacheSize, IL1::lineSize, IL1::associativity);

namespace DL1
{
    // 1st level data cache: 32 kB, 64 B lines, 4-way associative (paper conf.)
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 4;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR DL1::CACHE dl1("L1 Data Cache", DL1::cacheSize, DL1::lineSize, DL1::associativity);

namespace UL2
{
    // 2nd level unified cache: 32 kB, 64 B lines, 4-way (paper conf.)
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 4;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);

    //typedef CACHE_DIRECT_MAPPED(max_sets, allocation) CACHE;
    const UINT32 max_associativity = associativity;
    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR UL2::CACHE ul2("L2 Unified Cache", UL2::cacheSize, UL2::lineSize, UL2::associativity);

namespace UL3
{
    // 3rd level unified cache: 128 kB, 64 B lines, 8-way (paper conf.)
    const UINT32 cacheSize = 128*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);

    //typedef CACHE_DIRECT_MAPPED(max_sets, allocation) CACHE;
    const UINT32 max_associativity = associativity;
    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR UL3::CACHE ul3("L3 Unified Cache", UL3::cacheSize, UL3::lineSize, UL3::associativity);


//-------------------------------------------------------------------

// Print a memory read record
VOID RecordMemRead(VOID * addr)
{

    CACHE_TAG tag;
    UINT32 index;

    if (CCL == L1D)
    {
        dl1.SplitAddress((ADDRINT)addr, tag, index);
        
        /*
        cout << addr;
        cout << "\n";
        cout << tag;
        cout << "\n";
        cout << index;
        cout << "\n\n\n\n";
        */
    }

    //fprintf(trace,"%p: R %p\n", ip, addr);
    //fprintf(trace,"R %p\n", addr);

}

// Print a memory write record
VOID RecordMemWrite(VOID * addr)
{
    CACHE_TAG tag;
    UINT32 index;

    if (CCL == L1D)
    {
        dl1.SplitAddress((ADDRINT)addr, tag, index);
        cout << tag;
        cout << "\n";
        cout << addr;
        cout << "\n\n\n\n";
    }




    //fprintf(trace,"%p: W %p\n", ip, addr);
    //fprintf(trace,"W %p\n", addr);

}

/*

VOID TakeCache(INS ins)
{ 

    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {

	        INS_InsertPredicatedCall(
				   ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
				   IARG_MEMORYOP_EA, memOp,
				   IARG_UINT32, INS_MemoryOperandSize(ins, memOp),
				   IARG_END);

        }
        
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
	        INS_InsertPredicatedCall(
				   ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
				   IARG_MEMORYOP_EA, memOp,
				   IARG_UINT32, INS_MemoryOperandSize(ins, memOp),
				   IARG_END);
        }
    }
}

*/

//----------------------------------------------------------------





LOCALFUN VOID Fini(int code, VOID * v)
{
    /*
    std::cerr << itlb;
    std::cerr << dtlb;
    std::cerr << il1;
    std::cerr << dl1;
    std::cerr << ul2;
    std::cerr << ul3;
    */
}

LOCALFUN VOID Ul2Access(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType)
{
    // second level unified cache
    const BOOL ul2Hit = ul2.Access(addr, size, accessType);

    // third level unified cache
    if ( ! ul2Hit)
    {
        ul3.Access(addr, size, accessType);
    }
    else
    {
        CACHE_TAG tag;
        UINT32 index;
        ul2.SplitAddress((ADDRINT)addr, tag, index);
    }

}

LOCALFUN VOID InsRef(ADDRINT addr)
{
    const UINT32 size = 1; // assuming access does not cross cache lines
    const CACHE_BASE::ACCESS_TYPE accessType = CACHE_BASE::ACCESS_TYPE_LOAD;

    // ITLB
    itlb.AccessSingleLine(addr, accessType);

    // first level I-cache
    const BOOL il1Hit = il1.AccessSingleLine(addr, accessType);

    // second level unified Cache
    if ( ! il1Hit) 
    {
        Ul2Access(addr, size, accessType);
    }
    else
    {
        CACHE_TAG tag;
        UINT32 index;
        il1.SplitAddress((ADDRINT)addr, tag, index);
    }
}

LOCALFUN VOID MemRefMulti(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType)
{
    // DTLB
    dtlb.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

    // first level D-cache
    const BOOL dl1Hit = dl1.Access(addr, size, accessType);

    // second level unified Cache
    if ( ! dl1Hit)
    {
        Ul2Access(addr, size, accessType);
    }
    else
    {
        CACHE_TAG tag;
        UINT32 index;
        dl1.SplitAddress((ADDRINT)addr, tag, index);
    }

}

LOCALFUN VOID MemRefSingle(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType)
{
    // DTLB
    dtlb.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

    // first level D-cache
    const BOOL dl1Hit = dl1.AccessSingleLine(addr, accessType);

    // second level unified Cache
    if ( ! dl1Hit) 
    {
        Ul2Access(addr, size, accessType);
    }
    else
    {
        CACHE_TAG tag;
        UINT32 index;
        dl1.SplitAddress((ADDRINT)addr, tag, index);

        char *dest = (char*) malloc(size);
        memcpy(dest, (const void*) addr, size);

        //cout << string(dest);
        cout << dest;
        cout << "\n";

        //int aPtr;
        //int len = 1; // Start with 1 string
        //aPtr = malloc(sizeof(int) * strlen(addr)); // Do not cast malloc in C
        //aPtr[0] = "This is a test";

    }
}

LOCALFUN VOID Instruction(INS ins, VOID *v)
{


    // all instruction fetches access I-cache
    INS_InsertCall(
        ins, IPOINT_BEFORE, (AFUNPTR)InsRef,
        IARG_INST_PTR,
        IARG_END);

    if (INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
    {
        const UINT32 size = INS_MemoryReadSize(ins);
        const AFUNPTR countFun = (size <= 4 ? (AFUNPTR) MemRefSingle : (AFUNPTR) MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, countFun,
            IARG_MEMORYREAD_EA,
            IARG_MEMORYREAD_SIZE,
            IARG_UINT32, CACHE_BASE::ACCESS_TYPE_LOAD,
            IARG_END);
    }

    if (INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
    {
        const UINT32 size = INS_MemoryWriteSize(ins);
        const AFUNPTR countFun = (size <= 4 ? (AFUNPTR) MemRefSingle : (AFUNPTR) MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, countFun,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_UINT32, CACHE_BASE::ACCESS_TYPE_STORE,
            IARG_END);
    }

}

GLOBALFUN int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);

    //parser argument for TLB (-4kb or -4mb)
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if (arg == "-pinplay")
        {
            pinplay.Activate(argc, argv,KnobPinPlayLogger, KnobPinPlayReplayer);
            is_pin_play = true;
        }
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0; // make compiler happy
}
