
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

LexicalAnalyzer lexer;
Token t1, t2, t3, t4;
string s1;
struct item {string identifier, scope, access_modifier;} tmpItem;
struct node {string scope, parent;} tmpNode;

vector<item> ID_list;
vector<item> leftside;
vector<item> rightside;
vector<node> scope_link;
vector<string> scope_stack;
vector<string> ID_buffer;

int parse_stmt_list();
int parse_stmt();
int parse_private_vars();
int parse_public_vars();
int parse_var_list();
int parse_global_vars();
int parse_scope();
int parse_program();

bool IsBufferEmpty();
void PushBuffer(string);
string PopBuffer();

string checkPrefix(item);
void printAssignments();

bool IsBufferEmpty()
{
    return ID_buffer.empty();
}

void PushBuffer(string s)
{
    ID_buffer.push_back(s);
}

string PopBuffer()
{
    string s;
    s = ID_buffer.back();
    ID_buffer.pop_back();
    return s;
}

int parse_stmt_list()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_stmt();
        t2 = lexer.GetToken();
        if (t2.token_type == ID) {
            lexer.UngetToken(t2);
            parse_stmt_list();
            //cout << "stmt_list -> stmt stmt_list\n";
            return 0;
        }
        else if (t2.token_type == RBRACE) {
            lexer.UngetToken(t2);
            //cout << "stmt_list -> stmt\n";
            return 0;
        }
        else {
            cout << "Syntax Error" << endl;
            exit(1);
        }
    }
    else {
        cout << "Syntax Error" << endl; 
        exit(1);
    }
}

int parse_stmt()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t2 = lexer.GetToken();
        if (t2.token_type == EQUAL) {
            t3 = lexer.GetToken();
            if (t3.token_type == ID) {
                t4 = lexer.GetToken();
                if (t4.token_type == SEMICOLON) {

                    tmpItem.identifier = t1.lexeme;
                    tmpItem.scope = scope_stack.back();
                    tmpItem.access_modifier = "none";
                    leftside.push_back(tmpItem);
                    tmpItem.identifier = t3.lexeme;
                    tmpItem.scope = scope_stack.back();
                    tmpItem.access_modifier = "none";
                    rightside.push_back(tmpItem);
                    //cout << "stmt -> ID EQUAL ID SEMICOLON\n";
                    return 0;
                }
                else {
                    cout << "Syntax Error" << endl; 
                    exit(1);
                }
            }
            else {
                cout << "Syntax Error" << endl; 
                exit(1);
            }
        }
        else if (t2.token_type == LBRACE) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t1);
            parse_scope();
            //cout << "stmt -> scope\n";
            return 0;
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        cout << "Syntax Error" << endl; 
        exit(1);
    }
}

int parse_private_vars()
{
    t1 = lexer.GetToken();
    if (t1.token_type == PRIVATE) {
        t2 = lexer.GetToken();
        if (t2.token_type == COLON) {
            parse_var_list();
            t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                //cout<< "private_vars -> PRIVATE COLON var_list SEMICOLON\n";
            }
            else {
                cout << "Syntax Error" << endl; 
                exit(1);
            }
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        lexer.UngetToken(t1);
    }
    return 0;
}

int parse_public_vars() 
{
    t1 = lexer.GetToken();
    if (t1.token_type == PUBLIC) {
        t2 = lexer.GetToken();
        if (t2.token_type == COLON) {
            parse_var_list();
            t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                //cout<< "public_vars -> PUBLIC COLON var_list SEMICOLON\n";
            }
            else {
                cout << "Syntax Error" << endl; 
                exit(1);
            }
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        lexer.UngetToken(t1);
    }
    return 0;
}

int parse_var_list()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t2 = lexer.GetToken();
        if (t2.token_type == COMMA) {
            PushBuffer(t1.lexeme);
            parse_var_list();
            //cout << "var_list -> ID COMMA var_list\n";
            return 0;
        }
        else {
            lexer.UngetToken(t2);
            PushBuffer(t1.lexeme);
            //cout << "var_list -> ID\n";
            return 0;
        }
    }
    else {
        cout << "Syntax Error" << endl; 
        exit(1);
    }
}

int parse_global_vars() 
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_var_list();
        t2 = lexer.GetToken();
        if (t2.token_type == SEMICOLON) {
            //cout << "global_vars -> var_list SEMICOLON\n";
            return 0;
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        cout << "Syntax Error" << endl; 
        exit(1);
    }
    return 0;
}

int parse_scope()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t2 = lexer.GetToken();
        if (t2.token_type == LBRACE) {
            s1 = t1.lexeme;
            scope_stack.push_back(s1);
            
            parse_public_vars();

            while (!IsBufferEmpty()) {
                tmpItem.identifier = PopBuffer();
                tmpItem.scope = s1;
                tmpItem.access_modifier = "public";
                ID_list.push_back(tmpItem);
            }

            parse_private_vars();

            while (!IsBufferEmpty()) {
                tmpItem.identifier = PopBuffer();
                tmpItem.scope = s1;
                tmpItem.access_modifier = "private";
                ID_list.push_back(tmpItem);
            }

            tmpNode.scope = s1;
            tmpNode.parent = scope_stack[scope_stack.size()-2];
            scope_link.push_back(tmpNode);

            parse_stmt_list();
            t3 = lexer.GetToken();
            if (t3.token_type == RBRACE) {
                scope_stack.pop_back();
                //cout << "scope -> ID LBRACE public_vars private_vars stmt_list RBRACE\n";
                return 0;
            }
            else {
                cout << "Syntax Error" << endl; 
                exit(1);
            }
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        cout << "Syntax Error" << endl; 
        exit(1);
    }
    return 0;
}

int parse_program()
{
    scope_stack.push_back("global");
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t2 = lexer.GetToken();
        if (t2.token_type == LBRACE) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t1);

            tmpNode.scope = "global";
            tmpNode.parent = "none";
            scope_link.push_back(tmpNode);

            parse_scope();
            //cout << "program -> epsilon scope\n";            
            return 0;
        }
        else if (t2.token_type == COMMA || t2.token_type == SEMICOLON) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t1);
            parse_global_vars();
           
            while (!IsBufferEmpty()) {
                tmpItem.identifier = PopBuffer();
                tmpItem.scope = "global";
                tmpItem.access_modifier = "public";
                ID_list.push_back(tmpItem);
            }

            tmpNode.scope = "global";
            tmpNode.parent = "none";
            scope_link.push_back(tmpNode);

            t1 = lexer.GetToken();
            if (t1.token_type == ID) {
                t2 = lexer.GetToken();
                if (t2.token_type == LBRACE) {
                    lexer.UngetToken(t2);
                    lexer.UngetToken(t1);
                    parse_scope();
                    //cout << "program -> global_vars scope\n";                    
                    return 0;
                }
                else {
                    cout << "Syntax Error" << endl; 
                    exit(1);
                }
            }
            else {
                cout << "Syntax Error" << endl; 
                exit(1);
            }
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        cout << "Syntax Error" << endl; 
        exit(1);
    }
    return 0;
}

string checkPrefix(item x)
{
    string s1;
    int check1 = 0;
    vector<string> scope_list;
    tmpNode.scope = x.scope;
    while (check1 != 1){
        for (int i=0; i<scope_link.size(); ++i) {
            if (tmpNode.scope == scope_link[i].scope) {
                scope_list.push_back(scope_link[i].scope);
                if (scope_link[i].parent != "none") {
                    tmpNode.scope = scope_link[i].parent;
                    break;
                }
                else {
                    check1 = 1;
                    break;
                }
            }
        }
    }
    
    for (int i=0; i<scope_list.size(); ++i) {
        for (int j=0; j<ID_list.size(); ++j) {
            if (x.identifier == ID_list[j].identifier && x.scope == ID_list[j].scope) {
                s1 = ID_list[j].scope + ".";
                return s1;
            }
            
            if (x.identifier == ID_list[j].identifier && x.scope != ID_list[j].scope && scope_list[i] == ID_list[j].scope && ID_list[j].scope != "global" && ID_list[j].access_modifier == "public") {
                
                s1 = ID_list[j].scope + ".";
                return s1;
            }

            if (x.identifier == ID_list[j].identifier && x.scope != ID_list[j].scope && scope_list[i] == ID_list[j].scope && ID_list[j].scope == "global") {
                return "::";
            }
        }
    }
    return "?.";
}

void printAssignments()
{
    for (int i=0; i<leftside.size(); ++i) {
        cout << checkPrefix(leftside[i]) << leftside[i].identifier << " = " << checkPrefix(rightside[i]) << rightside[i].identifier << endl;
    }
}

int main()
{
    parse_program();

    //cout << "---- parsing done ----" << endl;

    t3 = lexer.GetToken();
    if (t3.token_type != END_OF_FILE) {
        cout << "Syntax Error" << endl; 
        exit(1);
    }

   printAssignments();

   return 0;
}
