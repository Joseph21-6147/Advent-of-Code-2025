// AoC 2025 - Day 04 - Printing Department
// =======================================

// date:  2025-12-04
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day04"          // <<===== adapt to day nr of AoC

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>

#include "../flcTimer.h"
#include "../parse_functions.h"
#include "my_utility.h"

#include "vector_types.h"            // used for modeling coordinates

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

// the data consists of a grid, modelled as a std::vector of std::string
// the grid contains '.' (empty spaces) and '@' (rolls of paper)
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "..@@.@@@@."; dData.push_back( cDatum );
    cDatum = "@@@.@.@.@@"; dData.push_back( cDatum );
    cDatum = "@@@@@.@.@@"; dData.push_back( cDatum );
    cDatum = "@.@@@@..@."; dData.push_back( cDatum );
    cDatum = "@@.@@@@.@@"; dData.push_back( cDatum );
    cDatum = ".@@@@@@@.@"; dData.push_back( cDatum );
    cDatum = ".@.@.@.@@@"; dData.push_back( cDatum );
    cDatum = "@.@@@.@@@@"; dData.push_back( cDatum );
    cDatum = ".@@@@@@@@."; dData.push_back( cDatum );
    cDatum = "@.@.@@@.@."; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        vData.push_back( sLine );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndline = true ) {
    // define your datum printing code here
    std::cout << iData;
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

// returns true if the coordinate pair (x, y) is not in bounds of the grid dData
bool IsOutOfBounds( DataStream &dData, int x, int y ) {
    if (x < 0 || x >= (int)dData[0].length()) { return true; }
    if (y < 0 || y >= (int)dData.size()) {      return true; }
    return false;
}

// returns true if out of bounds or '.' in the grid at (x, y)
bool IsEmpty( DataStream &dData, int x, int y ) {
    if (IsOutOfBounds( dData, x, y )) return true;
    return dData[y][x] == '.';
}

// returns true if in bounds and '@' in the grid at (x, y)
bool IsRoll( DataStream &dData, int x, int y ) {
    if (IsOutOfBounds( dData, x, y )) return false;
    return dData[y][x] == '@';
}

// accumulate and return the number of neighbours that is a paper roll
int NrNeighbourRolls( DataStream &dData, int x, int y ) {
    int nOccupied = 0;
    if (IsRoll( dData, x - 1, y - 1 )) nOccupied += 1;
    if (IsRoll( dData, x    , y - 1 )) nOccupied += 1;
    if (IsRoll( dData, x + 1, y - 1 )) nOccupied += 1;
    if (IsRoll( dData, x - 1, y     )) nOccupied += 1;
    if (IsRoll( dData, x + 1, y     )) nOccupied += 1;
    if (IsRoll( dData, x - 1, y + 1 )) nOccupied += 1;
    if (IsRoll( dData, x    , y + 1 )) nOccupied += 1;
    if (IsRoll( dData, x + 1, y + 1 )) nOccupied += 1;
    return nOccupied;
}

// a paper roll can be moved/processed/worked on if has less than 4 neighbouring rolls (in 8-connected grid)
bool IsWorkable( DataStream &dData, int x, int y ) {
    return IsRoll( dData, x, y ) && (NrNeighbourRolls( dData, x, y ) < 4);
}

// use a std::set to get the unique positions for the paper rolls that are workable
std::set<vi2d> GetRollPositions( DataStream &dData ) {
    std::set<vi2d> sResult;

    for (int y = 0; y < (int)dData.size(); y++) {
        for (int x = 0; x < (int)dData[0].length(); x++) {
            if (IsWorkable( dData, x, y )) {
                sResult.insert( vi2d( x, y ));
            }
        }
    }
    return sResult;
}

// ----- PART 2

// no additional functions needed - solution for part 2 is coded directly from the functions for part 1

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

    // just call the function to get all unique paper roll positions that are workable...
    std::set<vi2d> glbRollPositions = GetRollPositions( inputData );

//    for (auto e : glbRollPositions) {
//        std::cout << e.ToString() << std::endl;
//    }

    // ... and report its size
    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << glbRollPositions.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    int nCacheRemoved = 0;
    int nTotalRemoved = 0;

    glbRollPositions.clear();

    // keep removing workable paper rolls, until no paper rolls are workable anymore
    // accumulate the number of removed paper rolls
    do {
        glbRollPositions = GetRollPositions( inputData );
        nCacheRemoved = glbRollPositions.size();
        nTotalRemoved += nCacheRemoved;
        for (auto pos : glbRollPositions) {
            inputData[pos.y][pos.x] = '.';
        }
//        std::cout << "Removed " << nCacheRemoved << " rolls this iteration, cumulated: " << nTotalRemoved << std::endl;

    } while (nCacheRemoved > 0);

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << nTotalRemoved << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
