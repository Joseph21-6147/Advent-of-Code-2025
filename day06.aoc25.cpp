// AoC 2025 - Day 06 - Trash Compactor
// ===================================

// date:  2025-12-06
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day06"          // <<===== adapt to day nr of AoC

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

// the data consists of lines of tokens, the first (n-1) lines are numbers (operands), the last line contains the operators
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions


// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "123 328  51 64 "; dData.push_back( cDatum );
    cDatum = " 45 64  387 23 "; dData.push_back( cDatum );
    cDatum = "  6 98  215 314"; dData.push_back( cDatum );
    cDatum = "*   +   *   +  "; dData.push_back( cDatum );
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
    std::cout << "line: _" << iData << "_";
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

// ----- PART 2

bool OnlySpaces( std::string &s ) {
    bool bResult = true;
    for (int i = 0; i < (int)s.length() && bResult; i++) {
        bResult &= s[i] == ' ';
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
    int nOperatorIx = inputData.size() - 1;
    long long llGrandTotal = 0;

    while (inputData[0].length() > 0) {

        // first determine token length using the operator line
        int nTokenLength = -1;
        for (int i = 1; i < (int)inputData[nOperatorIx].length() && nTokenLength == -1; i++) {
            if (inputData[nOperatorIx][i] != ' ') {
                nTokenLength = i;
            }
        }
        if (nTokenLength == -1) {
            nTokenLength = inputData[nOperatorIx].length();
        }

        // then determine the operator ...
        char cOperator = inputData[nOperatorIx][0];

        // ... and get the operands in a std::vector, using the previously determined token length
        std::vector<std::string> vOperands;
        for (int i = 0; i < (int)inputData.size(); i++) {
            std::string aux = get_token_sized( nTokenLength, inputData[i] );
            // the operator line needs to be stripped as well, but must not be storen in vOperands
            if (i < (int)inputData.size() - 1) {
                vOperands.push_back( aux );
            }
        }

        // having the operator and operands worked out, combine them into a local sum answer
        long long llLocalTotal;
        if (cOperator == '+') {

            llLocalTotal = 0;
            for (auto e : vOperands) {
                llLocalTotal += stoll( e );
            }
        } else if (cOperator == '*') {

            llLocalTotal = 1;
            for (auto e : vOperands) {
                llLocalTotal *= stoll( e );
            }
        } else {
            std::cout << "ERROR: main() [ part 1 ] --> unidentified operator encountered: " << cOperator << std::endl;
        }

        // accumulate the final answer
        llGrandTotal += llLocalTotal;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << llGrandTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    int nNrOperands = nOperatorIx;

    llGrandTotal = 0;
    bool bEndOfLine = false;

    while (inputData[0].length() > 0) {

        // first determine token length using the operator line
        int nTokenLength = -1;
        for (int i = 1; i < (int)inputData[nOperatorIx].length() && nTokenLength == -1; i++) {
            if (inputData[nOperatorIx][i] != ' ') {
                nTokenLength = i;
            }
        }
        if (nTokenLength == -1) {
            nTokenLength = inputData[nOperatorIx].length();
            bEndOfLine = true;
        }

        // then determine the operator ...
        char cOperator = inputData[nOperatorIx][0];

        // now we know that the operands are from 0 and have tokenlenght
        // get the numbers, combine them into a local answer, and cumulate a global answer from it
        long long llLocalAnswer;
        if (cOperator == '+') {

            int nNr;
            llLocalAnswer = 0;
            for (int i = nTokenLength - 1; i >= 0; i--) {
                std::string nrString;
                for (int j = 0; j < nNrOperands; j++) {
                    nrString.push_back( inputData[j][i] );
                }
                nNr = OnlySpaces( nrString ) ? 0 : stoi( nrString );
                llLocalAnswer += nNr;
            }
        } else if (cOperator == '*') {
            int nNr;
            llLocalAnswer = 1;
            for (int i = nTokenLength - 1; i >= 0; i--) {
                std::string nrString;
                for (int j = 0; j < nNrOperands; j++) {
                    nrString.push_back( inputData[j][i] );
                }
                nNr = OnlySpaces( nrString) ? 1 : stoi( nrString );
                llLocalAnswer *= nNr;
            }
        } else {
            std::cout << "ERROR: main() [ part 2 ] --> unidentified operator encountered: " << cOperator << std::endl;
        }

        // trim the input lines
        for (int i = 0; i < (int)inputData.size(); i++) {
            if (bEndOfLine) {
                inputData[i].clear();
            } else {
                std::string sRemove = get_token_sized( nTokenLength, inputData[i] );
            }
        }

        llGrandTotal += llLocalAnswer;


//        std::cout << "Operator = " << cOperator << std::endl;
//        std::cout << "local answer: " << llLocalAnswer << ", cumulated answer: " << llGrandTotal << std::endl;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llGrandTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
