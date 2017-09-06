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
#include <unordered_set>
#include <map>
#include <string>
#include <sstream>

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
        set<int> ConstructEclosure( const set <int> &states );
        set<int> ConstructEclosure( int state );
        void closureHelper( vector < map <char, set <int> > > & tTable, int state, set <int> & eClosureSet);
        set<int> move(set<int> & T, char symbol);
        void constructSubset();
};

int main(){

    nfa2dfa nd;
    nd.ParseNFA();
    nd.constructSubset();

    return 0;
}
/** Constructing set of states for DFA **/
void nfa2dfa::constructSubset(){
    int i, j;
    set <int> U;
    unordered_set < string > trackingSets;

    i = 0;
    //adding start state to Dstates
    Dstates.push_back(ConstructEclosure(nfa.initState - 1));
    trackingSets.insert( SetToString(Dstates[0]));

    set <int> b;
    while( i != Dstates.size() ){
        for ( j = 0; j < symbols.size()-1; j++){
            U = ConstructEclosure( move( Dstates[i],symbols[j]) );
            string s = SetToString(U);

            if ( trackingSets.find(s) == trackingSets.end() ) {
                Dstates.push_back(U);
                trackingSets.insert(s);
            }
            cout << SetToString(Dstates[i]) << endl;
            cout << symbols[j] << " : "<< SetToString(U) << endl <<endl;
        }
        i++;
    }
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
set<int> nfa2dfa::ConstructEclosure( const set <int> &states ) {

    set <int> eClosure;
    /** For each state we create an eClosure, and then do union of closures **/
    for (int i : states){
        set <int> subSet;
        closureHelper(nfa.transTable, i-1, subSet);
        //TODO: refactor here
        eClosure.insert(subSet.begin(), subSet.end());
    }
    return eClosure;
}

/** Construction of e-closure() for one state **/
set<int> nfa2dfa::ConstructEclosure( int state ) {
    set <int> eClosure;
    closureHelper(nfa.transTable, state, eClosure);

    return eClosure;
}

/** Recursive helper function for e-closure(s) construction **/
void nfa2dfa::closureHelper( vector < map <char, set <int> > > & tTable, int state, set <int> & eClosureSet) {
    map < char, set <int> >::iterator mit;

    // add states reachable through e transition
    eClosureSet.insert(state+1);

    mit = tTable[state].find('E');
    if ( mit == tTable[state].end() ) return;

    // see if we can have further 'E' transitions through found set
    for (int i : mit->second) {
        closureHelper(tTable, --i, eClosureSet);
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