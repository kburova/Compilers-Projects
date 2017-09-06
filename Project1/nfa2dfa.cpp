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
#include <map>
#include <string>
#include <sstream>

using namespace std;

struct FA {
    int initState;
    set <int> finalStates;
    int numOfStates;
    vector < map < char, set <int> > > transTable;
};

string SetToString(set<int> &s);
void StringToSet(set<int> &s, const string &setStr);
void ParseNFA(FA &nfa);
void ConstructEclosure( FA & nfa , vector < set <int> > & eClosure );
void closureHelper( vector < map <char, set <int> > > & tTable, int state, set <int> & eClosureSet);

int main(){

    FA nfa;
    vector < set <int> > eClosure;

    ParseNFA(nfa);
    eClosure.resize(nfa.numOfStates);
    ConstructEclosure(nfa, eClosure);
    
    for (set <int> s : eClosure){
        cout << SetToString(s) << endl;
    }
    return 0;
}

/** Construction of e-closure(s) set **/
void ConstructEclosure( FA & nfa , vector < set <int> > & eClosure ) {
    int i;
    map < char, set <int> >::iterator mit;

    /** For each NFA state we look if it has any 'E' transitions,
     *  and if yes, we create E-closure set for that state
     */
    for (i = 0; i < nfa.numOfStates; i++){
        closureHelper(nfa.transTable, i, eClosure[i]);
    }
}

/** Recursive helper function for e-closure(s) construction **/
void closureHelper( vector < map <char, set <int> > > & tTable, int state, set <int> & eClosureSet) {
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
void ParseNFA(FA &nfa) {
    string line, token;
    bool isSymbols;
    int i, j;
    char setOfStates[10000];
    istringstream row;
    vector < char > symbols;

    /** Start scanning:
     *  Parse inittial state, final states, and number of states first.
     *  After that, scan transition table line by line:
     *      If starts with "State", we save language symbols
     *      Else we parse set of states for that transition
     */
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
}