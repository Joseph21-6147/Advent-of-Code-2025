// AoC 2025 - Day 08 - Playground
// ==============================

// date:  2025-12-08
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day08"          // <<===== adapt to day nr of AoC

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>

#include "../flcTimer.h"
#include "../parse_functions.h"
#include "my_utility.h"
#include "vector_types.h"

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

// the data consists of junction boxes which have a 3d location and may be connected to a circuit
typedef struct {
    vi3d pos;
    int circuitIx = -1;
} DatumType;
typedef std::vector<DatumType> DataStream;

int GetCircuitID( DatumType &d ) {
    return d.circuitIx;
}

bool IsConnected( DatumType &d ) {
    return GetCircuitID( d ) != -1;
}

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { { 162, 817, 812 }, -1 }; dData.push_back( cDatum );
    cDatum = { {  57, 618,  57 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 906, 360, 560 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 592, 479, 940 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 352, 342, 300 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 466, 668, 158 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 542,  29, 236 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 431, 825, 988 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 739, 650, 466 }, -1 }; dData.push_back( cDatum );
    cDatum = { {  52, 470, 668 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 216, 146, 977 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 819, 987,  18 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 117, 168, 530 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 805,  96, 715 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 346, 949, 466 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 970, 615,  88 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 941, 993, 340 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 862,  61,  35 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 984,  92, 344 }, -1 }; dData.push_back( cDatum );
    cDatum = { { 425, 690, 689 }, -1 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sX, sY, sZ;
        sX = get_token_dlmtd( ",", sLine );
        sY = get_token_dlmtd( ",", sLine );
        sZ =                       sLine  ;
        DatumType cDatum = { { stoi( sX ), stoi( sY ), stoi( sZ ) }, -1 };
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
    std::cout << "junction at: " << iData.pos.ToString() << " " << "part of circuit: " << iData.circuitIx;
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

// returns straight line distance between (3D) positions of a and b
float GetDistance( vi3d &a, vi3d &b ) {
    return (vf3d( a ) - vf3d( b )).length();
}

// typedefs to create a (sorted) distance list
typedef struct {
    int junctionA, junctionB;
    float fDistance;
} DistanceType;
typedef std::vector<DistanceType> DistanceStream;


void PrintDistanceType( DistanceType &d, DataStream &ds ) {
    std::cout << "Distance: "         << d.fDistance
              << ", junction A ID = " << d.junctionA
              << ", junction B ID = " << d.junctionB << " --> ";
    PrintDatum( ds[d.junctionA], false );
    std::cout << " and ";
    PrintDatum( ds[d.junctionB], true );
}

void PrintDistanceStream( DistanceStream &vD, DataStream &ds ) {
    for (auto e : vD) {
        PrintDistanceType( e, ds );
    }
    std::cout << std::endl;
}

// generates and returns an exhaustive list of all possible pairs of junctions in vData, including their distance
DistanceStream GetSortedDistancesList( DataStream &vData ) {
    // create the list of distances between all possible pairs of junctions in vData
    DistanceStream vResult;
    for (int i = 0; i < (int)vData.size() - 1; i++) {
        for (int j = i + 1; j < (int)vData.size(); j++) {
            float fDist = GetDistance( vData[i].pos, vData[j].pos );
            DistanceType aux = { i, j, fDist };
            vResult.push_back( aux );
        }
    }
    // sort the list in increasing distance value
    std::sort(
        vResult.begin(),
        vResult.end(),
        []( DistanceType &a, DistanceType &b ) {
            return a.fDistance < b.fDistance;
        }
    );

    return vResult;
}

// typedefs to organize junctions into circuits
typedef std::vector<int> CircuitType;
typedef std::vector<CircuitType> CircuitStream;

void PrintCircuitType( CircuitType &ct ) {
    std::cout << "Circuit size: " << ct.size() << " --> ";
    for (auto e : ct) {
        std::cout << e << ", ";
    }
    std::cout << std::endl;
}

void PrintCircuitStream( CircuitStream &cs ) {
    for (auto e : cs) {
        PrintCircuitType( e );
    }
    std::cout << std::endl;
}

// For part 1 you need a table with the statistics of the circuits. More specific: you need the three largest circuits
// and the number of junctions each of them holds. This function counts and reports these "circuit statistics".
void PrintCircuitStatistics( CircuitStream &cs, DataStream &ds, bool bVerbose = true ) {
    // prepare a vector with nLimit elements for counting:
    // vCounts[i] will hold the number of circuits that contain i junctions
    std::vector<int> vCounts;
    int nLimit = 2000;             // this value is determined empirically
    for (int i = 0; i < nLimit; i++) {
        vCounts.push_back( 0 );
    }
    // gather the size statistics
    for (auto e : cs) {
        int nSize = (int)e.size();
        if (nSize >= nLimit) {
            std::cout << "PrintCircuitStatistics() --> too large number " << nSize << std::endl;
        } else {
            vCounts[nSize] += 1;
        }
    }
    // count and report the circuit statistics for 2 or more junctions
    // collect the top three in the process
    int nJunctionsInCircuit = 0;
    std::vector<long long> vAnswer;
    int nAnswerCount = 0;
    for (int i = nLimit - 1; i >= 0; i--) {
        if (vCounts[i] > 0) {
            if (bVerbose) {
                std::cout << vCounts[i] << " circuits have size " << i << std::endl;
            }
            nJunctionsInCircuit += (vCounts[i] * i);
            if (nAnswerCount < 3) {
                vAnswer.push_back( i );
                nAnswerCount += 1;
            }
        }
    }
    // count the number of isolated junctions (that are not part of any circuit)
    int nIsolatedJunctions = 0;
    for (auto e : ds) {
        if (!IsConnected( e )) {
            nIsolatedJunctions += 1;
        }
    }
    if (bVerbose) {
        std::cout << "junctions - in circuits = " << nJunctionsInCircuit
                  << ", isolated = "              << nIsolatedJunctions
                  << ", total = "                 << nJunctionsInCircuit + nIsolatedJunctions << std::endl;
    }

    // calculate and report the top 3 sizes multiplied
    long long llTotalAnswer = 1;
    for (int i = 0; i < nAnswerCount; i++) {
        llTotalAnswer *= vAnswer[i];
    }
    std::cout << "Top 3 sizes multiplied = " << llTotalAnswer << std::endl;
}

// This function connects two junctions identified by indices ixA and ixB into DataStream ds
// The result of the connection is both stored in CircuitStream cs, and in the circuitIx values
// of the ds elements.
//
// When connecting two junctions there are four possible scenarios:
//   1  neither A nor B is connected --> connect A and B into a new circuit
//   2  A is connected, B is not     --> Add B to the circuit of A
//   3  B is connected, A is not     --> Add A to the circuit of B
//   4  A and B are both connected AND
//          to the same circuit      --> do nothing
//          to different circuits    --> get the lower indexed circuit,
//                                       combine the other into this circuit and
//                                       dispose the higher indexed circuit
//
// These four scenarios are implemented below
void Connect( CircuitStream &cs, DataStream &ds, int ixA, int ixB ) {

    bool bAconnected = IsConnected( ds[ixA] );
    bool bBconnected = IsConnected( ds[ixB] );

    // case 1: neither A nor B is connected --> connect both into a new circuit
    if (!bAconnected && !bBconnected) {

        CircuitType newCircuit;          // create a new circuit and put into circuit stream
        newCircuit.push_back( ixA );
        newCircuit.push_back( ixB );
        cs.push_back( newCircuit );

        int nCircuitIx = cs.size() - 1;  // set the circuit indices of the junctions
        ds[ixA].circuitIx = nCircuitIx;
        ds[ixB].circuitIx = nCircuitIx;

    } else
    // case 2: A is connected, B is not --> add B into the circuit of A
    if (bAconnected && !bBconnected) {

        int nCircuitIx = ds[ixA].circuitIx;  // get A's circuit ID
        cs[nCircuitIx].push_back( ixB );     // add B to that circuit
        ds[ixB].circuitIx = nCircuitIx;      // set B's circuit ID

    } else
    // case 3: B is connected, A is not --> add A into the circuit of B
    if (!bAconnected && bBconnected) {

        int nCircuitIx = ds[ixB].circuitIx;  // get B's circuit ID
        cs[nCircuitIx].push_back( ixA );     // add A to that circuit
        ds[ixA].circuitIx = nCircuitIx;      // set A's circuit ID

    } else
    // case 4: both are connected ...
    if (bAconnected && bBconnected) {
        int nAcID = GetCircuitID( ds[ixA] );
        int nBcID = GetCircuitID( ds[ixB] );
        if (nAcID == nBcID) {
            // ... to the same circuit --> do nothing
        } else {
            // ... to different circuits
            if (nAcID < nBcID) {
                // combine circuit B into circuit A and dispose of B
                for (auto junctionB : cs[nBcID]) {
                    cs[nAcID].push_back( junctionB );
                    ds[junctionB].circuitIx = nAcID;
                }
                cs[nBcID].clear();  // Note that the circuit is not removed but only cleared!
            } else {
                // combine circuit A into circuit B and dispose of A
                for (auto junctionA : cs[nAcID]) {
                    cs[nBcID].push_back( junctionA );
                    ds[junctionA].circuitIx = nBcID;
                }
                cs[nAcID].clear();  // Note that the circuit is not removed but only cleared!
            }
        }
    } else {
        std::cout << "ERROR: Connect() --> can't figure out this scenario!" << std::endl;
    }
}

// ----- PART 2

// returns true if all junctions in ds are connected to the same circuit
bool AllConnected( DataStream &ds ) {
    bool bResult = IsConnected( ds[0] );
    int nCircuitID = GetCircuitID( ds[0] );

    for (int i = 1; i < (int)ds.size() && bResult; i++) {
        bResult = (GetCircuitID( ds[i] ) == nCircuitID);
    }
    return bResult;
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

//    std::cout << "Creating distance list...";
    DistanceStream vDistances = GetSortedDistancesList( inputData );
//    std::cout << " done! Distances list contains " << vDistances.size() << " elements." << std::endl;

    // for part 1, just create 1000 connections and check on the circuit statistics
    CircuitStream vCircuits;
    for (int i = 0; i < 1000; i++) {
        Connect( vCircuits, inputData, vDistances[i].junctionA, vDistances[i].junctionB );
    }
//    std::cout << std::endl << "1000 connections made, getting circuit statistics: " << std::endl;
    PrintCircuitStatistics( vCircuits, inputData, false );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: <see above>" << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    for (auto &e : vCircuits) { e.clear(); }
    vCircuits.clear();

    // for part 2 - make connections until all junctions are connected in the same circuit
    int i = 0;
    int nCacheJunctionA, nCacheJunctionB;
    do {
        nCacheJunctionA = vDistances[i].junctionA;
        nCacheJunctionB = vDistances[i].junctionB;

        Connect( vCircuits, inputData, vDistances[i].junctionA, vDistances[i].junctionB );

        i += 1;
    } while (!AllConnected( inputData ));

//    std::cout << std::endl << "A total of: " << i << " connections were made, the last two junctions were: " << std::endl;
//    PrintDatum( inputData[nCacheJunctionA] );
//    std::cout << " and " << std::endl;
//    PrintDatum( inputData[nCacheJunctionB] );

    // compute the wall distance from the last two junctions' X coordinate
    long long llXA = inputData[nCacheJunctionA].pos.x;
    long long llXB = inputData[nCacheJunctionB].pos.x;
    long long llWallDist = llXA * llXB;

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llWallDist << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}















//        Timing 0 - input data preparation: 1.0555 msec
//
//    Answer to day08 part 1: 127551
//
//        Timing 1 - solving puzzle part 1: 38.4545 msec
//
//    Answer to day08 part 2: 2347225200
//
//        Timing 2 - solving puzzle part 2: 1.1005 msec
//
//    Process returned 0 (0x0)   execution time : 0.094 s
