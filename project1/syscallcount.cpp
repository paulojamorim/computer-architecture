#include <iostream>
#include <fstream>
#include "pin.H"

ofstream OutFile;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 sysi_count = 0;
static UINT64 prgi_count = 0;

// To sum of instruction size
static UINT64 sysi_sum_size = 0;
static UINT64 prgi_sum_size = 0;

static UINT32 largest_inst_prog = 0;
static UINT32 lowest_inst_prog = 100000;

static UINT32 largest_inst_sys = 0;
static UINT32 lowest_inst_sys = 100000;


// This function is called before every instruction is executed
static VOID count_syscall(uint32_t ins_size) 
{

    sysi_sum_size += ins_size;
    sysi_count++;

    if (ins_size > largest_inst_sys)
        largest_inst_sys = ins_size;

    if (ins_size < lowest_inst_sys)
        lowest_inst_sys = ins_size;
}

static VOID count_program(uint32_t ins_size)  
{ 
    prgi_count++; 
    prgi_sum_size += ins_size;

    if (ins_size > largest_inst_prog)
        largest_inst_prog = ins_size;

    if (ins_size < lowest_inst_prog)
        lowest_inst_prog = ins_size;

}
    
// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    if(INS_IsSyscall(ins))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_syscall,IARG_UINT32, INS_Size(ins), IARG_END);

    }
    else
    {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)count_program,IARG_UINT32, INS_Size(ins), IARG_END);

   }

    //PIN_AddSyscallEntryFunction(ins);

}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{

    //printf(" >> %lu ", c);
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    OutFile << "Sys - number of inst. " << sysi_count << endl;
    OutFile << "Sys - largest inst. : " << largest_inst_sys << endl;
    OutFile << "Sys - lowest inst. : " << lowest_inst_sys << endl;
    OutFile << "Sys - mean inst. size : " << (sysi_sum_size / sysi_count) << endl;
    
    OutFile << "\n" << endl;
    
    OutFile << "Prog - number of inst. " << prgi_count << endl;
    OutFile << "Prog - largest inst. : " << largest_inst_prog << endl;
    OutFile << "Prog - lowest inst. : " << lowest_inst_prog << endl;
    OutFile << "Prog - mean inst. size : " << (prgi_sum_size / prgi_count) << endl;

    OutFile.close();

}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

void OnSyscall(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    //printf("Made syscall #%lu\n", PIN_GetSyscallNumber(ctxt, std));
    //c++;
}


int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) 
        return Usage();

    OutFile.open(KnobOutputFile.Value().c_str(), std::ios_base::app);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    PIN_AddSyscallEntryFunction(OnSyscall, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    return 0;
}
