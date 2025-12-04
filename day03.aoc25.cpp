// AoC 2025 - Day 03 - Lobby
// =========================

// date:  2025-12-03
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day03"          // <<===== adapt to day nr of AoC

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

// the data consists of banks of batteries, modeled as digits in a std::string
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "987654321111111"; dData.push_back( cDatum );
    cDatum = "811111111111119"; dData.push_back( cDatum );
    cDatum = "234234234234278"; dData.push_back( cDatum );
    cDatum = "818181911112111"; dData.push_back( cDatum );
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
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here
    std::cout << iData << std::endl;
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

// we need two digits from a string. The first one needs to be at large as possible,
// but it cannot be the last digit since there has to be a second one
int FindFirstIndex( std::string &sBank, int nLen ) {
    int nFound = 0;
    char cResult = '0';
    for (int i = 0; i < nLen - 1; i++) {
        if (sBank[i] > cResult) {
            nFound = i;
            cResult = sBank[i];
        }
    }
    return nFound;
}

// the index of the first digit is passed as nFstIx, just find the largest digit from
// the remainder of the string and return its index
int FindSecndIndex( std::string &sBank, int nLen, int nFstIx ) {
    int nFound = nFstIx + 1;
    char cResult = '0';
    for (int i = nFound; i < nLen; i++) {
        if (sBank[i] > cResult) {
            nFound = i;
            cResult = sBank[i];
        }
    }
    return nFound;
}

// ----- PART 2

// Generalized version of index searching functions from part 1:
//   sBank is the battery bank, having nLen digits
//   nCurIx is the index of the previously "locked" digit
//   n is the number of digits to keep free at the end of the string, since more digits need to be locked
int FindNthIndex( std::string &sBank, int nLen, int nCurIx, int n ) {
    int nFound = nCurIx + 1;
    char cResult = '0';
    for (int i = nFound; i < nLen - n; i++) {
        if (sBank[i] > cResult) {
            nFound = i;
            cResult = sBank[i];
        }
    }
    return nFound;
}

// Returns a std::vector of indices that indicate the largest possible Joltage
// for battery bank sBank
std::vector<int> FindTwelveIndices( std::string &sBank ) {
    std::vector<int> vResult;
    int nLen = sBank.length();
    int nCurIx = -1;

    for (int i = 11; i >= 0; i--) {
        int nIx = FindNthIndex( sBank, nLen, nCurIx, i );
        vResult.push_back( nIx );
        nCurIx = nIx;
    }
    return vResult;
}

// Creates and returns a long long integer from the std::vector of indices into sBank
long long IndicesToNumber( std::string &sBank, std::vector<int> &vIndices ) {
    long long power = 1;
    long long llResult = 0;
    for (int i = 11; i >= 0; i--) {
        llResult += (power * (sBank[vIndices[i]] - '0'));
        power *= 10;
    }
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

    int nTotal = 0;

    for (int i = 0; i < (int)inputData.size(); i++) {
        std::string sBank = inputData[i];
        int nLen = sBank.length();
        // get indices to the first and second battery
        int nIx1 = FindFirstIndex( sBank, nLen );
        int nIx2 = FindSecndIndex( sBank, nLen, nIx1 );
        // convert them to a number - knowing that there are always 2 digits
        std::string sNr = "  ";
        sNr[0] = sBank[nIx1];
        sNr[1] = sBank[nIx2];
        int nJoltage = stoi( sNr );
        nTotal += nJoltage;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << nTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    long long llTotal = 0;

    for (int i = 0; i < (int)inputData.size(); i++) {
        // get indices to the 12 batteries producing the largest Joltage
        std::vector<int> vIndices = FindTwelveIndices( inputData[i] );
        // convert to a number
        long long llJoltage = IndicesToNumber( inputData[i], vIndices );
        llTotal += llJoltage;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
