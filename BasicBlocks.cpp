
/*! @file
 *  Base was taken from MyPinTool.cpp
 *  Print out BB: <addr> for each basic block executed.
 *  This will be parsed later for code coverage purposes.
 */

#include "pin.H"
#include <iostream>
#include <fstream>

/* ================================================================== */
// Global variables 
/* ================================================================== */

std::ostream * out = &cerr;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool",
    "o", "", "specify file name for BasicBlocks output");

KNOB<BOOL>   KnobLog(KNOB_MODE_WRITEONCE,  "pintool",
    "log", "1", "log basic blocks called in the application");


/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
INT32 Usage()
{
    cerr << "This tool prints out the address of each basic block that" << endl <<
            "gets executed for use with code coverage analysis" << endl << endl;

    cerr << KNOB_BASE::StringKnobSummary() << endl;

    return -1;
}

/* ===================================================================== */
// Analysis routines
/* ===================================================================== */

/*!
 * Print the address of any basic blocks called during execution
 * This function is called for every basic block when it is about to be executed.
 * @param[in]   addr            start address of the basic block
 * @note use atomic operations for multi-threaded applications
 */
VOID LogBbl(ADDRINT addr)
{
	// This value is based on x86_64 Linux systems
	if (addr < 0x7f0000000000) {
		std::cout << "BB: 0x" << std::hex << addr << endl;
	}
}

/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

/*!
 * Insert call to the LogBbl() analysis routine before every basic block 
 * of the trace.
 * This function is called every time a new trace is encountered.
 * @param[in]   trace    trace to be instrumented
 * @param[in]   v        value specified by the tool in the TRACE_AddInstrumentFunction
 *                       function call
 */
VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Insert a call to LogBbl() before every basic bloc, passing the number of instructions
        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)LogBbl, IARG_ADDRINT, BBL_Address(bbl), IARG_END);
    }
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments, 
 *                              including pin -t <toolname> -- ...
 */
int main(int argc, char *argv[])
{
    // Initialize PIN library. Print help message if -h(elp) is specified
    // in the command line or the command line is invalid 
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    string fileName = KnobOutputFile.Value();

    if (!fileName.empty()) { out = new std::ofstream(fileName.c_str());}

    if (KnobLog)
    {
        // Register function to be called to instrument traces
        TRACE_AddInstrumentFunction(Trace, 0);
    }
    
    cerr <<  "===============================================" << endl;
    cerr <<  "This application is instrumented by BasicBlocks" << endl;
    if (!KnobOutputFile.Value().empty()) 
    {
        cerr << "See file " << KnobOutputFile.Value() << " for analysis results" << endl;
    }
    cerr <<  "===============================================" << endl;

    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
