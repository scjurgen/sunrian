//
//  Created by Schwietering, Jürgen on 27.12.13.
//  Copyright (c) 2013 Schwietering, Jürgen. All rights reserved.
//

#ifndef SRErrorCode_include_h
#define SRErrorCode_include_h

/** Codes returned from the evaluator */

typedef enum {
	E_OK,         //!Successful evaluation
	E_SYNTAX,     //!Syntax error
	E_UNBALANCED, //!Unbalanced parenthesis
	E_DIVZERO,    //!Attempted division by zero
	E_UNKNOWN,    //!Reference to unknown variable
	E_MAXVARS,    //!Maximum variables exceeded
	E_BADFUNC,    //!Unrecognised function
	E_NUMARGS,    //!Wrong number of arguments to function
	E_NOARGS,     //!Missing an argument to a function
	E_EMPTY,      //!Empty expression
} EVALEXPR_RESULT;

#endif