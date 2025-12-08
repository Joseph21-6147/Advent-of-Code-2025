// AoC 2025 - Day 07 - Laboratories
// ================================

// date:  2025-12-07
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day07"          // <<===== adapt to day nr of AoC

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

// The data consists of a map containing
//   exactly one start point (S)
//   splitters (^)
//   empty positions (.)
// The map is modeled as a std::vector of std::string
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// for convenience
int MapWidth(  DataStream &dMap ) { return dMap[0].length(); }
int MapHeight( DataStream &dMap ) { return dMap.size(); }

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions


// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = ".......S......."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = ".......^......."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = "......^.^......"; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = ".....^.^.^....."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = "....^.^...^...."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = "...^.^...^.^..."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = "..^...^.....^.."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );
    cDatum = ".^.^.^.^.^...^."; dData.push_back( cDatum );
    cDatum = "..............."; dData.push_back( cDatum );

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

// get a list (std::vector) of all splitters '^' in the map
std::vector<vi2d> GetSplitterList( DataStream &dMap ) {
    std::vector<vi2d> vResult;
    for (int y = 0; y < MapHeight( dMap ); y++) {
        for (int x = 0; x < MapWidth( dMap ); x++) {
            if (dMap[y][x] == '^') {
                vResult.push_back( { x, y } );
            }
        }
    }
    return vResult;
}

// Performs one step in the propagation of beams in dMap. If any changes to the map are made,
// the function returns true. False otherwise.
bool PropagateBeamStep( DataStream &dMap ) {
    bool bChanged = false;

    for (int y = 0; y < MapHeight( dMap ); y++) {
        for (int x = 0; x < MapWidth( dMap ); x++) {
            // start point encountered - propagate beam one position down in the map
            // (if it wasn't already propagated)
            if (dMap[y][x] == 'S') {
                if (y < MapHeight( dMap ) - 1 && dMap[y + 1][x] == '.') {
                    dMap[y + 1][x] = '|';
                    bChanged = true;
                }
            } else
            // beam segment encountered - check on the position below:
            //   if it's an empty field, just propagate another beam segment
            //   if it's a splitter, propagate two beams adjacent to the splitter
            if (dMap[y][x] == '|') {
                if (y < MapHeight( dMap ) - 1) {
                    if (dMap[y + 1][x] == '.') {
                        dMap[y + 1][x] = '|';
                        bChanged = true;
                    }
                    if (dMap[y + 1][x] == '^') {
                        // left to the splitter
                        if (x > 0 && dMap[y + 1][x - 1] == '.') {
                            dMap[y + 1][x - 1] = '|';
                            bChanged = true;
                        }
                        // right to the splitter
                        if (x < MapWidth( dMap ) - 1 && dMap[y + 1][x + 1] == '.') {
                            dMap[y + 1][x + 1] = '|';
                            bChanged = true;
                        }
                    }
                }
            }
            // ignore empty spaces and splitters, they are no trigger values
            // when it comes to propagating the beam segments
        }
    }
    return bChanged;
}

// the number of splits is not the same as the number of splitters, since some splitters are idling and
// do not actually split a beam. So count only the splitters that actually get beamed onto
int CountNrSplits( DataStream &dMap, std::vector<vi2d> &vSplitters ) {
    int nResult = 0;
    for (auto e : vSplitters) {
        if (e.y > 0 && dMap[e.y - 1][e.x] == '|') {
            nResult += 1;
        }
    }
    return nResult;
}

// ----- PART 2

// I need some additional data structures for part 2 - a list of beam segments, the latter containing
// the time lines value at that point in the beam graph
typedef struct {
    vi2d pos;
    long long llTimeLines;
} BeamSegmentType;
typedef std::vector<BeamSegmentType> BeamType;

// for testing and debugging
void PrintBeamSegment( BeamSegmentType &bs ) {
    std::cout << "bs pos: " << bs.pos.ToString() << ", timelines: " << bs.llTimeLines << std::endl;
}

// for testing and debugging
void PrintBeam( BeamType &vBeam ) {
    for (int i = 0; i < (int)vBeam.size(); i++) {
        PrintBeamSegment( vBeam[i] );
    }
    std::cout << std::endl;
}

// search index in vBeams std::vector where position (x, y) occurs (if anywhere)
// returns -1 if not found
int Index( BeamType &vBeams, int x, int y ) {
    int nResult = -1;
    for (int i = 0; i < (int)vBeams.size() && nResult == -1; i++) {
        if (vBeams[i].pos.x == x && vBeams[i].pos.y == y) {
            nResult = i;
        }
    }
    return nResult;
}

// returns true if a beam segment at (x, y) exists in vBeams
bool Exists( BeamType &vBeams, int x, int y ) {
    return (Index( vBeams, x, y ) >= 0);
}

// ========== case functions needed in GetBeamList() ==========

// The beam coming from the start has just one timeline. It acts as the source of the beam graph.
void HandleStart( DataStream &dMap, BeamType &vBeams, int x, int y ) {
    if (!Exists( vBeams, x, y )) {
        BeamSegmentType aux;
        // store two segments, one at the 'S' position, and one right below it
        aux.pos.x = x; aux.pos.y = y    ; aux.llTimeLines = 1; vBeams.push_back( aux );
        aux.pos.x = x; aux.pos.y = y + 1; aux.llTimeLines = 1; vBeams.push_back( aux );
    }
}

// If the position exists as a beam segment, it is skipped, otherwise it is processed, meaning that
//    * its time line value is derived from it's predecessors in the graph, and
//    * a new beam segment is added to the beam segment list
// A segment can have 1, 2 or 3 predecessors in the graph, and its time line value is the sum of the time lines of the predecessor(s)
void HandleBar( DataStream &dMap, BeamType &vBeams, int x, int y ) {
    if (!Exists( vBeams, x, y )) {
        bool bSplitterLeft = x >                    0 && dMap[y    ][x - 1] == '^' && y > 0 && dMap[y - 1][x - 1] == '|';
        bool bSplitterRght = x < MapWidth( dMap ) - 1 && dMap[y    ][x + 1] == '^' && y > 0 && dMap[y - 1][x + 1] == '|';
        bool bBarAbove     = y >                    0 && dMap[y - 1][x    ] == '|';

        int nLeftIndex = -1; long long llLeftTimeLines = 0;
        int nRghtIndex = -1; long long llRghtTimeLines = 0;
        int nAbveIndex = -1; long long llAbveTimeLines = 0;

        if (bSplitterLeft) {
            nLeftIndex = Index( vBeams, x - 1, y - 1 );
            if (nLeftIndex == -1) { std::cout << "ERROR: HandleBar() --> splitter left case: could not find (" << x - 1 << ", " << y - 1 << ") index" << std::endl; }
            llLeftTimeLines = vBeams[nLeftIndex].llTimeLines;
        }
        if (bSplitterRght) {
            nRghtIndex = Index( vBeams, x + 1, y - 1 );
            if (nRghtIndex == -1) { std::cout << "ERROR: HandleBar() --> splitter right case: could not find (" << x + 1 << ", " << y - 1 << ") index" << std::endl; }
            llRghtTimeLines = vBeams[nRghtIndex].llTimeLines;
        }
        if (bBarAbove) {
            nAbveIndex = Index( vBeams, x, y - 1 );
            if (nAbveIndex == -1) { std::cout << "ERROR: HandleBar() --> bar above case: could not find (" << x << ", " << y - 1 << ") index" << std::endl; }
            llAbveTimeLines = vBeams[nAbveIndex].llTimeLines;
        }

        long long llCumulatedTimelines = 0;

        if (bSplitterLeft) { llCumulatedTimelines += llLeftTimeLines; }
        if (bSplitterRght) { llCumulatedTimelines += llRghtTimeLines; }
        if (bBarAbove    ) { llCumulatedTimelines += llAbveTimeLines; }

        BeamSegmentType aux = { { x, y }, llCumulatedTimelines };
        vBeams.push_back( aux );
    }
}

// converts the graph into a list of beam segments, and using HandleStart() and HandleBar() for it
BeamType GetBeamList( DataStream &dMap ) {
    BeamType vResult;

    for (int y = 0; y < MapHeight( dMap ); y++) {
        for (int x = 0; x < MapWidth( dMap ); x++) {
            switch (dMap[y][x]) {
                case 'S': HandleStart( dMap, vResult, x, y ); break;
                case '|': HandleBar(   dMap, vResult, x, y ); break;
                case '^': /* no action */                     break;
                case '.': /* no action */                     break;
                default : std::cout << "ERROR: GetBeamList() --> unknown map value: " << dMap[y][x] << std::endl;
            }
        }
    }
    return vResult;
}

// a beam segment is a leaf if it's located on the lower row of the map
bool IsLeaf( DataStream &dMap, int x, int y ) {
    return y == MapHeight( dMap ) - 1;
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

    std::vector<vi2d> vSplitterList = GetSplitterList( inputData );

    do {
        // just propagate
    } while (PropagateBeamStep( inputData ));

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << CountNrSplits( inputData, vSplitterList ) << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

//    inputData = part2Data; // get fresh untouched copy of input data
// NO: for this day in particular I need to continue with the result of part 1!

    // part 2 code here

    BeamType vBeamList = GetBeamList( inputData );

    long long llAnswer = 0;
    for (int i = 0; i < (int)vBeamList.size(); i++) {
        if (IsLeaf( inputData, vBeamList[i].pos.x, vBeamList[i].pos.y )) {
            llAnswer += vBeamList[i].llTimeLines;
        }
    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << llAnswer << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
