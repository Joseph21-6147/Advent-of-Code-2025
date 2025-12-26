// AoC 2025 - Day 12 - Christmas Tree Farm
// =======================================

// date:  2025-12-12
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day12"          // <<===== adapt to day nr of AoC

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

// the data consists of small grids (3x3) of free ('.') or occupied ('#') fields that form a shape...

typedef std::vector<std::string> ShapeString;   // always 3x3 grid of either '.' or '#'
typedef struct {
    ShapeString vShape;
    int nSurface = -1;
} ShapeType;
typedef std::vector<ShapeType> ShapeStream;

// ... and a list of regions with required shape quantities
typedef struct {
    int w, l;   // width and length of region
    std::vector<int> vFitSpecs;   // contains quantities of shapes to fit in the region
                                  // there are always 6 numbers in vFitSpecs, since there are 6 shapes
} RegionType;
typedef std::vector<RegionType> RegionStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// count and return the number of occupied locations in shape s
int GetShapeSurface( ShapeString &s ) {
    int nResult = 0;
    for (int y = 0; y < (int)s.size(); y++) {
        for (int x = 0; x < (int)s[0].length(); x++) {
            switch ( s[y][x] ) {
                case '#': nResult += 1; break;
                case '.': /* ignore */  break;
                default : std::cout << "ERROR: GetShapeSurface() --> unexpected char: " << s[y][x] << std::endl;
            }
        }
    }
    return nResult;
}

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( ShapeStream &s, RegionStream &r ) {

    // hard code your examples here - use the same data for your test input file
    ShapeString sString;
    ShapeType sDatum;
    sString = { "###", "##.", "##." }; sDatum.vShape = sString; s.push_back( sDatum );
    sString = { "###", "##.", ".##" }; sDatum.vShape = sString; s.push_back( sDatum );
    sString = { ".##", "###", "##." }; sDatum.vShape = sString; s.push_back( sDatum );
    sString = { "##.", "###", "##." }; sDatum.vShape = sString; s.push_back( sDatum );
    sString = { "###", "#..", "###" }; sDatum.vShape = sString; s.push_back( sDatum );
    sString = { "###", ".#.", "###" }; sDatum.vShape = sString; s.push_back( sDatum );

    RegionType rDatum;
    rDatum = {  4, 4, { 0, 0, 0, 0, 2, 0 } }; r.push_back( rDatum );
    rDatum = { 12, 5, { 1, 0, 1, 0, 2, 2 } }; r.push_back( rDatum );
    rDatum = { 12, 5, { 1, 0, 1, 0, 3, 2 } }; r.push_back( rDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, ShapeStream &s, RegionStream &r ) {

    std::ifstream dataFileStream( sFileName );
    s.clear();
    r.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {

        bool bStageShapes = sLine[1] == ':';

        if (bStageShapes) {
            ShapeType newShape;
            // ignore the line with the index, just get the next one
            for (int i = 0; i < 3; i++) {
                getline( dataFileStream, sLine );
                newShape.vShape.push_back( sLine );
            }
            s.push_back( newShape );
            // remove empty line
            getline( dataFileStream, sLine );
        } else {
            std::string sWidth = get_token_dlmtd( "x", sLine );
            std::string sLength = get_token_dlmtd( ": ", sLine );
            RegionType newRegion = { stoi( sWidth ), stoi( sLength ) };
            while (!sLine.empty()) {
                std::string sNr = get_token_dlmtd( " ", sLine );
                int nNr = stoi( sNr );
                newRegion.vFitSpecs.push_back( nNr );
            }
            r.push_back( newRegion );
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   ShapeStream &s, RegionStream &r ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, s, r ); }
void GetData_PUZZLE( ShapeStream &s, RegionStream &r ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, s, r ); }

// ==========   CONSOLE OUTPUT FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions

// output to console for testing
void PrintShape( ShapeType &s, bool bEndline = true ) {
    // define your datum printing code here
    for (auto e : s.vShape) {
        std::cout << e << std::endl;
    }
    std::cout << "srfc = " << s.nSurface << std::endl;
    if (bEndline) {
        std::cout << std::endl;
    }
}

// output to console for testing
void PrintShapeStream( ShapeStream &ss ) {
    for (int i = 0; i < (int)ss.size(); i++) {
        std::cout << "Index = " << i << std::endl;
        PrintShape( ss[i] );
    }
    std::cout << std::endl;
}

// output to console for testing
void PrintRegion( RegionType &r, bool bEndline = true ) {
    // define your datum printing code here
    std::cout << r.w << " x " << r.l << " - ";
    for (auto e : r.vFitSpecs) {
        std::cout << e << " ";
    }
    if (bEndline) {
        std::cout << std::endl;
    }
}

// output to console for testing
void PrintRegionStream( RegionStream &rs ) {
    for (auto &e : rs) {
        PrintRegion( e );
    }
    std::cout << std::endl;
}

// ==========   PROGRAM STAGING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( ShapeStream &s, RegionStream &r, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( s, r ); break;
        case TEST:    GetData_TEST(    s, r ); break;
        case PUZZLE:  GetData_PUZZLE(  s, r ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }

    for (int i = 0; i < (int)s.size(); i++) {
        s[i].nSurface = GetShapeSurface( s[i].vShape );
    }

    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintShapeStream( s );
        PrintRegionStream( r );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// ----- PART 1

// define a struct to hold info on the region w.r.t. surface available vs surface requirements
typedef struct {
    int nRegionIndex;
    int nSurfaceAvailable;
    int nSurfaceNeeded;
    int nDifference;
} CasusType;
typedef std::vector<CasusType> CasusStream;

// Check on surface needed vs available for the region r (which has index nRegIx in the input data)
// Return the result as a CasusType variable
CasusType GetCasusInfo( RegionType &r, int nRegIx, ShapeStream &ss ) {

    CasusType result;
    result.nRegionIndex = nRegIx;
    result.nSurfaceAvailable = r.w * r.l;
    result.nSurfaceNeeded = 0;
    for (int i = 0; i < (int)r.vFitSpecs.size(); i++) {
        result.nSurfaceNeeded += (r.vFitSpecs[i] * ss[i].nSurface);
    }
    result.nDifference = result.nSurfaceAvailable - result.nSurfaceNeeded;

    return result;
}

// ----- PART 2

// There's no coding required for part 2: if you gather all previous stars, you
// get the last one for free...

// ==========   MAIN()

int main()
{
    glbProgPhase = PUZZLE;     // program phase to EXAMPLE, TEST or PUZZLE
    std::cout << "Phase: " << ProgPhase2string() << std::endl << std::endl;
    flcTimer tmr;

/* ========== */   tmr.StartTiming();   // ============================================vvvvv

    // get input data, depending on the glbProgPhase (example, test, puzzle)
    ShapeStream inputShapes;
    RegionStream inputRegions;
    GetInput( inputShapes, inputRegions, glbProgPhase != PUZZLE );
    ShapeStream part2Shapes = inputShapes;
    RegionStream part2Regions = inputRegions;

    std::cout << "Data stats - size of shape stream " << inputShapes.size()
              <<                " and region stream " << inputRegions.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here


    CasusStream vCasus;
    // for each region, get the difference between the surface needed and available
    for (int i = 0; i < (int)inputRegions.size(); i++) {
//        std::cout << std::endl << "Processing region: " << i << std::endl;
        CasusType curCase = GetCasusInfo( inputRegions[i], i, inputShapes );
        vCasus.push_back( curCase );
    }

//    // sort the list for easy (manual) analysis
//    std::sort(
//        vCasus.begin(),
//        vCasus.end(),
//        []( CasusType &a, CasusType &b ) {
//            return a.nDifference < b.nDifference;
//        }
//    );
//    for (int i = 0; i < (int)vCasus.size(); i++) {
//        CasusType &curCase = vCasus[i];
//        std::cout << "region index = " << curCase.nRegionIndex
//                  << ", surface available = " << curCase.nSurfaceAvailable
//                  << ", surface needed = " << curCase.nSurfaceNeeded
//                  << ", difference = " << curCase.nDifference
//                  << ", % = " << float( curCase.nDifference ) * 100.0f / float( curCase.nSurfaceAvailable )
//                  << std::endl;
//
//    }

/*
 * Manual analysis showed that
 *   - the cases that were impossible often lacked only a few spaces, but
 *   - cases that were not impossible always had a lot (at least 29-30%) of spaces left
 *
 * From that I guessed that the "theoretically impossible" criterion would suffice to
 * get today's puzzle done, and it did :)
 *
 * To be frank: some Discord suggestions led me to this way of thinking.
 */

    // get total size
    int nSize = vCasus.size();
    // get the number of regions with a negative difference (i.e. they are definitely impossible)
    int nLess0 = std::count_if(
        vCasus.begin(),
        vCasus.end(),
        []( CasusType &a ) {
            return a.nDifference < 0;
        }
    );

//    std::cout << "There are " << nSize << " regions, and for " << nLess0 << " the difference is negative" << std::endl;
//    std::cout << "so for " << nSize - nLess0 << " there is (at least theoretically) a solution!" << std::endl;

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << nSize - nLess0 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputShapes = part2Shapes; // get fresh untouched copy of input data
    inputRegions = part2Regions;

    // part 2 code here


    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
