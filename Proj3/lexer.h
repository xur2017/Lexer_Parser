
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    INT, REAL, BOOL, TRUE, FALSE,
    IF, WHILE, SWITCH, CASE,
    NOT, PLUS, MINUS, MULT, DIV,
    GREATER, LESS, GTEQ, LTEQ, NOTEQUAL,
    LPAREN, RPAREN,
    NUM, REALNUM,
    PUBLIC, PRIVATE,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRACE, RBRACE, ID, ERROR
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool IsKeyword(std::string);
	  
    bool SkipComment();

    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();

    void UngetStr(std::string);
	  std::string TestNUM();
    std::string TestREALNUM();
    Token ScanNumber();
};

#endif  //__LEXER__H__
