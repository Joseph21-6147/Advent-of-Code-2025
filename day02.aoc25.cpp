// AoC 2025 - Day 02 - Gift Shop
// =============================

// date:  2025-12-02
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day02"          // <<===== adapt to day nr of AoC

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
// use long long, since the input data contains large value integers
typedef struct {
    long long id1, id2;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = {         11,         22 }; dData.push_back( cDatum );
    cDatum = {         95,        115 }; dData.push_back( cDatum );
    cDatum = {        998,       1012 }; dData.push_back( cDatum );
    cDatum = { 1188511880, 1188511890 }; dData.push_back( cDatum );
    cDatum = {     222220,     222224 }; dData.push_back( cDatum );
    cDatum = {    1698522,    1698528 }; dData.push_back( cDatum );
    cDatum = {     446443,     446449 }; dData.push_back( cDatum );
    cDatum = {   38593856,   38593862 }; dData.push_back( cDatum );
    cDatum = {     565653,     565659 }; dData.push_back( cDatum );
    cDatum = {  824824821,  824824827 }; dData.push_back( cDatum );
    cDatum = { 2121212118, 2121212124 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    getline( dataFileStream, sLine );

    while (!sLine.empty()) {
        std::string s1, s2, st;
        st = get_token_dlmtd( ",", sLine );
        split_token_dlmtd( "-", st, s1, s2 );
        DatumType cDatum = { stoll( s1 ), stoll( s2 ) };
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
    std::cout << iData.id1 << "-" << iData.id2;
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

// ----- PART 1

// An id is NOT valid IFF
//   1. it's length is an even number, and
//   2. it's two halves are identical
bool IsValid( long long id ) {
    bool bResult = true;               // assume valid until proven otherwise
    std::string sId = to_string( id );
    int sLen = sId.length();
    if (sLen % 2 == 0) {
        // split string in two parts
        std::string sLeft, sRght;
        split_token_sized( sLen / 2, sId, sLeft, sRght );
        if (sLeft == sRght) {
            bResult = false;
        }
    }
    return bResult;
}

// ----- PART 2

// checks validity of the id that is passed as a token, and a divisor value nPartLen
// ASSUMPTION: nPartLen must indeed be a divisor of the length of sID
bool CheckCase( std::string sID, int nPartLen ) {
    bool bValid = false;    // assume IN-valid until proven otherwise

    // get the pieces of sID into a std::vector
    std::vector<std::string> vTokens;
    int nParts = sID.length() / nPartLen;
    for (int i = 0; i < nParts; i++) {
        std::string s = get_token_sized( nPartLen, sID );
        vTokens.push_back( s );
    }
    // check pieces pairwise against each other
    bool bIdentical = true;
    for (int i = 0; i < nParts - 1 && bIdentical; i++) {
        bIdentical = (vTokens[i] == vTokens[i + 1]);
    }
    // sID is invalid if all the pieces are identical
    bValid = !bIdentical;
    return bValid;
}

// Brute for through all possible part lengths of id. If id can be split into
// multiple pieces without a remainder, we found a divisor, and use
// CheckCase() for checking that situation
bool IsValid2( long long id ) {
    bool bValid = true;                 // assume valid until proven otherwise
    std::string sId = to_string( id );
    int sLen = sId.length();
    for (int i = sLen - 1; i > 0 && bValid; i--) {
        // check case only if i is a divisor of sLen
        if (sLen % i == 0) {
            std::string sCopy = sId; // the parameter is altered, so use a copy
            bValid = CheckCase( sCopy, i );
        }
    }
    return bValid;
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
    long long llResult = 0;

    for (int i = 0; i < (int)inputData.size(); i++) {
        // process the range for one input datum
        DatumType d = inputData[i];
        for (long long j = d.id1; j <= d.id2; j++) {
            if (!IsValid( j )) {
                llResult += j;
            }
        }
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << llResult << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    llResult = 0;

    for (int i = 0; i < (int)inputData.size(); i++) {
        // process the range for one input datum
        DatumType d = inputData[i];
        for (long long j = d.id1; j <= d.id2; j++) {
            if (!IsValid2( j )) {
                llResult += j;
            }
        }
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llResult << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
