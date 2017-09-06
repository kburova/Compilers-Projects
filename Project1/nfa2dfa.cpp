/*****************************************
 * CS 461. Project 1.
 * Completed by : Ksenia Burova
 *
 * Implementation of NFA to DFA algorithm
 *****************************************/

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <unordered_map>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

string SetToString(set<int> &s);
void StringToSet(set<int> &s, const string &setStr);

struct FA {
    int initState;
    set <int> finalStates;
    int numOfStates;
    vector < map < char, set <int> > > transTable;
};

class nfa2dfa{
    private:
        FA nfa, dfa;
        vector < set <int> > Dstates;
        vector < char > symbols;
    public:
        void ParseNFA();
        set<int> ConstructEclosure( const set <int> &states, bool & isFinal );
        set<int> ConstructEclosure( int state, bool & isFinal );
        void closureHelper( vector < map <char, set <int> > > & tTable, int state, set <int> & eClosureSet, bool & isFinal);
        set<int> move(set<int> & T, char symbol);
        void constructSubset();
        void printFA(bool option);
};

int main(){

    nfa2dfa nd;
    nd.ParseNFA();
    nd.constructSubset();
    nd.printFA(1);
    return 0;
}

/** Pretty Print of Finite Automata **/
void nfa2dfa::printFA(bool option){
    /** DFA on true option **/
    if (option) {
        cout << "final DFA:"<< endl;

        printf("Initial State:  %d\nFinal States:   %s\nTotal States:   %d\n", dfa.initState, SetToString(dfa.finalStates).c_str(), dfa.numOfStates );
        cout<<"State   ";
        for (int i = 0; i < symbols.size()-1; i++) {
            printf("%-12c", symbols[i]);
        }
        printf("\n");
        for (int j =0; j <dfa.numOfStates; j++) {
            printf("%-7d ", j+1);
            for (int i = 0; i < symbols.size() - 1; i++) {
                printf("%-12s",SetToString(dfa.transTable[j][symbols[i]]).c_str());
            }
            printf("\n");
        }
    }
}
/** Constructing set of states for DFA **/
void nfa2dfa::constructSubset(){
    int i, j, size;
    string s;
    set <int> U;
    bool isFinal;
    unordered_map < string, int > trackingSets;

    i = 0;
    size = 1;
    //adding start state to Dstates
    isFinal = false;
    Dstates.push_back( ConstructEclosure(nfa.initState - 1, isFinal) );
    s = SetToString(Dstates[0]);
    trackingSets.insert( make_pair(s,1) );

    cout << "creating corresponding DFA ..." << endl;
    printf("new DFA state:  1    -->  %s\n", s.c_str());
    if (isFinal) dfa.finalStates.insert(size);
    dfa.initState = 1;

    while( i != Dstates.size() ){
        map < char, set <int> > ttable;
        for ( j = 0; j < symbols.size()-1; j++){

            isFinal = false;
            U = ConstructEclosure( move( Dstates[i],symbols[j]), isFinal );
            s = SetToString(U);

            if ( trackingSets.find(s) == trackingSets.end() && s.length() != 2 /*"{}"*/) {
                Dstates.push_back(U);
                
                printf("new DFA state:  %d    -->  %s\n", ++size, s.c_str());
                trackingSets.insert( make_pair(s, size) );
                if (isFinal) dfa.finalStates.insert(size);
            }
            if (s.size() != 2)
            printf("State %s goes to %d on %c\n", SetToString( Dstates[i] ).c_str(), trackingSets.find(s)->second, symbols[j] );
            if (s.size() != 2) ttable[symbols[j]].insert( trackingSets.find(s)->second );
        }
        dfa.transTable.push_back(ttable);
        i++;
    }
    dfa.numOfStates = Dstates.size();
    cout << "done."<< endl << endl;
}

/** Creating a set of NFA states to which there is a transition on
 *  a given symbol (e.g. 'a' or 'b') from some state s in T **/
set<int> nfa2dfa::move(set <int> & T, char symbol){
    set <int> fromT;
    map < char, set <int> >::iterator mit;
    for (int i : T) {
        mit = nfa.transTable[i-1].find(symbol);

        //insert all possible transitions for each state to a new set
        if ( mit != nfa.transTable[i-1].end() ) {
            fromT.insert(mit->second.begin(), mit->second.end());
        }
    }
    return fromT;
}

/** Construction of e-closure() for set of states **/
set<int> nfa2dfa::ConstructEclosure( const set <int> &states, bool & isFinal ) {

    set <int> eClosure;
    /** For each state we create an eClosure, and then do union of closures **/
    for (int i : states){
        set <int> subSet;
        closureHelper(nfa.transTable, i-1, subSet, isFinal);
        //TODO: refactor here
        eClosure.insert(subSet.begin(), subSet.end());
    }
    return eClosure;
}

/** Construction of e-closure() for one state **/
set<int> nfa2dfa::ConstructEclosure( int state, bool & isFinal ) {
    set <int> eClosure;
    closureHelper(nfa.transTable, state, eClosure, isFinal);

    return eClosure;
}

/** Recursive helper function for e-closure(s) construction **/
void nfa2dfa::closureHelper( vector < map <char, set <int> > > & tTable, int state, set <int> & eClosureSet, bool & isFinal) {
    map < char, set <int> >::iterator mit;

    // add states reachable through e transition
    eClosureSet.insert(state+1);
    if ( nfa.finalStates.find(state+1) != nfa.finalStates.end()) isFinal = true;

    mit = tTable[state].find('E');
    if ( mit == tTable[state].end() ) return;

    // see if we can have further 'E' transitions through found set
    for (int i : mit->second) {
        closureHelper(tTable, --i, eClosureSet, isFinal);
    }
}

/** parseSet takes a string in a form of "{x1,x2,...xn}" and parses it into a set **/
void StringToSet(set <int> &s, const string &setStr) {
    string stateNum;
    int i;
    for ( i = 1; i < setStr.length(); i++) {
        if ( ((setStr[i] == ',') || ( setStr[i] == '}') )&& (stateNum.length() != 0) ) {
            s.insert(stoi(stateNum));
            stateNum.clear();
        }
        else stateNum.push_back(setStr[i]);
    }
}

/** Construct a string of "{x1,x2,...xn}" from form set **/
string SetToString(set<int> &s) {
    ostringstream os;

    os << "{";
    for (int i : s) {
        os << i ;
        // don't print comma after last element
        if (i != *s.rbegin()) os << ",";
    }
    os << "}";

    return os.str();
}

/** Parsing NFA from stdin **/
void nfa2dfa::ParseNFA() {
    string line, token;
    bool isSymbols;
    int i, j;
    char setOfStates[10000];
    istringstream row;

    /** Start scanning:
     *  Parse inittial state, final states, and number of states first.
     *  After that, scan transition table line by line:
     *      If starts with "State", we save language symbols
     *      Else we parse set of states for that transition
     */

    cout << "reading nfa...";
    while(getline(cin,line)){
        if (line.length() == 0) break;
        if ( sscanf(line.c_str(), "Initial State: %d", &nfa.initState) == 1 ) {
            // just scanning here
        }else if ( sscanf(line.c_str(), "Final States: %s", setOfStates) == 1 ) {
            StringToSet(nfa.finalStates, string(setOfStates));
        } else if ( sscanf(line.c_str(), "Total States: %d", &nfa.numOfStates) == 1 ) {
            nfa.transTable.resize(nfa.numOfStates);
        } else{
            row.clear();
            row.str(line);
            row >> token;

            if (token[0] == 'S') {
                isSymbols = true;
            }else{
                i = stoi(token) - 1;
                isSymbols = false;
            }

            j = 0;
            while (row >> token) {
                if (isSymbols) {
                    symbols.push_back(token[0]);
                } else {
                    if (token.size() > 2) {
                        StringToSet(nfa.transTable[i][symbols[j]], token);
                    }
                    j++;
                }
            }
        }
    }
    cout << "done." << endl;
}