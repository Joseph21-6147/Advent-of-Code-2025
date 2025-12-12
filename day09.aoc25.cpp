// AoC 2025 - Day 09 - Movie Theater
// =================================

// date:  2025-12-09
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day09"          // <<===== adapt to day nr of AoC

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

// the data consists of red tile locations (integer coordinates in 2d)
typedef vi2d DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS          <<===== adapt to match DatumType and DataStream definitions


// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = {  7,1 }; dData.push_back( cDatum );
    cDatum = { 11,1 }; dData.push_back( cDatum );
    cDatum = { 11,7 }; dData.push_back( cDatum );
    cDatum = {  9,7 }; dData.push_back( cDatum );
    cDatum = {  9,5 }; dData.push_back( cDatum );
    cDatum = {  2,5 }; dData.push_back( cDatum );
    cDatum = {  2,3 }; dData.push_back( cDatum );
    cDatum = {  7,3 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sX, sY;
        split_token_dlmtd( ",", sLine, sX, sY );
        DatumType cDatum = { stoi( sX ), stoi( sY ) };
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
    std::cout << iData.ToString();
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

// definition of an "area", having two corner points and a surface value
typedef struct {
    vi2d p1, p2;
    long long llSurface;
} AreaType;
typedef std::vector<AreaType> AreaStream;

void PrintArea( AreaType &as, bool bEndl = true ) {
    std::cout << "p1 = " << as.p1.ToString() << ", p2 = " << as.p2.ToString() << ", area = " << as.llSurface;
    if (bEndl) {
        std::cout << std::endl;
    }
}

void PrintAreaStream( AreaStream &as, bool bOnlyFirst50 = false ) {
    int nLimit = bOnlyFirst50 ? std::min( 50, (int)as.size() ) : (int)as.size();
    for (int i = 0; i < nLimit; i++) {
        AreaType a = as[i];
        PrintArea( a );
    }
    std::cout << std::endl;
}

// returns the surface of the area described by corner points a and b
long long GetAreaSurface( vi2d &a, vi2d &b ) {
    long long x1 = a.x, y1 = a.y;
    long long x2 = b.x, y2 = b.y;
    return (abs( x1 - x2 ) + (long long)1) * (abs( y1 - y2 ) + (long long)1);
}

// Creates and returns a list of all possible area's between any two (corner) points from ds
// The returned list is sorted in decreasing surface value
AreaStream GetAreaStream( DataStream &ds ) {
    // get the area for all possible combinations of corner points in a list
    AreaStream vResult;
    for (int i = 0; i < (int)ds.size() - 1; i++) {
        for (int j = i + 1; j < (int)ds.size(); j++) {
            AreaType aux = { ds[i], ds[j], GetAreaSurface( ds[i], ds[j] ) };
            vResult.push_back( aux );
        }
    }
    // sort the result list in decreasing area value
    std::sort(
        vResult.begin(),
        vResult.end(),
        []( AreaType &a, AreaType &b ) {
            return a.llSurface > b.llSurface;
        }
    );
    return vResult;
}

// ----- PART 2

// =====/ Directed graph stuff / ===============

// definition of a directed graph "edge", coming from a point and going to a point
typedef struct {
    vi2d from, to;
} EdgeType;
typedef std::vector<EdgeType> EdgeStream;

void PrintEdge( EdgeType &e, bool bEndl = true ) {
    std::cout << "from: " << e.from.ToString() << " to: " << e.to.ToString();
    if (bEndl) std::cout << std::endl;
}

void PrintEdgeStream( EdgeStream &es ) {
    for (auto edge : es) {
        PrintEdge( edge );
    }
    std::cout << std::endl;
}

// Definition of a graph "node", having a position in 2D space, an incoming and an outgoing edge
// The edges are identified as indices into the EdgeStream list of edges
typedef struct {
    vi2d pos;
    int incoming, outgoing;
} GraphNode;
typedef std::vector<GraphNode> GraphType;

void PrintNode( GraphNode &n, EdgeStream &es ) {
    std::cout << "graph node position: " << n.pos.ToString() << std::endl;
    std::cout << "  incoming edge = "; PrintEdge( es[n.incoming] );
    std::cout << "  outgoing edge = "; PrintEdge( es[n.outgoing] );
}

void PrintGraph( GraphType &g, EdgeStream &es ) {
    for (auto node : g) {
        PrintNode( node, es );
    }
    std::cout << std::endl;
}

// Returns a list of 2 edge indices of the two edges from es that are adjacent to
// the graph node at pos
std::vector<int> FindAdjacentEdgeIndices( EdgeStream &es, vi2d &pos ) {
    std::vector<int> vIndices;
    for (int i = 0; i < (int)es.size(); i++) {
        if (es[i].from == pos || es[i].to == pos) {
            vIndices.push_back( i );
        }
    }
    if (vIndices.size() != 2) {
        std::cout << "ERROR: FindAdjacentEdgeIndices() --> unexpected number of indices: " << vIndices.size() << std::endl;
    }
    return vIndices;
}

// =====/ orientation queries /===============

// returns true if edge e is horizontal (resp. vertical)
bool IsHorizontal( EdgeType &e ) { return e.from.y == e.to.y; }
bool IsVertical(   EdgeType &e ) { return e.from.x == e.to.x; }

// returns true if edge is directed up (down/left/right)
bool GoingNorth( EdgeType &e ) { return IsVertical(   e ) && e.from.y > e.to.y; }
bool GoingSouth( EdgeType &e ) { return IsVertical(   e ) && e.from.y < e.to.y; }
bool GoingWest(  EdgeType &e ) { return IsHorizontal( e ) && e.from.x > e.to.x; }
bool GoingEast(  EdgeType &e ) { return IsHorizontal( e ) && e.from.x < e.to.x; }

// overloaded versions
bool GoingNorth( vi2d &fm, vi2d &to ) { EdgeType aux = { fm, to }; return GoingNorth( aux ); }
bool GoingSouth( vi2d &fm, vi2d &to ) { EdgeType aux = { fm, to }; return GoingSouth( aux ); }
bool GoingWest(  vi2d &fm, vi2d &to ) { EdgeType aux = { fm, to }; return GoingWest(  aux ); }
bool GoingEast(  vi2d &fm, vi2d &to ) { EdgeType aux = { fm, to }; return GoingEast(  aux ); }

// returns intersection point for a and b
// returns (-1, -1) if no intersection
vi2d GetIntersectionPoint( EdgeType &a, EdgeType &b) {

    vi2d result = { -1, -1 };
    if (IsHorizontal( a )) {
        if (IsHorizontal( b )) {
            // two edges that are parallel can't intersect
        } else {
            bool bIntersect = (
                // b is vertical, you can use either from or to point x coord
                std::min( a.from.x, a.to.x ) <= b.from.x &&
                std::max( a.from.x, a.to.x ) >= b.from.x &&
                // a is horizontal, you can use either from or to point y coord
                std::min( b.from.y, b.to.y ) <= a.from.y &&
                std::max( b.from.y, b.to.y ) >= a.from.y
            );
            if (bIntersect) {
                result = { b.from.x, a.from.y };
            }
        }
    } else {   // a is vertical
        if (IsVertical( b )) {
            // two edges that are parallel can't intersect
        } else {
            bool bIntersect = (
                // a is vertical, you can use either from or to point x coord
                std::min( b.from.x, b.to.x ) <= a.from.x &&
                std::max( b.from.x, b.to.x ) >= a.from.x &&
                // b is horizontal, you can use either from or to point y coord
                std::min( a.from.y, a.to.y ) <= b.from.y &&
                std::max( a.from.y, a.to.y ) >= b.from.y
            );
            if (bIntersect) {
                result = { a.from.x, b.from.y };
            }
        }
    }
    return result;
}

// returns the graph node index into g where the node with position pos is found
// returns -1 if not found
int GetNodeIndex( GraphType &g, vi2d &pos ) {
    int nResult = -1;
    for (int i = 0; i < (int)g.size() && nResult < 0; i++) {
        if (g[i].pos == pos) {
            nResult = i;
        }
    }
    return nResult;
}

// use the "to" point from edge e, and check where the other edge adjacent to that to point is going
// returns true if it goes right (i.e. 90 degrees clockwise w.r.t. direction of e)
// NOTE: left and right are relative to the current edge direction (whereas North, South etc are absolute directions)
bool ExtendsRight( GraphType &g, EdgeStream &es, EdgeType &e ) {
    vi2d to_point = e.to;
    int nIndex = GetNodeIndex( g, to_point );
    EdgeType next = es[ g[nIndex].outgoing ];

    return (
        (GoingNorth( e ) && GoingEast(  next )) ||
        (GoingEast(  e ) && GoingSouth( next )) ||
        (GoingSouth( e ) && GoingWest(  next )) ||
        (GoingWest(  e ) && GoingNorth( next ))
    );
}

// returns true if it goes left (i.e. 90 degrees counter clockwise w.r.t. direction of e)
bool ExtendsLeft( GraphType &g, EdgeStream &es, EdgeType &e ) {
    vi2d to_point = e.to;
    int nIndex = GetNodeIndex( g, to_point );
    EdgeType next = es[ g[nIndex].outgoing ];

    return (
        (GoingNorth( next ) && GoingEast(  e )) ||
        (GoingEast(  next ) && GoingSouth( e )) ||
        (GoingSouth( next ) && GoingWest(  e )) ||
        (GoingWest(  next ) && GoingNorth( e ))
    );
}

// use the "from" point from e, and check where the other edge adjacent to that from point is going
// returns true if e goes right (i.e. 90 degrees clockwise w.r.t. direction of prev)
bool PreExtendsRight( GraphType &g, EdgeStream &es, EdgeType &e ) {
    vi2d from_point = e.from;
    int nIndex = GetNodeIndex( g, from_point );
    EdgeType prev = es[ g[nIndex].incoming ];

    return (
        (GoingNorth( prev ) && GoingEast(  e )) ||
        (GoingEast(  prev ) && GoingSouth( e )) ||
        (GoingSouth( prev ) && GoingWest(  e )) ||
        (GoingWest(  prev ) && GoingNorth( e ))
    );
}

// returns true if e goes left (i.e. 90 degrees counter clockwise w.r.t. direction of prev)
bool PreExtendsLeft( GraphType &g, EdgeStream &es, EdgeType &e ) {
    vi2d from_point = e.from;
    int nIndex = GetNodeIndex( g, from_point );
    EdgeType prev = es[ g[nIndex].incoming ];

    return (
        (GoingNorth( e ) && GoingEast(  prev )) ||
        (GoingEast(  e ) && GoingSouth( prev )) ||
        (GoingSouth( e ) && GoingWest(  prev )) ||
        (GoingWest(  e ) && GoingNorth( prev ))
    );
}

// an edge is valid iff
// there is no intersection at all, or
// there are only touching intersections in combination with the right direction of the second edge
bool EdgeIsValid( GraphType &g, EdgeType &e, EdgeStream &es ) {
    // make sure e is oriented either right going (for horizontal) or down going (for vertical)
    EdgeType cur = { { std::min( e.from.x, e.to.x ), std::min( e.from.y, e.to.y ) },
                     { std::max( e.from.x, e.to.x ), std::max( e.from.y, e.to.y ) } };

    bool bValid = true;
    for (int i = 0; i < (int)es.size() && bValid; i++) {

        vi2d intersectResult = GetIntersectionPoint( cur, es[i] );
        if (intersectResult == vi2d( -1, -1 )) {
            // no intersection - remains valid
        } else {
            bool bCurTo = intersectResult == cur.to;
            bool bCurFm = intersectResult == cur.from;
            bool bCmpTo = intersectResult == es[i].to;
            bool bCmpFm = intersectResult == es[i].from;

            // get all 16 possible configs for above flags
            if (!bCurFm && !bCurTo && !bCmpFm && !bCmpTo) { /* no touch intersection */
                bValid = false;
            }

            if (!bCurFm && !bCurTo && !bCmpFm &&  bCmpTo) { /* single intersection: case A */
                // case A
                bool bValid1 = (
                    GoingEast( cur ) && (
                        (GoingSouth( es[i] ) && ExtendsLeft( g, es, es[i] )) ||
                        (GoingNorth( es[i] ) && ExtendsLeft( g, es, es[i] ))
                    )
                );
                bool bValid2 = (
                    GoingSouth( cur ) && (
                        (GoingWest( es[i] ) && ExtendsLeft( g, es, es[i] )) ||
                        (GoingEast( es[i] ) && ExtendsLeft(   g, es, es[i] ))
                    )
                );
                bValid = bValid1 || bValid2;
            }
            if (!bCurFm && !bCurTo &&  bCmpFm && !bCmpTo) { /* single intersection: case B */
                // case B
                bool bValid1 = (
                    GoingEast( cur ) && (
                        (GoingSouth( es[i] ) && PreExtendsLeft( g, es, es[i] )) ||
                        (GoingNorth( es[i] ) && PreExtendsLeft( g, es, es[i] ))
                    )
                );
                bool bValid2 = (
                    GoingSouth( cur ) && (
                        (GoingWest( es[i] ) && PreExtendsLeft( g, es, es[i] )) ||
                        (GoingEast( es[i] ) && PreExtendsLeft( g, es, es[i] ))
                    )
                );
                bValid = bValid1 || bValid2;
            }
            if (!bCurFm &&  bCurTo && !bCmpFm && !bCmpTo) { /* single intersection: case C */
                // case C
                bValid = (
                    (GoingEast(  cur ) && GoingSouth( es[i] )) ||
                    (GoingSouth( cur ) && GoingWest(  es[i] ))
                );
            }
            if ( bCurFm && !bCurTo && !bCmpFm && !bCmpTo) { /* single intersection: case D */
                // case D
                bValid = (
                    (GoingEast(  cur ) && GoingNorth( es[i] )) ||
                    (GoingSouth( cur ) && GoingEast(  es[i] ))
                );
            }
            if (!bCurFm &&  bCurTo && !bCmpFm &&  bCmpTo) { /* double intersection: case E */
                // case E
                bool bValid1 = (
                    GoingEast( cur ) && (
                        GoingSouth( es[i] ) || (
                            GoingNorth( es[i] ) && ExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bool bValid2 = (
                    GoingSouth( cur ) && (
                        GoingWest( es[i] ) || (
                            GoingEast( es[i] ) && ExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bValid = bValid1 || bValid2;
            }
            if (!bCurFm &&  bCurTo &&  bCmpFm && !bCmpTo) { /* double intersection: case F */
                // case F
                bool bValid1 = (
                    GoingEast( cur ) && (
                        GoingSouth( es[i] ) || (
                            GoingNorth( es[i] ) && PreExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bool bValid2 = (
                    GoingSouth( cur ) && (
                        GoingWest( es[i] ) || (
                            GoingEast( es[i] ) && PreExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bValid = bValid1 || bValid2;
            }
            if ( bCurFm && !bCurTo && !bCmpFm &&  bCmpTo) { /* double intersection: case G */
                // case G
                bool bValid1 = (
                    GoingEast( cur ) && (
                        GoingNorth( es[i] ) || (
                            GoingSouth( es[i] ) && ExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bool bValid2 = (
                    GoingSouth( cur ) && (
                        GoingEast( es[i] ) || (
                            GoingWest( es[i] ) && ExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bValid = bValid1 || bValid2;
            }
            if ( bCurFm && !bCurTo &&  bCmpFm && !bCmpTo) { /* double intersection: case H */
                // case H
                bool bValid1 = (
                    GoingEast( cur ) && (
                        GoingNorth( es[i] ) || (
                            GoingSouth( es[i] ) && PreExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bool bValid2 = (
                    GoingSouth( cur ) && (
                        GoingEast( es[i] ) || (
                            GoingWest( es[i] ) && PreExtendsLeft( g, es, es[i] )
                        )
                    )
                );
                bValid = bValid1 || bValid2;
            }
            // all error cases below
//            if (!bCurFm && !bCurTo &&  bCmpFm &&  bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 1" << std::endl; }
//            if (!bCurFm &&  bCurTo &&  bCmpFm &&  bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 2" << std::endl; }
//            if ( bCurFm && !bCurTo &&  bCmpFm &&  bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 3" << std::endl; }
//            if ( bCurFm &&  bCurTo && !bCmpFm && !bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 4" << std::endl; }
//            if ( bCurFm &&  bCurTo && !bCmpFm &&  bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 5" << std::endl; }
//            if ( bCurFm &&  bCurTo &&  bCmpFm && !bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 6" << std::endl; }
//            if ( bCurFm &&  bCurTo &&  bCmpFm &&  bCmpTo) { /* error - cannot occur */ std::cout << "ERROR in logic table 7" << std::endl; }
        }
    }
    return bValid;
}

bool AreaIsValid( GraphType &g, AreaType &a, EdgeStream &es ) {
    // get area corner points
    int up = std::min( a.p1.y, a.p2.y );
    int dn = std::max( a.p1.y, a.p2.y );
    int lt = std::min( a.p1.x, a.p2.x );
    int rt = std::max( a.p1.x, a.p2.x );
    // create area edges from them
    EdgeType up_hor = { vi2d( lt, up ), vi2d( rt, up ) };
    EdgeType dn_hor = { vi2d( lt, dn ), vi2d( rt, dn ) };
    EdgeType lt_ver = { vi2d( lt, up ), vi2d( lt, dn ) };
    EdgeType rt_ver = { vi2d( rt, up ), vi2d( rt, dn ) };
    // check validity of edges - the area is valid iff all edges are
    return (
        EdgeIsValid( g, up_hor, es ) &&
        EdgeIsValid( g, dn_hor, es ) &&
        EdgeIsValid( g, lt_ver, es ) &&
        EdgeIsValid( g, rt_ver, es )
    );
}

// create a copy of vInput where only the "valid" areas are copied over
AreaStream FilterAreaStream( GraphType &g, AreaStream &vInput, EdgeStream &es ) {
    AreaStream vResult;
    for (int i = 0; i < (int)vInput.size(); i++) {
        if (AreaIsValid( g, vInput[i], es )) {
            vResult.push_back( vInput[i] );
        }
    }
    return vResult;
}

// used in graph creation
int FindHorizontalEdgeIndex( EdgeStream &es, vi2d &pos ) {
    std::vector<int> vAdjacentEdges = FindAdjacentEdgeIndices( es, pos );

    int nResult = -1;
    if (IsHorizontal( es[vAdjacentEdges[0]] )) {
        nResult = vAdjacentEdges[0];
    } else if (IsHorizontal( es[vAdjacentEdges[1]] )) {
        nResult = vAdjacentEdges[1];
    } else {
        std::cout << "ERROR: FindHorizontalEdgeIndex() --> could not find horizontal adjacent edge" << std::endl;
    }
    return nResult;
}

void CreateGraph( DataStream &iData, GraphType &g, EdgeStream &es ) {
    // first get all the locations from iData into g
    // search for the ultimate upper left point in the process
    int nCacheStartPoint;
    int nMinY = INT_MAX, nMinX = INT_MAX;
    for (int i = 0; i < (int)iData.size(); i++) {
        GraphNode aux;
        aux.pos = iData[i];
        g.push_back( aux );
        if ((aux.pos.y < nMinY) || (aux.pos.y == nMinY && aux.pos.x < nMinX)) {
            nMinY = aux.pos.y;
            nMinX = aux.pos.x;
            nCacheStartPoint = i;
        }
    }

    // determine the horizontal edges and put them in es
    std::sort(                                        // first sort on y coordinate
        g.begin(),
        g.end(),
        []( GraphNode &a, GraphNode &b ) {
            return (a.pos.y < b.pos.y) || (a.pos.y == b.pos.y && a.pos.x < b.pos.x);
        }
    );
    for (int i = 0; i < (int)g.size(); i += 2) {      // pair wise create the horizontal edges
        EdgeType aux = { g[i].pos, g[i + 1].pos };
        es.push_back( aux );
    }

    // determine the vertical edges and put them in es
    std::sort(                                        // first sort on x coordinate
        g.begin(),
        g.end(),
        []( GraphNode &a, GraphNode &b ) {
            return (a.pos.x < b.pos.x) || (a.pos.x == b.pos.x && a.pos.y < b.pos.y);
        }
    );
    for (int i = 0; i < (int)g.size(); i += 2) {      // pair wise create the vertical edges
        EdgeType aux = { g[i].pos, g[i + 1].pos };
        es.push_back( aux );
    }

    // note: nCacheStartPoint indexs into iData, not into g
    vi2d startPos = iData[nCacheStartPoint];
    // now we need to associate the edges with the incoming and outgoing values of the graph nodes,
    // and at the same time fix the order of the coordinate in the edges if necessary

    // get the horizontal edge outgoing of the start position
    int nOutgoingIx = FindHorizontalEdgeIndex( es, startPos );
    // swap the edge coordinates if needed to set the direction of the edge
    EdgeType &auxEdge = es[nOutgoingIx];
    if (auxEdge.to == startPos) {
        vi2d tmp = auxEdge.from;
        auxEdge.from = auxEdge.to;
        auxEdge.to = tmp;
    }

    // get index into the start node of g
    int nStartNodeIx = GetNodeIndex( g, startPos );
    GraphNode &startNode = g[nStartNodeIx];
    // set the horizontal edge as the outgoing edge for the start position graph node
    startNode.outgoing = nOutgoingIx;

    int nPrevNodeIx = nStartNodeIx;
    vi2d curCornerPos = es[startNode.outgoing].to;
    int nCurNodeIx = GetNodeIndex( g, curCornerPos );

    do {
        GraphNode &prevNode = g[nPrevNodeIx];
        // get next corner point/graph node as a reference
        GraphNode &curNode = g[nCurNodeIx];

        std::vector<int> vAdjacentEdgeIx = FindAdjacentEdgeIndices( es, curNode.pos );
        vi2d e0to = es[vAdjacentEdgeIx[0]].to;
        vi2d e0fm = es[vAdjacentEdgeIx[0]].from;
        vi2d e1to = es[vAdjacentEdgeIx[1]].to;
        vi2d e1fm = es[vAdjacentEdgeIx[1]].from;

        int nIndexUsed = -1;
        if (e0to == curNode.pos && e0fm == prevNode.pos) {
            // edge 0 is from the prev node to the current node
            // edge ordering is OK, just set the graph node to the correct edge
            curNode.incoming = vAdjacentEdgeIx[0];
            nIndexUsed = 0;
        } else
        if (e0fm == curNode.pos && e0to == prevNode.pos) {
            // edge 0 is from the current node to the prev node
            // swap edge ordering, and then set the graph node to edge 0
            vi2d tmp = es[vAdjacentEdgeIx[0]].to;
            es[vAdjacentEdgeIx[0]].to = es[vAdjacentEdgeIx[0]].from;
            es[vAdjacentEdgeIx[0]].from = tmp;

            curNode.incoming = vAdjacentEdgeIx[0];
            nIndexUsed = 0;
        } else
        if (e1to == curNode.pos && e1fm == prevNode.pos) {
            // edge 1 is from the prev node to the current node
            // edge ordering is OK, just set the graph node to the correct edge
            curNode.incoming = vAdjacentEdgeIx[1];
            nIndexUsed = 1;
        } else
        if (e1fm == curNode.pos && e1to == prevNode.pos) {
            // edge 1 is from the current node to the prev node
            // swap edge ordering, and then set the graph node to edge 1
            vi2d tmp = es[vAdjacentEdgeIx[1]].to;
            es[vAdjacentEdgeIx[1]].to = es[vAdjacentEdgeIx[1]].from;
            es[vAdjacentEdgeIx[1]].from = tmp;

            curNode.incoming = vAdjacentEdgeIx[1];
            nIndexUsed = 1;
        }

        // the incoming edge is set, now set the outgoing edge
        if (nIndexUsed == 0) {
            // use vAdjacentEdgeIx[1] as the outgoing edge
            if (e1to == curNode.pos) {
                // swap edge ordering and set graph node
                vi2d tmp = es[vAdjacentEdgeIx[1]].to;
                es[vAdjacentEdgeIx[1]].to = es[vAdjacentEdgeIx[1]].from;
                es[vAdjacentEdgeIx[1]].from = tmp;

                curNode.outgoing = vAdjacentEdgeIx[1];
            } else {
                // ordering is OK, just set graph node
                curNode.outgoing = vAdjacentEdgeIx[1];
            }
        } else
        if (nIndexUsed == 1) {
            // use vAdjacentIx[0] as the outgoing edge
            if (e0to == curNode.pos) {
                // swap edge ordering and set graph node
                vi2d tmp = es[vAdjacentEdgeIx[0]].to;
                es[vAdjacentEdgeIx[0]].to = es[vAdjacentEdgeIx[0]].from;
                es[vAdjacentEdgeIx[0]].from = tmp;

                curNode.outgoing = vAdjacentEdgeIx[0];
            } else {
                // ordering is OK, just set graph node
                curNode.outgoing = vAdjacentEdgeIx[0];
            }
        } else {
            std::cout << "ERROR: FindHorizontalEdgeIndex() --> could not find horizontal adjacent edge" << std::endl;
        }

        // prepare for next loop iteration
        int nNextNodeIx = GetNodeIndex( g, es[curNode.outgoing].to );
        nPrevNodeIx = nCurNodeIx;
        nCurNodeIx = nNextNodeIx;

    } while (nPrevNodeIx != nStartNodeIx);
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

    AreaStream vAreas = GetAreaStream( inputData );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: " << vAreas[0].llSurface << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    GraphType vNodes;
    EdgeStream vEdges;

    CreateGraph( inputData, vNodes, vEdges );

//    std::cout << "After CreateGraph() - edges: " << std::endl;
//    PrintEdgeStream( vEdges );
//    std::cout << "graph nodes: " << std::endl;
//    PrintGraph( vNodes, vEdges );

    vAreas = GetAreaStream( inputData );

//    std::cout << "filtering area list: " << std::endl;
    AreaStream vFilteredAreas = FilterAreaStream( vNodes, vAreas, vEdges );
//    std::cout << "result list: " << std::endl;
//    PrintAreaStream( vFilteredAreas, true );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: " << vFilteredAreas[0].llSurface << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2: " );   // ==============================^^^^^

    return 0;
}
