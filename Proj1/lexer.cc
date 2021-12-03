
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR",
	"REALNUM", "BASE08NUM", "BASE16NUM"// TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

bool IsPdigit08(char c) {return c >= '1' && c <= '7';}

bool IsPdigit16(char c) {return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'F');}

bool IsDigit08(char c) {return c >= '0' && c <= '7';}

bool IsDigit16(char c) {return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');}

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

bool LexicalAnalyzer::IsFollowedBy(string s1) 
{
	char c;
	string s2="";
	for (int i = 0; i < s1.size(); i++) {
		input.GetChar(c);
        s2 += c;
		if (input.EndOfInput() || c != s1[i]) {
            UngetStr(s2);
            return false;
		}
    }
    return true;
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

string LexicalAnalyzer::TestBASE08NUM()
{
	char c;
    string s = "";
    input.GetChar(c);
    s += c;
	if (c == '0') {
        if (IsFollowedBy("x08")) {
            s += "x08";
            return s; 
        }
    }
	else if (IsPdigit08(c)) {
		input.GetChar(c);
        if (input.EndOfInput()) {
            s += c;
            UngetStr(s);
            return "";
        }
		while(!input.EndOfInput() && IsDigit08(c)) {
            s += c;
            input.GetChar(c);
        }
        if (input.EndOfInput()) {
            s += c;
            UngetStr(s);
            return "";
        }
        if (!input.EndOfInput()) {input.UngetChar(c);}
        if (IsFollowedBy("x08")) {
            s += "x08";
            return s;
        }
    }
    UngetStr(s);
    return "";
}

string LexicalAnalyzer::TestBASE16NUM()
{
    char c;
    string s = "";
    input.GetChar(c);
    s += c;
	if (c == '0') {
        if (IsFollowedBy("x16")) {
            s += "x16";
            return s; 
        }
    }
	else if (IsPdigit16(c)) {
		input.GetChar(c);
        if (input.EndOfInput()) {
            s += c;
            UngetStr(s);
            return "";
        }
		while(!input.EndOfInput() && IsDigit16(c)) {
            s += c;
            input.GetChar(c);
        }
        if (input.EndOfInput()) {
            s += c;
            UngetStr(s);
            return "";
        }
        if (!input.EndOfInput()) {input.UngetChar(c);}
        if (IsFollowedBy("x16")) {
            s += "x16";
            return s;
        }
    }
    UngetStr(s);
    return "";
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
    s = TestBASE16NUM();
    if (s.size() > 0) {
        tmp.lexeme = s;
        tmp.token_type = BASE16NUM;
        tmp.line_no = line_no;
        return tmp;
    }
    s = TestBASE08NUM();
    if (s.size() > 0) {
        tmp.lexeme = s;
        tmp.token_type = BASE08NUM;
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

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
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
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
