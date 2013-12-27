//
//  Token.cpp
//  NumericExpressions
//
//  Created by Schwietering, Jürgen on 27.12.13.
//  Copyright (c) 2013 Schwietering, Jürgen. All rights reserved.
//
#include <stdlib.h>
#include <string.h>

#include "Token.h"

#define SR_TOKLEN 256

Token::Token()
{
    token = (char*)malloc(SR_TOKLEN);
    curMax = SR_TOKLEN;
    reset();
}

Token::~Token()
{
    free(token);
}

Token::Token(const Token &obj)
{
    token = (char*)malloc(obj.curMax);
    memcpy(token,obj.token,obj.curMax);
}

Token& Token::operator= (const Token &source)
{
    if (this != &source) // protect against invalid self-assignment
    {
        char *newtoken = (char*)malloc(source.curMax);
        memcpy(newtoken,source.token,source.curMax);
        free(token);
        curMax = source.curMax;
        token = newtoken;
    }
    return *this;
}

void Token::reset()
{
    wPos=0;
    token[0]=0;
}

void Token::add(char c)
{
    if (wPos==curMax-1)
    {
        token = (char*)realloc(token, wPos+1+SR_TOKLEN);
        curMax = wPos+1+SR_TOKLEN;
    }
    token[wPos++]=c;
    token[wPos]=0;
}

char * Token::value()
{
    return token;
}
