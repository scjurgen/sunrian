//
//  SRExpressionsParser.cpp
//
//  Created by Jürgen Schwietering on 5/18/12.
//  Copyright (c) 2012 Jürgen Schwietering All rights reserved.
//

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "SRErrorCodes.h"
#include "SRExpressionsParser.h"


typedef struct
{
	char name[SR_VARLEN + 1]; /* Variable name */
	double value;             /* Variable value */
	char *comment;
} VARIABLE;


typedef struct
{
	const char *name;
	double  (*func)();
} FUNCTION_NULL;


typedef struct
{
	const char *name;          /* Function name */
	int   args;                /* Number of arguments to expect */
	double  (*func)(double);   /* Pointer to function */
} FUNCTION;


typedef struct
{
	const char *name;
	int   args;
	double  (*func)(double, double);
} FUNCTION2;

typedef struct
{
	const char *name;
	double  (*func)(int n, double *a);
} FUNCTIONN;

/* special is ctype */
#define isNumeric(c)  ((c >= '0' && c <= '9') || c == '.')
#define isLetter(c)  ((c >= 'a' && c <= 'z')||(c >= 'A' && c <= 'Z'))
#define isAlpha(c)  (isLetter(c) || (c >= '0' && c <= '9') || c=='_' || c=='.')
#define isDelim(c)  (c == '<' ||c == '>' ||	\
c == '!' ||c == '~' ||		\
c == '#' || c == '?' || c == ':' || 	\
c == '+' || c == '-' || 	\
c == '*' || c == '/' || c == '%' || 	\
c == '^' || c == '(' || c == ')' || 	\
c == ',' || c == '='|| 	\
c == '|' || c == '&')

static VARIABLE Vars[SR_MAXVARS];
/** Array for user-defined variables 
this array is global to this file so it is shared between various expression objects.
That is nice but makes it prone to multithread errors ;-), 
use some mechanism to prevent that kind of error...
*/

double two=2.0;
double one=1.0;

double getPi(double arc, double factor)
{
  return asinl(arc)*factor;
}

static VARIABLE const constants[] =
{
    { "pi", M_PI, const_cast<char *>("pi") },
    { "e", M_E, const_cast<char *>("e") },
	{ "", 0,0 }
};

static inline double ldexpi(double x,double n)
{
	return (double)ldexpl((double)x,(int)n);
}

static inline double lb(double x)
{
	return (double)(log10l((double)x)/log10l(2.0));
}

// math function wrappers
static inline double deg(double x)
{
	return(double)(x * 180.0L / M_PI);
}

static inline double rad(double x)
{
	return(double)(x * M_PI / 180.0L);
}


static inline double andOp(double x, double y)
{
	return (double)((long long) x & (long long) y);
}

static inline double orOp(double x, double y)
{
	return (double)((long long) x | (long long) y);
}

static inline double xorOp(double x, double y)
{
	return (double)((long long) x ^ (long long) y);
}

static inline double gt(double x, double y)
{
	return (double)(x>y);
}

static inline double gte(double x, double y)
{
	return (double)(x>=y);
}

static inline double le(double x, double y)
{
	return (double)(x<y);
}

static inline double lee(double x, double y)
{
	return (double)(x<=y);
}

static inline double equal(double x, double y)
{
	return (double)(x==y);
}

static inline double unequal(double x, double y)
{
	return (double)(x!=y);
}


static inline double complOp(double x)
{
	return (double)(~(long long) x);
}

static inline double frexpm(double c)
{
	int exp;
	frexp((double)c,&exp);
	return (double) exp;
}
static inline double frexp(double c)
{
	int exp;
	return (double) frexpl(c,&exp);
}


#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

static inline double equirand()
{
	return (double)rand()*RS_SCALE;
}


// see http://en.wikipedia.org/wiki/Normal_distribution
static double drand (void) {
	double d;
	do {
		d = (((rand () * RS_SCALE) + rand ()) * RS_SCALE + rand ()) * RS_SCALE;
	} while (d >= 1.0); 
	return d;
}

#define irand(x) ((unsigned int) ((x) * drand ()))

// Box-Muller optimized
static double gaussRandom() 
{
	double u,v,r,c;
	u = 2.0*drand()-1;
	v = 2.0*drand()-1;
	r = u*u + v*v;
	if (r == 0.0 || r > 1.0) 
		return gaussRandom();

	c = sqrtl(-2*logl(r)/logl(M_E)/r);
	return u*c;
}

static inline double createMemberInNormalDistribution(double mean,double std_dev)
{
	return mean + (gaussRandom()*std_dev);
}


static double fib(double c)
{
	long long fn=1.0,f0=0,f1=1;
    unsigned int iterations=(unsigned int)c;
    if (iterations==0)
        return 0.0;
    if (iterations==1)
        return 1.0;
	for (int i=2; i <= iterations; i++)
	{
		fn = f0+f1;
		f0 = f1;
		f1 = fn;
	}
	return (double) fn;
}

static FUNCTION_NULL nullary[] =
{
	{ "rand",   equirand},
	{ "rnd",    equirand},
	{ "drnd",   drand},// density random
	{ "grnd",   gaussRandom},// gaussian random
	{ "",0}
};


static FUNCTION func1p[] =
{
	/* name, function to call */
	// math calc

	{ "sin",     1,    sin},
	{ "cos",     1,    cos},
	{ "tan",     1,    tan},
	{ "asin",    1,    asin},
	{ "acos",    1,    acos},
	{ "atan",    1,    atan},
	{ "sinh",    1,    sinh},
	{ "cosh",    1,    cosh},
	{ "tanh",    1,    tanh},
	{ "exp",     1,    exp},
	{ "log",     1,    log},
	{ "lb",      1,    lb},
	{ "log10",   1,    log10},
	{ "sqrt",    1,    sqrt},
	{ "floor",   1,    floor},
	{ "ceil",    1,    ceil},
	{ "abs",     1,    fabs},
	{ "deg",     1,    deg},
	{ "rad",     1,    rad},
	{ "frexp",   1,    frexp},// get exponent e of (x = m*2^e)
	{ "frexpm",  1,    frexpm},//get mantissa m of (x = m*2^e)
	{ "fib",     1,    fib},// get n-th fibonacci nr
	{ "not",     1,    complOp}, // boolean compl
	{ "",0,0 }
};


static double gcd(double m, double n)
{
	long long modnm;
	if (fabs(m)<=1.0)
		return m;
	if (fabs(n)<=1.0)
		return m;
	modnm=(long long)n % (long long) m;
	if (modnm==0)
	{
		return m;
	}
	else
		return gcd((double)modnm, m);
}


static FUNCTION2 func2p[] =
{
	/* name, function to call */
	{ "hypot",   2,    hypot},
	{ "pow",     2,    pow},
	{ "angle",   2,    atan2},
	{ "fmod",    2,    fmod},
	{ "ldexp",   2,    ldexpi},
	{ "cmind",   2,    createMemberInNormalDistribution},

	{ "gcd",     2,    gcd},
	{ "and",     2,    andOp},
	{ "or",      2,    orOp},
	{ "xor",     2,    xorOp},
	{ "gt",      2,    gt},
	{ "gte",     2,    gte},
	{ "le",      2,    le},
	{ "lee",     2,    lee},
	{ "equal",   2,    equal},
	{ "unequal", 2,    unequal},
	{ 0 }
};



static double avgn(int cnt, double *a)
{
	int i;
	double sum=(double)0;
	if (cnt<1)
		return (double)0;
	for (i=0; i < cnt; i++)
		sum+=a[i];
	return sum/(double)cnt;
}

static double geon(int cnt, double *a)
{
	int i;
	double prod=(double)1;
	if (cnt<1)
		return (double)0;
	for (i=0; i < cnt; i++)
		prod*=a[i];
	return powl(prod,1/(double)cnt);
}


static double maxn(int cnt, double *a)
{
	int i;
	if (cnt<1)
		return (double)0;
	double sum=a[0];
	for (i=1; i < cnt; i++)
	{
		if (a[i]>sum)
			sum=a[i];
	}
	return sum;
}


static double minn(int cnt, double *a)
{
	int i;
	if (cnt<1)
		return (double)0;
	double sum=a[0];
	for (i=1; i < cnt; i++)
	{
		if (a[i]<sum)
			sum=a[i];
	}
	return sum;
}

static FUNCTIONN funcnp[]=
{
	{ "avg",  avgn},
	{ "geo",  geon},
	{ "max",  maxn},
	{ "min",  minn},
	{ 0 }
};


void ClearAllVars(void)
{
	int i;
	for(i = 0; i < SR_MAXVARS; i++)
	{
		*Vars[i].name = 0;
		Vars[i].value = 0;
	}
}


int ClearVar(char* name)
{
	int i;
	for(i = 0; i < SR_MAXVARS; i++)
	{
		if(*Vars[i].name && ! strcmp(name, Vars[i].name))
		{
			*Vars[i].name = 0;
			Vars[i].value = 0;
			return 1;
		}
	}
	return(0);
}


// constructor should also load the variables
SRExpressionsParser::SRExpressionsParser()
{
	expression=0;
	errBufPos=0;
	errAnc=expression;
	errToken[0]=0;
	expError=E_EMPTY;
}

SRExpressionsParser::~SRExpressionsParser()
{
	// until now nothing to do...
}


/**
* GetValue(char* name, double* value)                                 
*                                                                       
* Looks up the specified variable (or constant) known as NAME and       
* returns its contents in VALUE.                                        
*                                                                       
* First the user-defined variables are searched, then the constants are 
* searched.                                                             
*                                                                       
* Returns 1 if the value was found, or 0 if it wasn't.                  
*/

bool SRExpressionsParser::getValue(char* name, double* value)
{
	int i;

	/* Now check the user-defined variables. */
	for(i = 0; i < SR_MAXVARS; i++)
	{
		if(*Vars[i].name && ! strcmp(name, Vars[i].name))
		{
			*value = Vars[i].value;
			return true;
		}
	}
  //if (strcmp(name,"pi")==0)
  //{
  //  *value=getPi(1.0,2.0);
  //  return true;
  //}
	/* Now check the programmer-defined constants. */
	for(i = 0; constants[i].name[0]; i++)
	{
		if(*constants[i].name && ! strcmp(name, constants[i].name))
		{
			*value = constants[i].value;
			return true;
		}
	}
	/* Now check nullary functinos */
	for(i = 0; nullary[i].name[0]; i++)
	{
		if(*nullary[i].name && ! strcmp(name, nullary[i].name))
		{
			*value = nullary[i].func();
			return true;
		}
	}
	return false;
}


/**
* SetValue (char* name, double* value)
*                                                                       
* First, it erases any user-defined variable that is called NAME.  Then
* it creates a new variable called NAME and gives it the value VALUE.
*
* Returns 1 if the value was added, or 0 if there was no more room.
*/

bool SetValue(char* name, double value, bool save)
{
	int  i;

	ClearVar(name);
	for(i = 0; i < SR_MAXVARS; i++)
	{
		if(! *Vars[i].name)
		{
			strcpy(Vars[i].name, name);
			Vars[i].name[SR_VARLEN] = 0;
			Vars[i].value = value;
			return true;
		}
	}
	return false;
}
/**
*/


int isUTF8(char *p)
{
  unsigned char *up = (unsigned char *)p;
  if ((up[0]>=0x80) && (up[1]==0x80))
    return 2;
  return 0;
}



void SRExpressionsParser::parse(void)
{

	type = VT_UNKNOWN;
	token.reset();
	while(isspace(*expression))
		expression++;
	if(isDelim(*expression))
	{
		// parse for valid ops

		type = VT_DEL;
		char op=*expression;
        token.add(*expression++);
		switch(op)
		{
		case '|':
			if (*expression == '|')
                token.add(*expression++);
			break;
		case '*':
			if (*expression == '*')
                token.add(*expression++);// power...
			break;
		case '&':
			if (*expression == '&')
                token.add(*expression++);
			break;
		case '!':
			if (*expression == '=')
                token.add(*expression++);
			break;
		case '=':
			if (*expression == '=')
                token.add(*expression++);
			break;
		case '<':
			if (*expression == '=')
                token.add(*expression++);
			if (*expression == '<')
                token.add(*expression++);
			break;
		case '>':
			if (*expression == '=')
                token.add(*expression++);
			if (*expression == '>')
                token.add(*expression++);
			break;

		}
	}
	else if((expression[0]=='0') && (expression[1]=='x')) // hex value
	{
		type = VT_NUM;
        token.add(*expression++);
        token.add(*expression++);
		while(isxdigit(*expression))
		{
			token.add(*expression++);
		}
	}
	else
		if(isNumeric(*expression))
		{
			type = VT_NUM;
			while(isNumeric(*expression) || (*expression=='e')) // floating point, scientific
			{
				if (*expression=='e')
				{
                    token.add(*expression++);
					if (*expression=='-')
                        token.add(*expression++);
				}
				else
				{
                    token.add(*expression++);
				}
			}
			switch(*expression)
			{
			case 'o':
			case 'b':
			case 'd':
                    token.add(*expression++);
				break;
			}
		}
		else if(isAlpha(*expression) || ((unsigned char)*expression>=0x80))
		{
			type = VT_VAR;
			while(isAlpha(*expression) || ((unsigned char)*expression>=0x80))
            {
                token.add(*expression++);
			}
		}
		else if(*expression)
		{
			token.add(*expression++);
			throw(E_SYNTAX);
		}
		if((*expression==':')&&(expression[1]=='='))
		{
			expression++;
			type = VT_DEF;
		}
		else if((*expression=='=') && (expression[1]!='='))
		{
			type = VT_VAR;
		}

		while(isspace(*expression))
			expression++;
}


/**
* Assign(double* r)
* This function handles any variable assignment operations.
* It returns a value of true if it is a top-level assignment operation,  
* otherwise it returns false
*/

bool SRExpressionsParser::assign(double* r)
{
	char t[SR_VARLEN + 1];

	if (type == VT_VAR)
	{
		if((*expression == '=')&&(expression[1] != '='))
		{
			strcpy(t, token.value());
			parse();
			parse();
			if(!*token.value())
			{
				ClearVar(t);
				return true;
			}
			ternary(r);
			if(!SetValue(t, *r, true))
				throw(E_MAXVARS);
			return true;
		}
	}
	if (type == VT_DEF)
	{
		if((*expression == '=')&&(expression[1] != '='))
		{
			strcpy(t, token.value());
			parse();
			parse();
			if(!*token.value())
			{
				ClearVar(t);
				return true;
			}
			ternary(r);
			if(!SetValue(t, *r, true))
				throw(E_MAXVARS);
			return true;
		}
	}
	ternary(r);
	return false;
}

// http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B


void SRExpressionsParser::ternary(double* r)
{
	double t = 0;
	Token op;
	int selected=0;
	int count=0;

	logicalOr(r);

	op = token;
	while ((!strcmp(op.value(),"#"))||(!strcmp(op.value(),"?"))||(!strcmp(op.value(),":")))
	{
		if (count==0)
		{
			if (!strcmp(op.value(),"#"))
			{
				selected=1+(int)(*r);
				*r=0.0;
			}
			else
				selected=2-(int)(*r);
		}
		count++;
		parse();
		logicalOr(&t);
		if ((!strcmp(op.value(),"#"))||(!strcmp(op.value(),"?"))||(!strcmp(op.value(),":")))
		{
			if (selected==count)
				*r = t;
		}
		op = token;
	}
}

void SRExpressionsParser::logicalOr(double* r)
{
	double t = 0;
	Token op;

	bitwiseOr(r);

	op = token;
	while((!strcmp(op.value(),"||")))
	{
		parse();
		bitwiseOr(&t);
		if (!strcmp(op.value(),"||"))
			*r = (int)*r || (int)t;
	}
}

void SRExpressionsParser::logicalAnd(double* r)
{
	double t = 0;
	Token op;

	bitwiseOr(r);

	op=token;
	while((!strcmp(op.value(),"&&")))
	{
		parse();
		bitwiseOr(&t);
		if (!strcmp(op.value(),"&&"))
			*r = (int)*r && (int)t;
	}
}


void SRExpressionsParser::bitwiseOr(double* r)
{
	double t = 0;
	char o;

	bitwiseXor(r);
	while((o = *(token.value())) == '|')
	{
		parse();
		bitwiseXor(&t);
		if(o == '|')
			*r = (int)*r | (int)t;
	}
}

void SRExpressionsParser::bitwiseXor(double* r)
{
	double t = 0;
	char o;

	bitwiseAnd(r);
	while((o = *(token.value())) == '~')
	{
		parse();
		bitwiseAnd(&t);
		if(o == '~')
			*r = (int)*r && (int)t;
	}
}
void SRExpressionsParser::bitwiseAnd(double* r)
{
	double t = 0;
	char o;

	equalUnequalTo(r);
	while((o = *(token.value())) == '&')
	{
		parse();
		equalUnequalTo(&t);
		if(o == '&')
			*r = (int)*r && (int)t;
	}
}

void SRExpressionsParser::equalUnequalTo(double* r)
{
	double t = 0;
	Token op;

	compare(r);
	op = token;
	while((!strcmp(op.value(),"!="))|| (!strcmp(op.value(),"=="))|| (!strcmp(op.value(),"<>")))
	{
		parse();
		compare(&t);
		if (!strcmp(op.value(),"!="))
		{
			*r = *r != t;
		}
		if (!strcmp(op.value(),"=="))
		{
			*r = *r == t;
		}
		if (!strcmp(op.value(),"<>"))
		{
			*r = *r != t;
		}
	}
}

void SRExpressionsParser::compare(double* r)
{
	double t = 0;
	Token op = token;

	bitwiseShift(r);
	op = token;
	while( (!strcmp(op.value(),"<")) || (!strcmp(op.value(),">"))|| (!strcmp(op.value(),">="))|| (!strcmp(op.value(),"<=")))
	{
		parse();
		bitwiseShift(&t);
		if (!strcmp(op.value(),"<"))
		{
			*r = (double)(*r < t);
		}
		if (!strcmp(op.value(),">"))
		{
			*r = (double)(*r > t);
		}
		if (!strcmp(op.value(),">="))
		{
			*r = (double)(*r >= t);
		}
		if (!strcmp(op.value(),"<="))
		{
			*r = (double)(*r <= t);
		}

		op=token;
	}
}


void SRExpressionsParser::bitwiseShift(double* r)
{
	double t = 0;
	Token op;

	addSub(r);
	op = token;
	while( (!strcmp(op.value(),"<<")) || (!strcmp(op.value(),">>")))
	{
		parse();
		addSub(&t);
		if (!strcmp(op.value(),"<<"))
		{
			*r = (double)((long long)*r << (long long)t);
		}
		if (!strcmp(op.value(),">>"))
		{
			*r = (double)((long long)*r >> (long long)t);
		}
	}
}


void SRExpressionsParser::addSub(double* r)
{
	double t = 0;
	char o;

	mulDiv(r);
	while((o = *(token.value())) == '+' || o == '-')
	{
		parse();
		mulDiv(&t);
		if(o == '+')
			*r = *r + t;
		else if(o == '-')
			*r = *r - t;
	}
}


void SRExpressionsParser::mulDiv(double* r)
{
	double t;
	char op;

	pow(r);
	while((op = *(token.value())) == '*' || op == '/' || op == '%')
	{
		parse();
		pow(&t);
		if(op == '*')
			*r = *r * t;
		else if(op == '/')
		{
			if(t == 0)
				throw(E_DIVZERO);
			*r = *r / t;
		}
		else if(op == '%')
		{
			if(t == 0)
				throw(E_DIVZERO);
			*r = fmodl(*r, t);
		}
	}
}

void SRExpressionsParser::pow(double* r)
{
	double t;

	unary(r);
	while(*(token.value()) == '^')
	{
		parse();
		unary(&t);
		*r = (double)powl((double)*r, (double)t);
	}
}

/** highest priority function
*/
void SRExpressionsParser::unary(double* r)
{
	char o = 0;

	if(*token.value() == '+' || *token.value() == '-'|| *token.value() == '!')
	{
		o = *token.value();
		parse();
	}
	varFunc(r);
	if(o == '-')
		*r = -*r;
	if(o == '!')
		*r = (*r!=0.0)?0.0:1.0;
}


/**
* double getNumval(char *t)
* This function parses the string t and returns the value. 
* It is based on various semantics of data input:
*
* 0d... --> time values (d[/.-]m[/.-]y H:M:S) // y maybe 70-.. or >1970, h in 24 (note: NOT mm/dd/yyyy)
* 0x... --> hex values

* extension to do
* 0i... --> inverse hex (intel notation)

* ...b --> binary input
* ...o --> octal input
* all others: --> floating point
*/
double getNumval(char *t)
{
	double val=(double)0.0;
	long long lval = 0;
	if ((t[0] == '0') && (t[1] == 'x'))
	{
		sscanf(t+2,"%llx", &lval);
		val = (double) lval;
		return val;
	}	
	if (t[strlen(t)-1] == 'o')
	{
		sscanf(t,"%llo", &lval);
		val = (double) lval;
		return val;
	}
	if (t[strlen(t)-1] == 'b')
	{
		lval = 0;
		while (t[0] != 'b')
		{
			if (*t=='1')
				lval = (lval <<1)+1;
			else
				lval = (lval <<1);
			t++;
		}
		val = (double) lval;
		return val;
	}
  
  sscanf(t,"%lf",&val);
  return val;
  //	return (double) atof(t);
}

/**                                                                       
* VarFunc(double* r)   Internal use only                                 
*                                                                       
* This function evals any literal numbers, variables, or functions.   
*                                                                       
*/

void SRExpressionsParser::varFunc(double* r)
{
	int  i;
	int  n;
	double a[256];

	if(*token.value() == '(')
	{
		parse();
		if(*token.value() == ')')
			throw(E_NOARGS);
		assign(r);
		if(*token.value() != ')')
			throw(E_UNBALANCED);
		parse();
	}
	else
	{
		if(type == VT_NUM)
		{
			*r = (double) getNumval(token.value());
			parse();
		}
		else if(type == VT_VAR)
		{
			if(*expression == '(')
			{
				for(i = 0; func1p[i].args; i++)
				{
					if(!strcmp(token.value(), func1p[i].name))
					{
						parse();
						n = 0;
						do
						{
							parse();
							if(*token.value() == ')' || *token.value() == ',')
								throw(E_NOARGS);
							a[n] = 0;
							assign(&a[n]);
							n++;
						} while(n < 4 && *token.value() == ',');
						parse();
						if(n != func1p[i].args)
						{
							strcpy(token.value(), func1p[i].name);
							throw(E_NUMARGS);
						}
						// *r = func2p[i].func(a[0], a[1]);
						*r = func1p[i].func(a[0]);
						return;
					}
				}
				for(i = 0; func2p[i].args; i++)
				{
					if(!strcmp(token.value(), func2p[i].name))
					{
						parse();
						n = 0;
						do
						{
							parse();
							if(*token.value() == ')' || *token.value() == ',')
								throw(E_NOARGS);
							a[n] = 0;
							assign(&a[n]);
							n++;
						} while(n < 4 && *token.value() == ',');
						parse();
						if(n != func2p[i].args)
						{
							strcpy(token.value(), func2p[i].name);
							throw(E_NUMARGS);
						}
						// *r = func2p[i].func(a[0], a[1]);
						*r = func2p[i].func(a[0],a[1]);
						return;
					}
				}
				for(i = 0; funcnp[i].func; i++)
				{
					if(!strcmp(token.value(), funcnp[i].name))
					{
						parse();
						n = 0;
						do
						{
							parse();
							if(*token.value() == ')' || *token.value() == ',')
								throw(E_NOARGS);
							a[n] = 0;
							assign(&a[n]);
							n++;
						} while(n < sizeof(a)/sizeof(double) && *token.value() == ',');
						parse();
						// *r = func2p[i].func(a[0], a[1]);
						*r = funcnp[i].func(n,a);
						return;
					}
				}
				throw(E_BADFUNC); // functions 
			}
			else if(! getValue(token.value(), r))
				throw(E_UNKNOWN);
			parse();
		}
		else
			throw(E_SYNTAX);
	}
}


/**
* Evaluate(char* e, double* result, int* a)                            
* This function is called to evaluate the expression E and return the   
* answer in RESULT.  If the expression was a top-level assignment, a    
* value of 1 will be returned in A, otherwise it will contain 0.        
*                                                                       
* Returns E_OK if the expression is valid, or an error code.           
*                                                                       
*/
EVALEXPR_RESULT SRExpressionsParser::evaluate(const char* e, double* result, int* a)
{
	try
	{
		expression = e;
		errAnc = e;
		do
		{
			*result = 0;
			parse();
			if(!*token.value())
				throw(E_EMPTY);
			*a = assign(result);
			if (*token.value()==',')
			{
				errAnc=token.value()+1;
			}
		}while (*token.value()==',');
	}
	catch(EVALEXPR_RESULT expError)
	{
		this->expError=expError;
		errBufPos=(long)(expression-errAnc-1); 
		strcpy(errToken,token.value());
		return expError;
	}
	return E_OK;
}
