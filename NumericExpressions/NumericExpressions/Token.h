//
//  Token.h
//  NumericExpressions
//
//  Created by Schwietering, Jürgen on 27.12.13.
//  Copyright (c) 2013 Schwietering, Jürgen. All rights reserved.
//

#ifndef __NumericExpressions__Token__
#define __NumericExpressions__Token__

class Token
{
private:
    int wPos;
    int curMax;
    char *token;
public:
    Token();
    ~Token();
    Token(const Token &obj);  // copy constructor
    Token& operator= (const Token &source);
    void reset();
    void add(char c);
    char *value();
};

#endif /* defined(__NumericExpressions__Token__) */
