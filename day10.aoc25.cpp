// AoC 2025 - Day 10 - Factory
// ===========================

// date:  2025-12-10
// by:    Joseph21 (Joseph21-6147)


// IMPORTANT NOTE: this code is by no means exemplary for the solution of today's AoC puzzle.
// At first, I implemented some Gaussian elimination code but I got stuck in it.
// Then I implemented the bifurcation method from reddit that was suggested on Discord by the olc community. That solution is is pretty elegant,
// but my solution failed on one of the input lines.
// To solve it, I did the failed input line by hand to arrive at an off by one solution.
// Finally - I guessed a couple of times to get to the final answer...
//
// Glad with the solution, but frustrated along the path of getting there, I didn't bother to clean up my code below


#define DAY_STRING  "day10"          // <<===== adapt to day nr of AoC

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>

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
typedef std::vector<int> NumberRange;
typedef struct {
    std::string sIndLights = "";
    std::vector<NumberRange> vvWireSchemes;
    NumberRange vJoltageNeeds;
} DatumType;
typedef std::vector<DatumType> DataStream;

void PrintNumberRange( NumberRange &vnr ) {
    for (auto e : vnr) {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions


// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { ".##."  , { {3}, {1,3}, {2}, {2,3}, {0,2}, {0,1}        }, {3,5,4,7}         }; dData.push_back( cDatum );
    cDatum = { "...#." , { {0,2,3,4}, {2,3}, {0,4}, {0,1,2}, {1,2,3,4} }, {7,5,12,7,2}      }; dData.push_back( cDatum );
    cDatum = { ".###.#", { {0,1,2,3,4}, {0,3,4}, {0,1,2,4,5}, {1,2}    }, {10,11,11,5,10,5} }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    // stop reading if there are no more lines left
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        DatumType cDatum;
        // for each line:
        // 1. parse indicator lights scheme (and get rid of [] chars)
        std::string sIndicatorLights = get_token_dlmtd( " ", sLine );
        for (int i = 1; i < (int)sIndicatorLights.length() - 1; i++) {
            cDatum.sIndLights.push_back( sIndicatorLights[i] );
        }
        // 2. parse wiring schemes
        while (sLine[0] == '(') {
            std::string sRemove = get_token_sized( 1, sLine );  // remove leading brace
            std::string sToken = get_token_dlmtd( ")", sLine );
            if (sLine.length() > 0) {
                // strip the "in between" space to the next token in sLine
                sRemove = get_token_sized( 1, sLine );
            }
            NumberRange vWireScheme;
            while (!sToken.empty()) {
                std::string sNr = get_token_dlmtd( ",", sToken );
                vWireScheme.push_back( stoi( sNr ));
            }
            cDatum.vvWireSchemes.push_back( vWireScheme );
        }
        // 3. parse joltage requirements
        if (sLine[0] == '{') {
            std::string sRemove = get_token_sized( 1, sLine );  // remove leading curly brace
            std::string sToken = get_token_dlmtd( "}", sLine );
            while (!sToken.empty()) {
                std::string sNr = get_token_dlmtd( ",", sToken );
                cDatum.vJoltageNeeds.push_back( stoi( sNr ));
            }
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
    // output indicator lights scheme
    std::cout << "[ " << iData.sIndLights << " ] - ";
    // output wire schemes
    for (int i = 0; i < (int)iData.vvWireSchemes.size(); i++) {
        std::cout << "(";
        NumberRange &curScheme = iData.vvWireSchemes[i];
        for (int j = 0; j < (int)curScheme.size(); j++) {
            std::cout << curScheme[j];
            if (j < (int)curScheme.size() - 1) {
                std::cout << ", ";
            }
        }
        if (i < (int)iData.vvWireSchemes.size() - 1) {
            std::cout << "), ";
        } else {
            std::cout << ") - ";
        }
    }
    // output joltage needs
    std::cout << "{";
    for (int i = 0; i < (int)iData.vJoltageNeeds.size(); i++) {
        std::cout << iData.vJoltageNeeds[i];
        if (i < (int)iData.vJoltageNeeds.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "}";
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

// toggles the indicator light in sLights at nLightIndex
void ToggleIndicator( std::string &sLights, int nLightIndex ) {

    CheckIndex( "ToggleIndicator()", nLightIndex, 0, (int)sLights.length() );

    sLights[nLightIndex] = (sLights[nLightIndex] == '.') ? '#' : '.';
}

// Take the wiring scheme from iDatum with index nButtonIndex, and toggle all the indicator lights
// according to the wiring scheme
// the result string is returned
std::string ApplyButton( DatumType &iDatum, int nButtonIndex, std::string &sInput ) {
    CheckIndex( "ApplyButton() 1", nButtonIndex, 0, (int)iDatum.vvWireSchemes.size() );
    NumberRange &vWireScheme = iDatum.vvWireSchemes[nButtonIndex];
    std::string sResult = sInput;
    for (int i = 0; i < (int)vWireScheme.size(); i++) {
        ToggleIndicator( sResult, vWireScheme[i] );
    }
    return sResult;
}

// =====/ definition of a BFS tree node /===============

typedef struct sTreeNodeStruct {
    int nLevel = -1;                        // on what level of the tree is this node? root level == 0
    int nDatumIx;                           // to what datum belongs this tree(node)?
    int nButtonPushed;                      // which button was pushed to get to this state?
    std::string sIndLights = "";            // current state of indicator lights
    std::vector<sTreeNodeStruct *> vKids;   // children for this node
    sTreeNodeStruct *pParent;               // parent to this node
    bool bVisited = false;                  // for A* algorithm
} TreeNode;
typedef TreeNode *TreeNodePtr;

// free all the memory that is held by the structure pointed at by t
void DisposeTree( TreeNodePtr t ) {
    if (t != nullptr) {
        for (int i = 0; i < (int)t->vKids.size(); i++) {
            DisposeTree( t->vKids[i] );
        }
        t->vKids.clear();
        delete t;
    }
}

int nGlbIndentCntr = 0;
int nIndentSize = 4;

void Indent() {
    for (int i = 0; i < nGlbIndentCntr; i++) {
        for (int j = 0; j < nIndentSize; j++) {
            std::cout << " ";
        }
    }
}

void PrintTreeNode( TreeNodePtr t ) {
    if (t != nullptr) {
        std::cout << "level = "      << t->nLevel;
        std::cout << " input ix = "  << t->nDatumIx;
        std::cout << " button ix = " << t->nButtonPushed;
        std::cout << " lights = "    << t->sIndLights;
        std::cout << std::endl;
    } else {
        std::cout << "NULL tree node" << std::endl;
    }
}

void PrintTree( TreeNodePtr t ) {
    if (t != nullptr) {
        Indent();
        std::cout << "level = "      << t->nLevel;
        std::cout << " input ix = "  << t->nDatumIx;
        std::cout << " button ix = " << t->nButtonPushed;
        std::cout << " lights = "    << t->sIndLights;
        std::cout << std::endl;
        nGlbIndentCntr += 1;
        for (int i = 0; i < (int)t->vKids.size(); i++) {
            PrintTree( t->vKids[i] );
        }
        nGlbIndentCntr -= 1;
    }
}

// convenience function - returns a newly created tree node with the parameter info in it
TreeNodePtr CreateTreeNode( int nLvl, int nDatumIx, int nButtPushed, std::string &sIndLghts, bool bVisited = false ) {
    TreeNodePtr newNode    = new TreeNode;
    newNode->nLevel        = nLvl;
    newNode->nDatumIx      = nDatumIx;
    newNode->nButtonPushed = nButtPushed;
    newNode->sIndLights    = sIndLghts;
    newNode->bVisited      = bVisited;
    newNode->pParent       = nullptr;
    return newNode;
}

// =====/ definition of struct for memoization /===============

typedef struct {
    std::string sLights;
    int nLevel;
} MemoType;
typedef std::vector<MemoType> MemoStream;

// Get the index into ms of the memo containing slghts. Returns -1 if not found
int GetMemoIndex( MemoStream &ms, std::string &slghts ) {
    int nResult = -1;
    for (int i = 0; i < (int)ms.size() && nResult < 0; i++) {
        if (ms[i].sLights == slghts) {
            nResult = i;
        }
    }
    return nResult;
}

// attempt to insert memo (slghts, nlvl):
//   If a memo with slghts already exists:
//     and level is lower or equal to nlvl --> do nothing, the memo is already in
//     and level is higher to nlvl         --> alter the level to nlvl
//   If a memo does not yet exist          --> insert it
void PutMemo( MemoStream &ms, std::string &slghts, int nlvl ) {
    int nMemoIndex = GetMemoIndex( ms, slghts );
    if (nMemoIndex < 0) {
        ms.push_back( { slghts, nlvl } );
    } else {

        CheckIndex( "PutMemo()", nMemoIndex, 0, (int)ms.size());

        if (ms[nMemoIndex].nLevel > nlvl) {
            ms[nMemoIndex].nLevel = nlvl;
        }
    }
}

// memoization - if string slghts alread exists in ms with a level value less than nLvl,
// this combination is not relevant anymore (since a shorter path led to the same result),
// and false is returned.
// Otherwise, true is returned.
bool IsRelevant( MemoStream &ms, std::string &slghts, int nLvl ) {
    int nMemoIndex = GetMemoIndex( ms, slghts );
    CheckIndex( "IsRelevant()", nMemoIndex, -1, (int)ms.size());
    return nMemoIndex < 0 || (ms[nMemoIndex].nLevel >= nLvl);
}

// perform breadth first search (using a queue, modelled by a std::deque) to explore
// the paths for pushing buttons in iData[nDatumIx].
// Since the input datum index is fed into the tree nodes, we need it as a separate parameter
int BFS( DataStream &iData, int nDatumIx, int nLimit = 1 ) {

    CheckIndex( "BFS()", nDatumIx, 0, iData.size());

    int nResultSteps = 0;

    DatumType &iDatum = iData[nDatumIx];
    std::deque<TreeNodePtr> Q;
    MemoStream vMemos;

    // Initialization:
    // create the first node, mark it visited and add it to the queue - indicator lights are all initially off
    std::string sLights;
    for (int i = 0; i < (int)iDatum.sIndLights.length(); i++) {
        sLights.push_back( '.' );
    }
    TreeNodePtr root = CreateTreeNode( 0, nDatumIx, -1, sLights, true );
    PutMemo( vMemos, sLights, 0 );
    Q.push_back( root );

    int nSolutions = 0;

    while (!Q.empty() && nSolutions < nLimit) {

        // Exploration:
        // while the queue is not empty, get front node from the queue
        TreeNodePtr curNode = Q.front();
        Q.pop_front();

        if (curNode->sIndLights == iDatum.sIndLights) {
            // a solution is found!!
            nResultSteps += curNode->nLevel;
            nSolutions += 1;
        } else {
            // Neighbor visit:
            // for the current node, visit all unvisited neighbours, mark them as visited and add them to the queue
            for (int i = 0; i < (int)iDatum.vvWireSchemes.size(); i++) {

                std::string auxLights = ApplyButton( iDatum, i, curNode->sIndLights );
                // check if this auxLights config doesn't already exist
                if (IsRelevant( vMemos, auxLights, curNode->nLevel + 1 )) {
                    TreeNodePtr aux = CreateTreeNode(
                        curNode->nLevel + 1,
                        curNode->nDatumIx,
                        i,
                        auxLights,
                        true
                    );
                    curNode->vKids.push_back( aux );
                    aux->pParent = curNode;

                    PutMemo( vMemos, auxLights, curNode->nLevel + 1 );
                    Q.push_back( aux );
                }
            }
        }
    }
    DisposeTree( root );
    Q.clear();
    vMemos.clear();

    return nResultSteps;
}


// ----- PART 2

typedef std::vector<NumberRange> SolutionSetType;

int PowerTwo( int nArg ) {
    if (nArg == 0) {
        return 1;
    } else {
        return 2 * PowerTwo( nArg - 1 );
    }
}

bool AllIndicatorsOff( std::string &sLights ) {
    bool bIsOff = true;
    for (int i = 0; i < (int)sLights.length() && bIsOff; i++) {
        bIsOff = (sLights[i] == '.');
    }
    return bIsOff;
}

void PrintSolutionSet( SolutionSetType &vvSet ) {

    for (int nScheme = 0; nScheme < (int)vvSet.size(); nScheme++) {
        for (int nButton = 0; nButton < (int)vvSet[0].size(); nButton++) {
            std::cout << vvSet[nScheme][nButton] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


// every button from the input datum can be pushed 0 or 1 time, so create a table with all possibilities
// and check whether that button push configuration is a solution
SolutionSetType GetSolutionSet( DataStream &inputData, int nDatumIx, std::string &sLights ) {
    // prepare some var's for easy reference

    CheckIndex( "GetSolutionSet()", nDatumIx, 0, inputData.size());

    DatumType &curDatum = inputData[nDatumIx];
//    int nNrButtons = (int)curDatum.vvWireSchemes.size();
    int nNrButtons = (int)curDatum.vvWireSchemes.size();
    int nNrPossibilities = PowerTwo( nNrButtons );

    // first get all possible button combinations in vFullSet
    SolutionSetType vFullSet;
    for (uint16_t i = 0; i < nNrPossibilities; i++) {
        NumberRange curSolution;
        curSolution.resize( nNrButtons );
        for (uint16_t b = 0; b < nNrButtons; b++) {
            if ((i >> b) & 0x00000001) {
                curSolution[b] = 1;
            } else {
                curSolution[b] = 0;
            }
        }
        vFullSet.push_back( curSolution );
    }

//    PrintSolutionSet( vFullSet );

    // now filter out only those button combinations that lead to configuration with all indicator lights off
    SolutionSetType vResultSet;
    for (int i = 0; i < (int)vFullSet.size(); i++) {
        NumberRange &vAttemptSolution = vFullSet[i];
        std::string sAttemptLights = sLights;
        for (int b = 0; b < nNrButtons; b++) {
            if (vAttemptSolution[b] == 1) {
                sAttemptLights = ApplyButton( curDatum, b, sAttemptLights );
            }
        }
        if (AllIndicatorsOff( sAttemptLights )) {
            vResultSet.push_back( vAttemptSolution );
        }
    }

    PrintSolutionSet( vResultSet );

    return vResultSet;
}

std::string GetParityString( NumberRange &vJoltLevels ) {
    std::string sResult;
    for (int i = 0; i < (int)vJoltLevels.size(); i++) {
        sResult.append( vJoltLevels[i] % 2 == 1 ? "#" : "." );
    }
    return sResult;
}

bool AllZero( NumberRange &vJoltLevels ) {
    bool bZero = true;
    for (int i = 0; i < (int)vJoltLevels.size() && bZero; i++) {
        bZero = (vJoltLevels[i] == 0);
    }
    return bZero;
}

bool ContainsNegatives( NumberRange &vJoltLevels ) {
    bool bNegFound = false;
    for (int i = 0; i < (int)vJoltLevels.size() && !bNegFound; i++) {
        bNegFound = (vJoltLevels[i] < 0);
    }
    return bNegFound;
}

NumberRange ApplyButton( DatumType &curDatum, int nButtonIx, NumberRange &vJoltLevels ) {
    NumberRange vResult = vJoltLevels;

    CheckIndex( "ApplyButton() 2", nButtonIx, 0, (int)curDatum.vvWireSchemes.size());

    for (int i = 0; i < (int)curDatum.vvWireSchemes[nButtonIx].size(); i++) {
        int nIndex = curDatum.vvWireSchemes[nButtonIx][i];

        CheckIndex( "ApplyButton() 3", nIndex, 0, (int)vResult.size());

        vResult[nIndex] -= 1;
    }
    return vResult;
}

#define INVALID 100000000

int MinimumButtons( DataStream &inputData, int nDatumIx, NumberRange &vJoltageLevels ) {

    CheckIndex( "MinimumButtons()", nDatumIx, 0, (int)inputData.size());


std::cout << "MinimumButtons() called with input datum: " << std::endl;
PrintDatum( inputData[nDatumIx] );
std::cout << "and joltage levels: "; PrintNumberRange( vJoltageLevels );


    NumberRange vRemainingJoltage = vJoltageLevels;
    DatumType &curDatum = inputData[nDatumIx];

    // suppose all values in vRemainingJoltage are zero --> answer is 0
    // else suppose some values are below zero --> answer is INVALID
    // else
    //     suppose parity string is all out --> divide by 2 immediately
    //     else -->
    //         get solution set for parity string
    //         if solution set is empty --> invalid
    //         else
    //             get the solution set to create an all out indicator lights
    //             for each solution in the solution set
    //                 apply the buttons for this solution on the remaining joltage
    //                 divide remaining joltage by 2
    //                 get solution by recursive call on remaining joltage

    //             select the smallest of all the answers and return it

    int nAnswer;
    // if all joltage levels have become 0, the answer is trivial (0 buttons left to press)
    if (AllZero( vRemainingJoltage )) {
        nAnswer = 0;

        std::cout << "Allzero, returning 0" << std::endl;

    } else
    // if any of the joltage levels has become negative, the answer is no longer valid
    if (ContainsNegatives( vRemainingJoltage )) {
        nAnswer = INVALID;

        std::cout << "INVALID (contains negatives), returning " << INVALID << std::endl;

    } else {

        std::string sParityString = GetParityString( vRemainingJoltage );


        std::cout << "Parity string: _" << sParityString << "_";

        // if the parity string for the remaining joltage levels is all out, then all levels are even
        if (AllIndicatorsOff( sParityString )) {
            for (int i = 0; i < (int)vRemainingJoltage.size(); i++) {
                vRemainingJoltage[i] /= 2;
            }

            int nLocalAnswer = MinimumButtons( inputData, nDatumIx, vRemainingJoltage );
            if (nLocalAnswer == INVALID) {
                nAnswer = INVALID;
            } else {
                nAnswer = 2 * nLocalAnswer;
            }

            std::cout << "All out case - local answer = " << nAnswer << std::endl;

        } else {

            // some joltage levels are odd - get solution set for solving this parity string
            SolutionSetType curSolutionSet = GetSolutionSet( inputData, nDatumIx, sParityString );

            if (curSolutionSet.empty()) {
                nAnswer = INVALID;

                std::cout << "INVALID (empty solution set), returning " << INVALID << std::endl;

            } else {
                NumberRange vAnswers;
                for (int i = 0; i < (int)curSolutionSet.size(); i++) {
                    NumberRange &curSolution = curSolutionSet[i];
                    NumberRange curJoltage = vRemainingJoltage;

                    int nButtons = std::accumulate( curSolution.begin(), curSolution.end(), 0 );
                    for (int b = 0; b < (int)curSolution.size(); b++) {
                        if (curSolution[b] == 1) {
                            NumberRange oldJoltage = curJoltage;
                            curJoltage = ApplyButton( curDatum, b, curJoltage );

    std::cout << "Applying wire scheme: " << b << ", transforming joltage levels from: " << std::endl;
    PrintNumberRange( oldJoltage );
    std::cout << " to " << std::endl;
    PrintNumberRange( curJoltage );
    std::cout << std::endl;

                        }
                    }

                    for (int j = 0; j < (int)curJoltage.size(); j++) {
                        curJoltage[j] /= 2;
                    }

    std::cout << "divided joltage levels by 2, result: ";
    PrintNumberRange( curJoltage );
    std::cout << std::endl;

                    int nLocalAnswer = MinimumButtons( inputData, nDatumIx, curJoltage );
                    if (nLocalAnswer == INVALID) {
                        vAnswers.push_back( INVALID );
                    } else {
                        vAnswers.push_back( nButtons + 2 * nLocalAnswer );
                    }
                }

                NumberRange::iterator iter = std::min_element(
                    vAnswers.begin(),
                    vAnswers.end()
                );
                nAnswer = (*iter);

                std::cout << "Regular case - local answer = " << nAnswer << std::endl;

            }
        }
    }
    return nAnswer;
}


// ==========   MAIN()

int main()
{
    glbProgPhase = TEST;     // program phase to EXAMPLE, TEST or PUZZLE
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

    int nTotalSteps = 0;
//    for (int i = 0; i < (int)inputData.size(); i++) {
////        std::cout << "Processing datum: " << i << " ";
////        PrintDatum( inputData[i] );
//
//        nTotalSteps += BFS( inputData, i );
//    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << nTotalSteps << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    int nTotal = 0;

    for (int i = 0; i < (int)inputData.size(); i++) {

        DatumType &curDatum = inputData[i];
        std::cout << "Processing datum: " << i << " " << std::endl;
        PrintDatum( curDatum );

        NumberRange vRemainingJoltage = inputData[i].vJoltageNeeds;
        int nLocal = MinimumButtons( inputData, i, vRemainingJoltage );
        nTotal += nLocal;
        std::cout << "minimal # of buttons to configure required joltage levels: " << nLocal << " accumulates to: " << nTotal << std::endl << std::endl;
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << nTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
