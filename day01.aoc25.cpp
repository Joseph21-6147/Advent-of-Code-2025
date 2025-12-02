// AoC 2025 - Day 01 - Secret Entrance
// ===================================

// date:  2025-12-01
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day01"          // <<===== adapt to day nr of AoC

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
typedef struct {
    int nr;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum.nr = -68; dData.push_back( cDatum );   // L68 becomes -68,
    cDatum.nr = -30; dData.push_back( cDatum );
    cDatum.nr = +48; dData.push_back( cDatum );   // R48 becomes +48
    cDatum.nr = - 5; dData.push_back( cDatum );
    cDatum.nr = +60; dData.push_back( cDatum );
    cDatum.nr = -55; dData.push_back( cDatum );
    cDatum.nr = - 1; dData.push_back( cDatum );
    cDatum.nr = -99; dData.push_back( cDatum );
    cDatum.nr = +14; dData.push_back( cDatum );
    cDatum.nr = -82; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sLeft, sRght;
        split_token_sized( 1, sLine, sLeft, sRght );
        DatumType cDatum;
        switch (sLeft[0]) {
            case 'L': cDatum.nr = - stoi( sRght ); break;
            case 'R': cDatum.nr = + stoi( sRght ); break;
            default : std::cout << "ERROR: ReadInputData() --> prefix character: " << sLeft << std::endl;
        }
        vData.push_back( cDatum );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndLine = true ) {
    // define your datum printing code here
    if (iData.nr < 0) {
        std::cout << "L" << - iData.nr;
    } else {
        std::cout << "R" <<   iData.nr;
    }
    if (bEndLine) std::cout << std::endl;
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

// just making sure that result of mod and div are allways >= 0

// ----- PART 1

int my_mod( int nr, int oper ) {
    int nResult = nr;
    while (nResult >= oper) { nResult -= oper; }
    while (nResult <     0) { nResult += oper; }
    return nResult;
}

int my_div( int nr, int oper ) {
    int nResult = (nr < 0 ? -( nr / oper) : (nr / oper));
    return nResult;
}

// ----- PART 2



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
    int nDialValue = 50, nZeros = 0;
//    std::cout << "the dial starts by pointing at: " << nDialValue << ", nr of zeros = " << nZeros << std::endl;

    for (int i = 0; i < (int)inputData.size(); i++) {
        nDialValue = my_mod( nDialValue + inputData[i].nr, 100 );
        if (nDialValue == 0) {
            nZeros += 1;
        }

//        std::cout << "the dial is rotated: ";
//        PrintDatum( inputData[i], false );
//        std::cout << " to point at: " << nDialValue << ", nr of zeros = " << nZeros<< std::endl;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1 = " << nZeros << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    nDialValue = 50;
    nZeros = 0;
//    std::cout << "the dial starts by pointing at: " << nDialValue << ", nr of zeros = " << nZeros << std::endl;


    for (int i = 0; i < (int)inputData.size(); i++) {
        int nDiv = my_div( inputData[i].nr, 100 );
        int nNewValue = my_mod( nDialValue + inputData[i].nr, 100 );

        // the case of the dial value being zero is tested separately, so exclude it from
        // the test for "going through" zero
        if (inputData[i].nr < 0) {                          // rotate dial to left
            if (nNewValue > nDialValue && nDialValue > 0) {
                nZeros += 1;
            }
        } else {                                            // rotate dial to right
            if (nNewValue < nDialValue && nNewValue != 0) {
                nZeros += 1;
            }
        }

        if (nNewValue == 0) {
            nZeros += 1;
        }
        nZeros += nDiv;
        nDialValue = nNewValue;

//        std::cout << "the dial is rotated: ";
//        PrintDatum( inputData[i], false );
//        std::cout << " to point at: " << nDialValue << ", nr of zeros = " << nZeros<< std::endl;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2 = " << nZeros << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
