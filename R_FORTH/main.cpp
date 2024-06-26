#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include "int_stack.hpp"

using namespace std;

Stack theStack(10);

bool isValidVarName(char* name){
    if (name == nullptr){
        cerr << "null pointer :(" << endl;
        return false;
    }
    if(findTokenType(name) == WORD){

        vector<string> reservedWords = 
            {"bye", "push", "pop", "top", "dup", "swap", "over", "rot", "drop",
            "2swap", "2over", "2drop", "2dup", "/mod", "mod", "clear-stack"};

        for (int i = 0; i < reservedWords.size(); i++){
            //iterate through reservedWords, convert current to char* for strcmp()
            int wordLength = reservedWords[i].length();
            char current[wordLength + 1];
            strcpy(current, reservedWords[i].c_str());
            if(strcmp(name, current) == 0){ //if name matches a reserved word
                return false;
            }
        }

        int temp;
        string nameString = name;
        if (theStack.getVariable(nameString, temp)){ //if name matches previous variable
            return false;
        }
        return true;
    }
    return false;
}

void processOneTok(char* tok, string wholeString){
    enum TokenTypeT type = findTokenType(tok);

    //Numbers
    if (type == NUMBER){
        theStack.intStackPush(stoi(tok));
    }

    //Variable assignment
    else if (type == SYMBOL && strcmp(tok, "!") == 0) {
        int value;
        theStack.intStackPop(&value); // Get the value from the stack
        char delim[] = " \t\n\r\f\v";

        //extract potential name
        int spacePos = wholeString.find(' ');
        int exPos = wholeString.find('!', spacePos);
        int subLen = exPos - spacePos - 1;
        string str = wholeString.substr(spacePos + 1, subLen);
        char *tok = new char[subLen + 1];
        str.copy(tok, subLen - 1);

        // Assign value to the variable
        int temp;
        if (theStack.getVariable(tok, temp)) {
            theStack.createVariable(tok, value); 
            cout << "Value " << value << " assigned to " << tok << endl;
        } else {
            cerr << "Invalid variable name: \"" << tok << "\"" << endl;
        }
    }

    //Operators
    else if (type == OPERATOR){
        *tok = (char)(*tok);
        switch(*tok){
            case '+':
                theStack.intStackAdd();
                break;
            case '-':
                theStack.intStackSubtract();
                break;
            case '*':
                theStack.intStackMultiply();
                break;
            case '/':
                theStack.intStackDivide();
                break;
            case '<':
                theStack.intStackLessThan();
                break;
            case '>':
                theStack.intStackGreaterThan();
                break;
            case ';':
                //functions unimplemented
                break;
            case ':':
                //functions unimplemented
                break;
            default: 
                cout << "Unrecognized operator\n";
        }
    }
    //Words
    else if (type == WORD){ //sincerist apologies for the else if monster, couldn't easily get switch to work here
        int tempvar;
        if (strcmp(tok, "clear-stack") == 0){
            theStack = Stack(10);
        }
        if (strcmp(tok, "/mod") == 0){
            theStack.intStackModAndQuotient();
        }
        else if (strcmp(tok, "mod") == 0){
            theStack.intStackModOnly();
        }
        else if (strcmp(tok, "dup") == 0){
            theStack.intStackDup();
        }
        else if (strcmp(tok, "swap") == 0){
            theStack.intStackSwap();
        }
        else if (strcmp(tok, "over") == 0){
            theStack.intStackOver();
        }
        else if (strcmp(tok, "rot") == 0){
            theStack.intStackRot();
        }
        else if (strcmp(tok, "drop") == 0){
            theStack.intStackDrop();
        }
        else if (strcmp(tok, "2swap") == 0){
            theStack.intStack2Swap();
        }
        else if (strcmp(tok, "2drop") == 0){
            theStack.intStack2Drop();
        }
        else if (strcmp(tok, "2dup") == 0){
            theStack.intStack2Dup();
        }
        else if (strcmp(tok, "2over") == 0){
            theStack.intStack2Over();
        }
        else if (theStack.getVariable(tok, tempvar) && tempvar != -1){
            theStack.intStackPush(tempvar);
        }
    }
    //Unrecognized
    else {
        cerr << "Error: unrecognized symbol" << endl;
    }
} 

int main(int argc, char * * argv){
    string userString;
    int size = 0;
    int charsRead;
    bool keepAcceptingInput = true;

    char delim[] = " \t\n\r\f\v";

    cout << "Welcome to RForth" << endl;

    //core terminal loop
    while(keepAcceptingInput){
        getline(cin, userString);
        charsRead = userString.length(); 

        //array copy of userString created to use strtok()
        char userArray[charsRead + 1];
        strcpy(userArray, userString.c_str());
        char* tok = strtok(userArray, delim);

        if (strcmp(tok, "bye") == 0){
            keepAcceptingInput = false;
            continue;
        }

        //while there is a next token to handle
        while(tok != NULL){
            //handle variable creation
            if (strcmp(tok, "variable") == 0){
                tok = strtok(NULL, delim);
                if(isValidVarName(tok)){ 
                    if (theStack.createMapEntry(tok)){
                        cout << "variable " << tok << " created" << endl; 
                    }
                }
                else{
                    cerr << "Error: invalid variable name" << endl;
                }
            }

            //normal token processing
            else {
                processOneTok(tok, userString);
                tok = strtok(NULL, delim); 
            }
        }
        theStack.intStackPrint();
    }

    cout << "See you!" << endl;
    return 0;
}
