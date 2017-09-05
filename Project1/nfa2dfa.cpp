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
#include <string>
#include <sstream>

using namespace std;

struct NFA {
    int initState;
    set <int> finalStates;
    int numOfStates;
    vector < char > symbols;
    vector < vector < set <int> > > transTable;
};

void printSet(set <int> & s) {
    cout << "{";
    for (int i : s)
        cout << i << ",";
    cout << "}";
}
void parseSet( set <int> & s, const string & setStr ) {
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

void parseNFA( NFA & nfa) {
    string line, token;
    char setOfStates[10000];
    istringstream row;
    bool isSymbols;
    int i, j;

    isSymbols = false;

    while(getline(cin,line)){
        if ( sscanf(line.c_str(), "Initial State: %d", &nfa.initState) == 1 ) {
//            cout << nfa.initState << endl;
        }else if ( sscanf(line.c_str(), "Final States: %s", setOfStates) == 1 ) {
            parseSet(nfa.finalStates, string(setOfStates) );
//            printSet(nfa.finalStates);

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

                if (isSymbols) nfa.symbols.push_back(token[0]);
                else {
                    parseSet(nfa.transTable[i][j], token);
                    j++;
                }
            }

            if (isSymbols) {
                for (j = 0; j < nfa.transTable.size(); j++){
                    nfa.transTable[j].resize(nfa.symbols.size());
                }
            }
        }
    }

}

void printNFA(NFA & n){

    cout << "Init state: " << n.initState << endl;
    cout << "Fin states: " ;
    printSet(n.finalStates);
    cout << endl << "Tot states: " << n.numOfStates <<endl;

    cout << "State " ;
    for (int i = 0; i < n.symbols.size(); i++) cout << n.symbols[i]<< " ";
    cout <<endl;

    for (int i = 0; i < n.transTable.size(); i++){
        cout << i+1 << " ";
        for (int j = 0; j < n.transTable[0].size(); j++){
            printSet(n.transTable[i][j]);
            cout << " ";
        }
        cout << endl;
    }
}

int main(){

    NFA nfa;

    parseNFA(nfa);
    printNFA(nfa);
    return 0;
}