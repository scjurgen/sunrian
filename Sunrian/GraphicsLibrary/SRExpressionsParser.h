//
//  SRExpressionsParser.h
//  LSystemsAndExpressions
//
//  Created by Jürgen Schwietering on 5/18/12.
//  Copyright (c) 2012 Jürgen Schwietering All rights reserved.
//

#ifndef SRExpressionsParser_h
#define SRExpressionsParser_h


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
    void reset();
    void add(char c);
    char *value();
};


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
    
	void Parse(void);
	bool Assign(double* r);
    
    // precendence from weakest to strongest
    
	void Ternary(double* r);
	void LogicalOr(double* r);
	void LogicalAnd(double* r);
	void BitwiseOr(double* r);
	void BitwiseXor(double* r);
	void BitwiseAnd(double* r);
	void EqualUnequalTo(double* r);
	void Compare(double* r);
	void BitwiseShift(double* r);
	void AddSub(double* r);
	void MulDiv(double* r);
	void Pow(double* r);
	void Unary(double* r); // logical not, bitwise ~
	void VarFunc(double* r);
	bool GetValue(char* name, double* value);
   
	int evalSplitSet(char *ln);
    //	bool getIterators(VARSET *v);
    //	char *setPatterns[SET_MAXDEPTH+1];
    //	int setDepth; // if set calculation then > 0
    //	bool firstSet(void);
    //	bool nextSet(double *val, int level);
    //	void setListFirstValue(VARSET *v);
public:
	void printError(void);
	EVALEXPR_RESULT Evaluate(const char* e, double* result, int* a);
};

#endif
