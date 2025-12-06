// AoC 2025 - Day 05 - Cafeteria
// =============================

// date:  2025-12-05
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day05"          // <<===== adapt to day nr of AoC

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

// for todays puzzle there's two types of input data:
// inclusive ranges (modeled as a struct with long longs as boundaries) ...
typedef struct {
    long long low, hgh;
} RangeType;
typedef std::vector<RangeType> RangeStream;

// ... and ID's (modeled as long long)
typedef long long DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( RangeStream &rData, DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    RangeType rDatum;
    rDatum = {  3,  5 }; rData.push_back( rDatum );
    rDatum = { 10, 14 }; rData.push_back( rDatum );
    rDatum = { 16, 20 }; rData.push_back( rDatum );
    rDatum = { 12, 18 }; rData.push_back( rDatum );

    DatumType cDatum;
    cDatum =  1; dData.push_back( cDatum );
    cDatum =  5; dData.push_back( cDatum );
    cDatum =  8; dData.push_back( cDatum );
    cDatum = 11; dData.push_back( cDatum );
    cDatum = 17; dData.push_back( cDatum );
    cDatum = 32; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, RangeStream &rData, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;

    // the transition from ranges to id's in the input file is denoted by an empty line
    // hence the check on the line length
    while (getline( dataFileStream, sLine ) && sLine.length() > 0) {
        std::string sLeft, sRght;
        split_token_dlmtd( "-", sLine, sLeft, sRght );
        RangeType rDatum = { stoll( sLeft ), stoll( sRght ) };
        rData.push_back( rDatum );
    }

    while (getline( dataFileStream, sLine )) {
        DatumType cDatum = stoll( sLine );
        vData.push_back( cDatum );
    }
    dataFileStream.close();
}

void GetData_TEST(   RangeStream &rData, DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, rData, dData ); }
void GetData_PUZZLE( RangeStream &rData, DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, rData, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// output to console for testing

void PrintRange( RangeType &rData, bool bEndline = true ) {
    // define your datum printing code here
    std::cout <<   "low: " << rData.low;
    std::cout << ", hgh: " << rData.hgh;
    if (bEndline) {
        std::cout << std::endl;
    }
}

void PrintDatum( DatumType &iData, bool bEndline = true ) {
    // define your datum printing code here
    std::cout << "ID: " << iData;
    if (bEndline) {
        std::cout << std::endl;
    }
}

// output to console for testing

void PrintRangeStream( RangeStream &rData ) {
    for (auto &r : rData) {
        PrintRange( r );
    }
    std::cout << std::endl;
}

void PrintDataStream( RangeStream &rData, DataStream &dData ) {
    for (auto &r : rData) {
        PrintRange( r );
    }
    std::cout << std::endl;

    for (auto &e : dData) {
        PrintDatum( e );
    }
    std::cout << std::endl;
}

// ==========   PROGRAM STAGING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( RangeStream &rData, DataStream &dData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( rData, dData ); break;
        case TEST:    GetData_TEST(    rData, dData ); break;
        case PUZZLE:  GetData_PUZZLE(  rData, dData ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintDataStream( rData, dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// ----- PART 1

// returns true if id d falls into the range r
bool IsInRange( DatumType &d, RangeType &r ) {
    return r.low <= d && d <= r.hgh;
}

// returns true if there's any range in rs that contains id d
bool IsFresh( DatumType &d, RangeStream &rs ) {
    bool bFound = false;
    for (int i = 0; i < (int)rs.size() && !bFound; i++) {
        bFound = IsInRange( d, rs[i] );
    }
    return bFound;
}

// ----- PART 2

// returns true if ranges a and b are NOT overlapping
bool NotOverlapping( RangeType &a, RangeType &b ) {
    return a.hgh < b.low || b.hgh < a.low;
}

// returns true if ranges a and b ARE overlapping
bool AreOverlapping( RangeType &a, RangeType &b ) {
    return !NotOverlapping( a, b );
}

// Sets a to be the combination of a and b. Sets b to be invalid (with boundary values of -1)
// ASSUMPTION: a and b are overlapping!
void Combine( RangeType &a, RangeType &b ) {
    RangeType res = {
        std::min( a.low, b.low ),
        std::max( a.hgh, b.hgh )
    };
    a = res;
    b.low = b.hgh = -1;
}

// when a range is combined into another range, I flag it as invalid by
// setting it's borders to -1
bool IsInvalidRange( RangeType &r ) {
    return r.low < 0 && r.hgh < 0;
}

// ==========   MAIN()

int main()
{
    glbProgPhase = PUZZLE;     // program phase to EXAMPLE, TEST or PUZZLE
    std::cout << "Phase: " << ProgPhase2string() << std::endl << std::endl;
    flcTimer tmr;

/* ========== */   tmr.StartTiming();   // ============================================vvvvv

    // get input data, depending on the glbProgPhase (example, test, puzzle)
    RangeStream rangeData;
    DataStream inputData;
    GetInput( rangeData, inputData, glbProgPhase != PUZZLE );
    RangeStream part2Range = rangeData;
    DataStream part2Data = inputData;
    std::cout << "Data stats - size of data stream " << rangeData.size() + inputData.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here
    int nFreshCount = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        if (IsFresh( inputData[i], rangeData )) {
            nFreshCount += 1;
        }
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << nFreshCount << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data
    rangeData = part2Range;

    // part 2 code here

    // first combine all ranges as much as possible - note that new overlap may become possible
    // as a result of a Combine() action, so continue combining until all overlaps are processed
    bool bChanged;
    do {
        bChanged = false;

        // attempt to combine each range against each other (but skip invalid ranges)
        for (int i = 0; i < (int)rangeData.size() - 1; i++) {
            if (!IsInvalidRange( rangeData[i] )) {
                for (int j = i + 1; j < (int)rangeData.size(); j++) {
                    if (!IsInvalidRange( rangeData[j] )) {

                        if (AreOverlapping( rangeData[i], rangeData[j] )) {
                            Combine( rangeData[i], rangeData[j] );
                            bChanged = true;
                        }
                    }
                }
            }
        }
    } while (bChanged);

    // after no more combining is possible, the set of ranges provides the unique set of fresh id's
    long long llTotal = 0;
    for (int i = 0; i < (int)rangeData.size(); i++) {
        if (!IsInvalidRange( rangeData[i] )) {
            long long llRangeValue = rangeData[i].hgh - rangeData[i].low + 1;
            llTotal += llRangeValue;
        }
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
