//
//  SRExpressionsParser.h
//  LSystemsAndExpressions
//
//  Created by Jürgen Schwietering on 5/18/12.
//  Copyright (c) 2012 Jürgen Schwietering All rights reserved.
//

#ifndef SRExpressionsParser_h
#define SRExpressionsParser_h
#include "SRErrorCodes.h"
#include "Token.h"

#define SR_SET_MAXDEPTH 20
#define SR_VARLEN   40              /* Max length of variable names */
#define SR_MAXVARS 256              /* Max user-defined variables */
#define SR_TOKLEN  256              /* Max token length (can be long)*/


typedef struct
{
	char name[SR_VARLEN+1];// set name (always {name})
	char *values;			// values in set
	double iterateStep;	// 0 if no iteration but fix values
	double start, end;	// iterations start and end value
	double n;				// temporary value for current evaluation
	char *currentValue;		// if no iteration points to current value
} VARSET;


class SRExpressionsParser
{
public:
	SRExpressionsParser();
	~SRExpressionsParser();
private:
	typedef enum {
		VT_UNKNOWN,
		VT_VAR,
		VT_SET,
		VT_DEF,
		VT_SUBST,
		VT_DEL,
		VT_NUM} VALTYPE;
    
    int expError;
	char errToken[SR_TOKLEN + 1];
	long errBufPos;
	const char* errAnc;
	const char* expression;
	Token token;
	VALTYPE type;
	VARSET unnamedSet[SR_SET_MAXDEPTH];
    
	void parse(void);
	bool assign(double* r);
    
    // precendence from weakest to strongest
    
	void ternary(double* r);
	void logicalOr(double* r);
	void logicalAnd(double* r);
	void bitwiseOr(double* r);
	void bitwiseXor(double* r);
	void bitwiseAnd(double* r);
	void equalUnequalTo(double* r);
	void compare(double* r);
	void bitwiseShift(double* r);
	void addSub(double* r);
	void mulDiv(double* r);
	void pow(double* r);
	void unary(double* r); // logical not, bitwise ~
	void varFunc(double* r);
	bool getValue(char* name, double* value);
   
	int evalSplitSet(char *ln);
    //	bool getIterators(VARSET *v);
    //	char *setPatterns[SET_MAXDEPTH+1];
    //	int setDepth; // if set calculation then > 0
    //	bool firstSet(void);
    //	bool nextSet(double *val, int level);
    //	void setListFirstValue(VARSET *v);
public:
	void printError(void);
	EVALEXPR_RESULT evaluate(const char* e, double* result, int* a);
};

#endif
