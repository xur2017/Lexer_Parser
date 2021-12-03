
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "INT", "REAL", "BOOL", "TRUE", "FALSE",
    "IF", "WHILE", "SWITCH", "CASE",
    "NOT", "PLUS", "MINUS", "MULT", "DIV",
    "GREATER", "LESS", "GTEQ", "LTEQ", "NOTEQUAL",
    "LPAREN", "RPAREN",
    "NUM", "REALNUM",
    "PUBLIC", "PRIVATE",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 9
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComment()
{
    char c;
    bool comment_encountered = false;
    
    input.GetChar(c);
    if (c == '/') {
        input.GetChar(c);
        if (c == '/') {
            comment_encountered = true;
            while (!input.EndOfInput() && c != '\n') {
                input.GetChar(c);
            }
            line_no += (c == '\n');
            SkipComment();
        }
        else {
            cout << "Syntax Error" << endl; 
            exit(1);
        }
    }
    else {
        input.UngetChar(c);
    }
    return comment_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

void LexicalAnalyzer::UngetStr(string s)
{
    for (int i = 0; i < s.size(); i++) {input.UngetChar(s[s.size()-i-1]);}
}

string LexicalAnalyzer::TestNUM()
{
	char c;
	string s = "";
	input.GetChar(c);
    s += c;
	if (c=='0') {
		return s;
	}
	else {
		input.GetChar(c);
		while (!input.EndOfInput() && isdigit(c)) {
			s += c;
			input.GetChar(c);
		}
		if (!input.EndOfInput()) {input.UngetChar(c);}
		return s;
	}
}

string LexicalAnalyzer::TestREALNUM()
{
    char c;
	string s = "";
	input.GetChar(c);
    s += c;
    if (c=='0') {
		input.GetChar(c);
        if (c == '.') {
            s += c;
            input.GetChar(c);
            if (isdigit(c)) {
                while (!input.EndOfInput() && isdigit(c)) {
                    s += c;
                    input.GetChar(c);
                }
                if (!input.EndOfInput()) {input.UngetChar(c);}
                return s;
            }
            else {
                s += c;
                UngetStr(s);
                return "";
            }
        }
        else {
            s += c;
            UngetStr(s);
            return "";
        }
	}
    else {
        input.GetChar(c);
        if (input.EndOfInput()) {
            s += c;
            UngetStr(s);
            return "";
        }
        while (!input.EndOfInput() && isdigit(c)) {
            s += c;
            input.GetChar(c);
        }
        if (input.EndOfInput()) {
            s += c;
            UngetStr(s);
            return "";
        }
        if (!input.EndOfInput()) {input.UngetChar(c);}
        input.GetChar(c);
        if (c == '.') {
            s += c;
            input.GetChar(c);
            if (isdigit(c)) {
                while (!input.EndOfInput() && isdigit(c)) {
                    s += c;
                    input.GetChar(c);
                }
                if (!input.EndOfInput()) {input.UngetChar(c);}
                return s;
            }
            else {
                s += c;
                UngetStr(s);
                return "";
            }
        }
        else {
            s += c;
            UngetStr(s);
            return "";
        }
    }
}

Token LexicalAnalyzer::ScanNumber()
{
    string s;

    s = TestREALNUM();
    if (s.size() > 0) {
        tmp.lexeme = s;
        tmp.token_type = REALNUM;
        tmp.line_no = line_no;
        return tmp;
    }

    s = TestNUM();
    if (s.size() > 0) {
        tmp.lexeme = s;
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    /*
    while (SkipComment()) {
        SkipSpace();
    }
    */
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '!':
            tmp.token_type = NOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            }
            else {
                input.UngetChar(c);
                tmp.token_type = GREATER;
            }
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            }
            else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            }
            else {
                input.UngetChar(c);
                tmp.token_type = LESS;
            }
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            }
            else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            }
            else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;
            return tmp;
    }
}
