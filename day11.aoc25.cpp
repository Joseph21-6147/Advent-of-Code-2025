// AoC 2025 - Day 11 - Reactor (attempt 2)
// =======================================

// date:  2025-12-26
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day11"          // <<===== adapt to day nr of AoC

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>

#include "../flcTimer.h"
#include "../parse_functions.h"
#include "my_utility.h"

// ==========   PROGRAM STAGING

enum eProgPhase {     // what stage are you in - set at start of main()
    EXAMPLE = 0,      //    1. start with hard coded examples to test your implemented solution on the example test sets
    TEST,             //    2. read the example test set from file, to test on your file reading and parsing
    PUZZLE            //    3. put your algorithm to work on the full scale puzzle data
} glbProgPhase;

std::string ProgPhase2string() {
    switch (glbProgPhase) {
        case EXAMPLE: return "EXAMPLE";
        case TEST   : return "TEST"   ;
        case PUZZLE : return "PUZZLE" ;
    }
    return "unknown";
}

// ==========   INPUT DATA STRUCTURES          <<===== adapt to match columns of input file

// the data consists of tuples (pairs) of numbers
typedef std::string DevName;
typedef struct {
    DevName sDevID;
    std::vector<DevName> vOutputs;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions


// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = { "aaa", { "you", "hhh"        }}; dData.push_back( cDatum );
    cDatum = { "you", { "bbb", "ccc"        }}; dData.push_back( cDatum );
    cDatum = { "bbb", { "ddd", "eee"        }}; dData.push_back( cDatum );
    cDatum = { "ccc", { "ddd", "eee", "fff" }}; dData.push_back( cDatum );
    cDatum = { "ddd", { "ggg"               }}; dData.push_back( cDatum );
    cDatum = { "eee", { "out"               }}; dData.push_back( cDatum );
    cDatum = { "fff", { "out"               }}; dData.push_back( cDatum );
    cDatum = { "ggg", { "out"               }}; dData.push_back( cDatum );
    cDatum = { "hhh", { "ccc", "fff", "iii" }}; dData.push_back( cDatum );
    cDatum = { "iii", { "out"               }}; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        DatumType cDatum;
        cDatum.sDevID = get_token_dlmtd( ": ", sLine );
        while (!sLine.empty()) {
            DevName sOutput = get_token_dlmtd( " ", sLine );
            cDatum.vOutputs.push_back( sOutput );
        }
        vData.push_back( cDatum );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndline = true ) {
    // define your datum printing code here
    std::cout << iData.sDevID << " --> ";
    for (auto e : iData.vOutputs) {
        std::cout << e << " ";
    }
    if (bEndline) {
        std::cout << std::endl;
    }
}

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    for (auto &e : dData) {
        PrintDatum( e );
    }
    std::cout << std::endl;
}

// ==========   PROGRAM STAGING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( DataStream &dData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( dData ); break;
        case TEST:    GetData_TEST(    dData ); break;
        case PUZZLE:  GetData_PUZZLE(  dData ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintDataStream( dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// ----- PART 1

//  I don't create a dedicated graph structure from the input data, since the input data can be seen
//  and used as a graph structure already. To use the input data as a graph structure, it's nice to have
//  the right getter functions on the input data.

// Returns the index of the graph node identified by label. If not found, returns -1
int GetNodeIndex( DataStream &graphData, const DevName &label ) {
    int nResult = -1;
    for (int i = 0; i < (int)graphData.size() && nResult < 0; i++) {
        if (graphData[i].sDevID == label) {
            nResult = i;
        }
    }
    return nResult;
}

// Returns the node ID (label) of the graph node indexed by nIndex.
DevName &GetNodeID( DataStream &graphData, int nIndex ) {
    CheckIndex( "GetNodeID()", nIndex, 0, (int)graphData.size());
    return graphData[nIndex].sDevID;
}

// Returns a reference to the std::vector of outputs of the graph node indexed by nIndex.
std::vector<DevName> &GetNodeOutputs( DataStream &graphData, int nIndex ) {
    CheckIndex( "GetNodeOutputs()", nIndex, 0, (int)graphData.size());
    return graphData[nIndex].vOutputs;
}

// recursive depht first search algorithm
long long DFS1( DataStream &graphData, const DevName &sCurNodeID, const DevName &sEndNodeID ) {

    long long llResult = -1;
    if (sCurNodeID == sEndNodeID) {
        // The algorithm found a path that lead to the final node.
        llResult = 1;
    } else {

        // prepare for recursive calls
        llResult = 0;
        int nCurNodeIndex = GetNodeIndex( graphData, sCurNodeID );
        if (nCurNodeIndex == -1) {
            std::cout << "ERROR: DFS1() --> could not find graph node with ID: " << sCurNodeID << std::endl;
        }
        std::vector<DevName> &vSuccessors = GetNodeOutputs( graphData, nCurNodeIndex );
        // recursively run the dfs algo on each successor node, and combine the results
        for (int i = 0; i < (int)vSuccessors.size(); i++) {
            DevName sNghbID = vSuccessors[i];
            llResult += DFS1( graphData, sNghbID, sEndNodeID );
        }
    }
    return llResult;
}

// ----- PART 2

// Some form of memoization is needed to speed up the dfs process for part 2.

// A memo contains the node id, both flags indicating whether "dac" or "fft" were visited and the resulting number of paths value
typedef struct sMemoElt {
    DevName   sID;
    bool      bVisitedDAC,
              bVisitedFFT;
    long long llResult;
} MemoElt;
typedef std::vector<MemoElt> MemoType;

// Look for the memo that contains the combination (sID, bDAC, bFFT) and return its index if found.
// Return -1 if not found.
int GetMemoIndex( MemoType &memo, const DevName &sID, bool bDAC, bool bFFT ) {
    int nResult = -1;

    // I wanna be sure on this bool comparison, so make it explicit
    auto bools_equal = []( bool a, bool b ) {
        return (a && b) || (!a && !b);
    };

    for (int i = 0; i < (int)memo.size() && nResult < 0; i++) {
        if (
            memo[i].sID == sID &&
            bools_equal( memo[i].bVisitedDAC, bDAC ) &&
            bools_equal( memo[i].bVisitedFFT, bFFT )
        ) {
            nResult = i;
        }
    }
    return nResult;
}

// returns true if the combination (sID, bDAC, bFFT) is in the memoization, false otherwise
bool IsInMemo( MemoType &memo, const DevName &sID, bool bDAC, bool bFFT ) {
    return GetMemoIndex( memo, sID, bDAC, bFFT ) >= 0;
}

// return memo result value using nIndex
long long GetMemoResult( MemoType &memo, int nIndex ) {
    CheckIndex( "GetMemoResult() [1]", nIndex, 0, memo.size());
    return memo[nIndex].llResult;
}

// return memo result value using (sID, bDAC, bFFT) combination
long long GetMemoResult( MemoType &memo, const DevName &sID, bool bDAC, bool bFFT ) {
    int nIndex = GetMemoIndex( memo, sID, bDAC, bFFT );
    CheckIndex( "GetMemoResult() [2]", nIndex, 0, memo.size());
    return memo[nIndex].llResult;
}

// store a new memo with the function parameters as its value
void PutMemoResult( MemoType &memo, const DevName &sID, bool bDAC, bool bFFT, long long llResult ) {
    MemoElt me = { sID, bDAC, bFFT, llResult };
    memo.push_back( me );
}

// a slightly more enhanced version of the depth first search algo from part 1:
//   * parameters are passed in the recursive call that indicate whether "dac" or "fft" have been visited
//   * memoization is applied to speed up the process
long long DFS2( DataStream &graphData, const DevName &sCurNodeID, const DevName &sEndNodeID, bool bVstdDAC, bool bVstdFFT, MemoType &memo ) {

    long long llResult = -1;
    if (sCurNodeID == sEndNodeID) {
        // The algorithm got upto the final node.
        if (bVstdDAC && bVstdFFT) {
            // If dac and fft were encountered in the process, then this path is valid, and accounts for 1
            llResult = 1;
        } else {
            // otherwise the path is not valid and accounts for 0
            llResult = 0;
        }
    } else {
        // first check if memoization can be used
        int nMemoIndex = GetMemoIndex( memo, sCurNodeID, bVstdDAC, bVstdFFT );
        if (nMemoIndex >= 0) {
            // return the previously stored result for this combination (sCurNodeID, bVstdDAC, bVstdFFT) if it exists
            llResult = GetMemoResult( memo, nMemoIndex );
        } else {

            // prepare for recursive calls on the successors of current node
            llResult = 0;
            bVstdDAC |= (sCurNodeID == "dac");
            bVstdFFT |= (sCurNodeID == "fft");

            int nCurNodeIndex = GetNodeIndex( graphData, sCurNodeID );
            if (nCurNodeIndex == -1) {
                std::cout << "ERROR: DFS2() --> could not find graph node with ID: " << sCurNodeID << std::endl;
            }
            std::vector<DevName> &vSuccessors = GetNodeOutputs( graphData, nCurNodeIndex );
            // recursively call the dfs algo on the successors, combining the result
            for (int i = 0; i < (int)vSuccessors.size(); i++) {
                llResult += DFS2( graphData, vSuccessors[i], sEndNodeID, bVstdDAC, bVstdFFT, memo );
            }
        }
    }
    if (llResult < 0) {
        std::cout << "ERROR: DFS2() --> (intermediate) result is negative: " << llResult << std::endl;
    }

    // store the result for this combination (sCurNodeID, bVstdDAC, bVstdFFT) for future use
    PutMemoResult( memo, sCurNodeID, bVstdDAC, bVstdFFT, llResult );

    return llResult;
}

// ==========   MAIN()

int main()
{
    glbProgPhase = PUZZLE;     // program phase to EXAMPLE, TEST or PUZZLE
    std::cout << "Phase: " << ProgPhase2string() << std::endl << std::endl;
    flcTimer tmr;

/* ========== */   tmr.StartTiming();   // ============================================vvvvv

    // get input data, depending on the glbProgPhase (example, test, puzzle)
    DataStream inputData;
    GetInput( inputData, glbProgPhase != PUZZLE );
    DataStream part2Data = inputData;
    std::cout << "Data stats - size of data stream " << inputData.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here

//    long long llResult1 = DFS1( inputData, "you", "out" );
//
//    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << llResult1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    MemoType c;
    long long llResult2 = DFS2( inputData, "svr", "out", false, false, c );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llResult2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
