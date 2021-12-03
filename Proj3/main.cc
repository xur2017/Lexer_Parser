
#include <iostream>
#include <istream>
#include <vector>
#include <deque>
#include <string>
#include <cctype>
#include <stdlib.h>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

LexicalAnalyzer lexer;
Token t1, t2, t3, t4, t5;
int i1, i2 = 4;

struct item {string ID_name; int ID_type;} tmpItem;
vector<item> ID_list;

deque<string> ID_buffer;

int update_ID_types(int oldType, int newType)
{
    for (int i=0; i<ID_list.size(); ++i) {
        if (ID_list[i].ID_type == oldType) {
            ID_list[i].ID_type = newType;
        }
    }
    return 0;
}

int get_ID_type(Token tk)
{
    for (int i=0; i<ID_list.size(); ++i) {
        if (ID_list[i].ID_name == tk.lexeme) {
            return ID_list[i].ID_type;
        }
    }
}

int add_ID_token(Token tk)
{
    int check = 0;
    for (int i=0; i<ID_list.size(); ++i) {
        if (ID_list[i].ID_name == tk.lexeme) {
            check = 1;
            break;
        }
    }
    if (check == 0) {
        tmpItem.ID_name = tk.lexeme;
        tmpItem.ID_type = i2;
        ID_list.push_back(tmpItem);
        i2++;
    }
    return 0;
}

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
    s = ID_buffer.front();
    ID_buffer.pop_front();
    return s;
}

int parse_program();
int parse_global_vars();
int parse_var_decl_list();
int parse_var_decl();
int parse_type_name();
int parse_body();
int parse_stmt_list();
int parse_stmt();
int parse_if_stmt();
int parse_while_stmt();
int parse_switch_stmt();
int parse_case_list();
int parse_case();
int parse_assignment_stmt();
int parse_expression();
int parse_primary();
int parse_binary_operator();
int parse_unary_operator();
int parse_var_list();

int parse_var_list()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        t2 = lexer.GetToken();
        if (t2.token_type == COMMA) {
            PushBuffer(t1.lexeme);
            parse_var_list();
            return 0;
        }
        else {
            lexer.UngetToken(t2);
            PushBuffer(t1.lexeme);
            return 0;
        }
    }
    return 0;
}

int parse_unary_operator()
{
    t1 = lexer.GetToken();
    if (t1.token_type == NOT) {
        return NOT;
    }
}

int parse_binary_operator()
{
    t1 = lexer.GetToken();
    if (t1.token_type == PLUS) {
        return PLUS;
    }
    else if (t1.token_type == MINUS) {
        return MINUS;
    }
    else if (t1.token_type == MULT) {
        return MULT;
    }
    else if (t1.token_type == DIV) {
        return DIV;
    }
    else if (t1.token_type == GREATER) {
        return GREATER;
    }
    else if (t1.token_type == LESS) {
        return LESS;
    }
    else if (t1.token_type == GTEQ) {
        return GTEQ;
    }
    else if (t1.token_type == LTEQ) {
        return LTEQ;
    }
    else if (t1.token_type == EQUAL) {
        return EQUAL;
    }
    else if (t1.token_type == NOTEQUAL) {
        return NOTEQUAL;
    }
}

int parse_primary()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        add_ID_token(t1);
        return get_ID_type(t1);
    }
    else if (t1.token_type == NUM) {
        return INT;
    }
    else if (t1.token_type == REALNUM) {
        return REAL;
    }
    else if (t1.token_type == TRUE || t1.token_type == FALSE) {
        return BOOL;
    }
}

int parse_expression()
{
    int tp, ltp, rtp;
    t1 = lexer.GetToken();
    if (t1.token_type == ID || t1.token_type == NUM || t1.token_type == REALNUM || t1.token_type == TRUE || t1.token_type == FALSE) {
        lexer.UngetToken(t1);
        tp = parse_primary();
    }
    else if (t1.token_type == NOT) {
        lexer.UngetToken(t1);
        parse_unary_operator();
        tp = parse_expression();
        if (tp<=3 && tp != BOOL) {
            cout << "TYPE MISMATCH " << t1.line_no << " C3"<<endl;
            exit(1);
        }
        else if (tp >= 4) {
            update_ID_types(tp, BOOL);
            tp = BOOL;
        }
    }
    else {
        lexer.UngetToken(t1);
        tp = parse_binary_operator();
        ltp = parse_expression();
        rtp = parse_expression();
        if (ltp != rtp) {
            if (tp == PLUS || tp == MINUS || tp == MULT || tp == DIV) {
                if (ltp <= 3 && rtp >= 4){
                    update_ID_types(rtp, ltp);
                    rtp = ltp;
                }
                else if (ltp >= 4 && rtp <= 3) {
                    update_ID_types(ltp, rtp);
                }
                else if (ltp >= 4 && rtp >= 4) {
                    update_ID_types(rtp, ltp);
                    rtp = ltp;
                }
                else {
                    cout << "TYPE MISMATCH " << t1.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else if (tp == GREATER || tp == LESS || tp == GTEQ || tp == LTEQ || tp == NOTEQUAL || tp == EQUAL) {
                if (ltp <= 3 && rtp >= 4) {
                    update_ID_types(rtp, ltp);
                }
                else if (ltp >= 4 && rtp <= 3) {
                    update_ID_types(ltp, rtp);
                }
                else if (ltp >= 4 && rtp >= 4) {
                    update_ID_types(rtp, ltp);
                }
                else {
                    cout << "TYPE MISMATCH " << t1.line_no << " C2"<<endl;
                    exit(1);
                }
            }
        }
        
        if (tp == GREATER || tp == LESS || tp == GTEQ || tp == LTEQ || tp == NOTEQUAL || tp == EQUAL) {
            tp = BOOL;
        }
        else if (tp == PLUS || tp == MINUS || tp == MULT || tp == DIV) {
            tp = rtp;
        }
    }
    return tp;
}

int parse_assignment_stmt() 
{
    int tp, ltp, rtp;
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        add_ID_token(t1);
        ltp = get_ID_type(t1);
        t2 = lexer.GetToken();
        if (t2.token_type == EQUAL) {
            rtp = parse_expression();
            if (ltp <= 3) {
                if (ltp != rtp) {
                    if (rtp <= 3) {
                        cout << "TYPE MISMATCH " << t1.line_no << " C1" << endl;
                        exit(1);
                    }
                    else {
                        update_ID_types(rtp, ltp);
                    }
                }
            }
            else {
                update_ID_types(ltp, rtp);
            }
            t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                return 0;
            }
        }
    }
    return 0;
}

int parse_case()
{
    t1 = lexer.GetToken();
    if (t1.token_type == CASE) {
        t2 = lexer.GetToken();
        if (t2.token_type == NUM) {
            t3 = lexer.GetToken();
            if (t3.token_type == COLON) {
                parse_body();
                return 0;
            }
        }
    }
    return 0;
}

int parse_case_list()
{
    t1 = lexer.GetToken();
    if (t1.token_type == CASE) {
        lexer.UngetToken(t1);
        parse_case();
        t2 = lexer.GetToken();
        if (t2.token_type == CASE) {
            lexer.UngetToken(t2);
            parse_case_list();
        }
        else if (t2.token_type == RBRACE) {
            lexer.UngetToken(t2);
        }
    }
    return 0;
}

int parse_switch_stmt()
{
    int tp;
    t1 = lexer.GetToken();
    if (t1.token_type == SWITCH) {
        t2 = lexer.GetToken();
        if (t2.token_type == LPAREN) {
            tp = parse_expression();
            if (tp<=3 && tp != INT) {
                cout<< "TYPE MISMATCH " << t1.line_no << " C5"<<endl;
                exit(1);
            }
            else if (tp >= 4) {
                update_ID_types(tp, INT);
            }
            t3 = lexer.GetToken();
            if (t3.token_type == RPAREN) {
                t4 = lexer.GetToken();
                if (t4.token_type = LBRACE) {
                    parse_case_list();
                    t5 = lexer.GetToken();
                    if (t5.token_type = RBRACE) {
                        return 0;
                    }
                }
            }
        }
    }
    return 0;
}

int parse_while_stmt()
{
    int tp;
    t1 = lexer.GetToken();
    if (t1.token_type == WHILE) {
        t2 = lexer.GetToken();
        if (t2.token_type == LPAREN) {
            tp = parse_expression();
            if (tp<=3 && tp != BOOL) {
                cout<< "TYPE MISMATCH " << t1.line_no << " C4" << endl;
                exit(1);
            }
            else if (tp >= 4) {
                update_ID_types(tp, BOOL);
            }
            t3 = lexer.GetToken();
            if (t3.token_type == RPAREN) {
                parse_body();
                return 0;
            }
        }
    }
    return 0;
}

int parse_if_stmt()
{
    int tp;
    t1 = lexer.GetToken();
    if (t1.token_type == IF) {
        t2 = lexer.GetToken();
        if (t2.token_type == LPAREN) {
            tp = parse_expression();
            if (tp <= 3 && tp != BOOL) {
                cout<< "TYPE MISMATCH " << t1.line_no << " C4" << endl;
                exit(1);
            }
            else if (tp >= 4) {
                update_ID_types(tp, BOOL);
            }
            t3 = lexer.GetToken();
            if (t3.token_type == RPAREN) {
                parse_body();
                return 0;
            }
        }
    }
    return 0;
}

int parse_stmt()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_assignment_stmt();
    }
    else if (t1.token_type == IF) {
        lexer.UngetToken(t1);
        parse_if_stmt();
    }
    else if (t1.token_type == WHILE) {
        lexer.UngetToken(t1);
        parse_while_stmt();
    }
    else if (t1.token_type == SWITCH) {
        lexer.UngetToken(t1);
        parse_switch_stmt();
    }
    return 0;
}

int parse_stmt_list() 
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID || t1.token_type == IF || t1.token_type == WHILE || t1.token_type == SWITCH) {
        lexer.UngetToken(t1);
        parse_stmt();
        t2 = lexer.GetToken();
        if (t2.token_type == ID || t2.token_type == IF || t2.token_type == WHILE || t2.token_type == SWITCH) {
            lexer.UngetToken(t2);
            parse_stmt_list();
        }
        else if (t2.token_type == RBRACE) {
            lexer.UngetToken(t2);
        }
    }
    return 0;
}

int parse_body()
{
    t1 = lexer.GetToken();
    if (t1.token_type == LBRACE) {
        parse_stmt_list();
        t2 = lexer.GetToken();
        if (t2.token_type == RBRACE) {
            return 0;
        }
    }
    return 0;
}

int parse_type_name()
{
    t1 = lexer.GetToken();
    if (t1.token_type == INT) {
        i1 = INT;
    }
    else if (t1.token_type == REAL) {
        i1 = REAL;
    }
    else if (t1.token_type == BOOL) {
        i1 = BOOL;
    }
    return 0;
}

int parse_var_decl()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_var_list();
        t2 = lexer.GetToken();
        if (t2.token_type == COLON) {
            parse_type_name();
            t3 = lexer.GetToken();
            if (t3.token_type == SEMICOLON) {
                while (!IsBufferEmpty()) {
                    tmpItem.ID_name = PopBuffer();
                    tmpItem.ID_type = i1;
                    ID_list.push_back(tmpItem);
                }
            }
        }
    }
    return 0;
}

int parse_var_decl_list()
{
    t1 = lexer.GetToken();
    while(t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_var_decl();
        t1 = lexer.GetToken();
    }
    lexer.UngetToken(t1);
    return 0;
}

int parse_global_vars()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_var_decl_list();
    }
    return 0;
}

int parse_program()
{
    t1 = lexer.GetToken();
    if (t1.token_type == ID) {
        lexer.UngetToken(t1);
        parse_global_vars();
        parse_body();
    }
    else if (t1.token_type == LBRACE) {
        lexer.UngetToken(t1);
        parse_body();
    }
    return 0;
}

void print_ID_list()
{
    string s1 = "";
    string tp1[] = { "int", "real", "bool"};

    for (int i=0; i<ID_list.size(); ++i) {
        if (ID_list[i].ID_type <= 3 && ID_list[i].ID_type > 0) {
            s1 = ID_list[i].ID_name + ": " + tp1[ID_list[i].ID_type-1] + " #";
            cout << s1 << endl; 
            s1 = "";
        }
        if (ID_list[i].ID_type >= 4) {
            s1 = ID_list[i].ID_name;
            for (int j=0; j<ID_list.size(); ++j) {
                if (ID_list[i].ID_type == ID_list[j].ID_type && i != j) {
                    s1 += ", " + ID_list[j].ID_name;
                    ID_list[j].ID_type = -1;
                }
            }
            ID_list[i].ID_type = -1;
            s1 += ": ? #";
            cout << s1 << endl;
            s1 = ""; 
        }
    }
}

int main()
{
    parse_program();

    print_ID_list();
    
    return 0;
}
