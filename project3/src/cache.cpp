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


/* -----------------------------------------------

Cache parameters:

Intel Xeon(R) CPU E5-2620

L1: 32 KB 
L2: 256 KB
L3: 15 MB (Set to 16MB because 15 crash pintool) 

Line size each levels:	64 bytes

--------------------------------------------------*/

#include <iostream>
#include <fstream>

#include "pin.H"
#include "pinplay.H"

PINPLAY_ENGINE pinplay;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE, "pintool", "log", "0", "Activate the pinplay logger");
KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE, "pintool", "replay", "0", "Activate the pinplay replayer");
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");

ofstream OutFile;

static UINT64 itlb_miss = 0;
static UINT64 dtlb_miss = 0;
static UINT64 memory_access_inst = 0;
static UINT64 memory_access_data = 0;

static bool is_tlb_4kb = true; //not is 4MB
static bool is_pin_play = false; //run tranditional mode (with out pinplay)


typedef UINT32 CACHE_STATS; // type of cache hit/miss counters

#include "pin_cache.H"


// -------  TLB 4k ----------------------------------------------------------------------------
namespace ITLB
{
    // instruction TLB: 4 kB pages, 512 entries, fully associative
    const UINT32 lineSize = 4*KILO;
    const UINT32 cacheSize = 512 * lineSize;
    const UINT32 associativity = 512;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALFUN ITLB::CACHE itlb("ITLB", ITLB::cacheSize, ITLB::lineSize, ITLB::associativity);

namespace DTLB
{
    // data TLB: 4 kB pages, 512 entries, fully associative
    const UINT32 lineSize = 4*KILO;
    const UINT32 cacheSize = 512 * lineSize;
    const UINT32 associativity = 512;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR DTLB::CACHE dtlb("DTLB", DTLB::cacheSize, DTLB::lineSize, DTLB::associativity);


//------- TLB 4MB ------------------------------------------------------------------------------
namespace ITLB_4MB
{
    // instruction TLB: 4 kB pages, 512 entries, fully associative
    const UINT32 lineSize = 4*MEGA;
    const UINT32 cacheSize = 512 * lineSize;
    const UINT32 associativity = 512;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALFUN ITLB_4MB::CACHE itlb_4mb("ITLB_4MB", ITLB_4MB::cacheSize, ITLB_4MB::lineSize, ITLB_4MB::associativity);

namespace DTLB_4MB
{
    // data TLB: 4 kB pages, 512 entries, fully associative
    const UINT32 lineSize = 4*MEGA;
    const UINT32 cacheSize = 512 * lineSize;
    const UINT32 associativity = 512;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR DTLB_4MB::CACHE dtlb_4mb("DTLB_4MB", DTLB_4MB::cacheSize, DTLB_4MB::lineSize, DTLB_4MB::associativity);




// ----------------- L1 ------------------------------------------------------------------------
namespace IL1
{
    // 1st level instruction cache: 32 kB, 32 B lines, 32-way associative
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR IL1::CACHE il1("L1 Instruction Cache", IL1::cacheSize, IL1::lineSize, IL1::associativity);

namespace DL1
{
    // 1st level data cache: 32 kB, 32 B lines, 8-way associative
    const UINT32 cacheSize = 32*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 8;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);
    const UINT32 max_associativity = associativity;

    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
}
LOCALVAR DL1::CACHE dl1("L1 Data Cache", DL1::cacheSize, DL1::lineSize, DL1::associativity);



//----------------- L2 --------------------------------------------------------------------------
namespace UL2
{
    // 2nd level unified cache: 256 KB, 64 B lines, direct mapped
    const UINT32 cacheSize = 256*KILO;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 1;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);

    typedef CACHE_DIRECT_MAPPED(max_sets, allocation) CACHE;
}
LOCALVAR UL2::CACHE ul2("L2 Unified Cache", UL2::cacheSize, UL2::lineSize, UL2::associativity);


//--------------- L3 -----------------------------------------------------------------------------
namespace UL3
{
    // 3rd level unified cache: 15 MB, 64 B lines, direct mapped
    const UINT32 cacheSize = 16*MEGA;
    const UINT32 lineSize = 64;
    const UINT32 associativity = 1;
    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

    const UINT32 max_sets = cacheSize / (lineSize * associativity);

    typedef CACHE_DIRECT_MAPPED(max_sets, allocation) CACHE;
}
LOCALVAR UL3::CACHE ul3("L3 Unified Cache", UL3::cacheSize, UL3::lineSize, UL3::associativity);


/*
LOCALFUN VOID Fini(int code, VOID * v)
{
    std::cerr << itlb;
    std::cerr << dtlb;
    std::cerr << il1;
    std::cerr << dl1;
    std::cerr << ul2;
    std::cerr << ul3;
}*/

LOCALFUN VOID Ul2Access(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType, bool is_data)
{
    // second level unified cache
    const BOOL ul2Hit = ul2.Access(addr, size, accessType);

    // third level unified cache
    if ( ! ul2Hit)
    {
        const BOOL ul3Hit = ul3.Access(addr, size, accessType);

        //L3 Miss
        if (! ul3Hit)
        {
            if (is_data == true)
            {
                //data miss (memory access)
                memory_access_data += 1;

            }
            else
            {
                // inst miss (memory access)
                memory_access_inst += 1;
            }
        }
    }
}

LOCALFUN VOID InsRef(ADDRINT addr)
{
    const UINT32 size = 1; // assuming access does not cross cache lines
    const CACHE_BASE::ACCESS_TYPE accessType = CACHE_BASE::ACCESS_TYPE_LOAD;
    BOOL tlb;

    // ITLB
    if (is_tlb_4kb)
        tlb =  itlb.AccessSingleLine(addr, accessType);
    else
        tlb =  itlb_4mb.AccessSingleLine(addr, accessType);

    // ITBL miss
    if (! tlb)
        itlb_miss += 1;

    // first level I-cache
    const BOOL il1Hit = il1.AccessSingleLine(addr, accessType);

    // second level unified Cache
    if ( ! il1Hit) Ul2Access(addr, size, accessType, false);
}

LOCALFUN VOID MemRefMulti(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType)
{
    // DTLB
    BOOL tlb;
    
    if (is_tlb_4kb)
        tlb = dtlb.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);
    else
        tlb = dtlb_4mb.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

    // DTBL miss
    if (! tlb)
        dtlb_miss += 1;

    // first level D-cache
    const BOOL dl1Hit = dl1.Access(addr, size, accessType);

    // second level unified Cache
    if ( ! dl1Hit) Ul2Access(addr, size, accessType, true);
}

LOCALFUN VOID MemRefSingle(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType)
{
    // DTLB
    const BOOL tlb = dtlb.AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);

    // DTBL miss
    if (! tlb)
        dtlb_miss += 1;

    // first level D-cache
    const BOOL dl1Hit = dl1.AccessSingleLine(addr, accessType);

    // second level unified Cache
    if ( ! dl1Hit) Ul2Access(addr, size, accessType, true);
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

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{

    OutFile.setf(ios::showbase);
    
    
    OutFile << "Memory access - Inst " << memory_access_inst << endl;
    OutFile << "Miss - Inst. TLB " << itlb_miss << endl;  
    OutFile << "Page table access - Inst. " << itlb_miss*3 << endl;  
    
    OutFile << "Memory access - Data " << memory_access_data << endl;
    OutFile << "Miss - Data TLB " << dtlb_miss << endl;
    OutFile << "Page table access - Data. " << dtlb_miss*3 << endl;

    
    OutFile.close();
}

GLOBALFUN int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);

    //parser argument for TLB (-4kb or -4mb)
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if (arg == "-4kb")
            is_tlb_4kb = true;

        if (arg == "-4mb")
            is_tlb_4kb = false;

        if (arg == "-pinplay")
        {
            pinplay.Activate(argc, argv,KnobPinPlayLogger, KnobPinPlayReplayer);
            is_pin_play = true;
        }
    }

    OutFile.open(KnobOutputFile.Value().c_str(), std::ios_base::app);
    
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0;
}

/* Pinplay replay
 
pin -xyzzy -reserve_memory pinball/foo.address -t your-tool.so -replay -replay:basename pinball/foo -- $PIN_ROOT/extrans/pinplay/bin/intel64/nullapp [for intel64 pinballs]

*/
