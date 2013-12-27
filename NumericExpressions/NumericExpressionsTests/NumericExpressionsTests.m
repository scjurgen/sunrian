//
//  NumericExpressionsTests.m
//  NumericExpressionsTests
//
//  Created by Schwietering, Jürgen on 27.12.13.
//  Copyright (c) 2013 Schwietering, Jürgen. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "NumericExpressions.h"



@interface NumericExpressionsTests : XCTestCase
{
    NumericExpressions *expr;

}
@end

@implementation NumericExpressionsTests

- (void)setUp
{
    [super setUp];
    expr=[[NumericExpressions alloc] init];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testSimpleExpressions
{
    XCTAssertEqualWithAccuracy( 5.0,  [expr evaluate:@" 1 + 4" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 5.0,  [expr evaluate:@"1+4" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(-3.0,  [expr evaluate:@"1-4" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(20.0,  [expr evaluate:@"(2+3)*4" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 8.0,  [expr evaluate:@"(2-4)*-4" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(1/7.0, [expr evaluate:@"1/7" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(32.0,  [expr evaluate:@"2^5" ], 0.0, @"error evaluation");
}


- (void)testNestedExpressions
{
    XCTAssertEqualWithAccuracy(19.0,  [expr evaluate:@"((5+3)*4+6)*0.5" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(15.0,  [expr evaluate:@"(1+(2+(3+(4+5))))" ], 0.0, @"error evaluation");
}

- (void)testNumbers
{
    XCTAssertEqualWithAccuracy( 1.2345,  [expr evaluate:@"1.2345" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 15.0,  [expr evaluate:@"0xf" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 31,  [expr evaluate:@"10101b+1010b" ], 0.0, @"error evaluation");

}

- (void)testVariables
{
    NumericExpressions *exprWithA=[[NumericExpressions alloc] init];
    [exprWithA evaluate:@"a=3-1"];
    XCTAssertEqualWithAccuracy(5.0,   [expr evaluate:@"a+a+1" ], 0.0, @"error evaluation");
    [exprWithA evaluate:@"a=a*2"];
    XCTAssertEqualWithAccuracy(9.0,   [expr evaluate:@"a+a+1" ], 0.0, @"error evaluation");
}

- (void)testFunctions
{
    XCTAssertEqualWithAccuracy(M_PI_4,[expr evaluate:@"atan(1)" ],  0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 4.0,  [expr evaluate:@"log(16)/log(2)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 4.0,  [expr evaluate:@"log10(10000.0)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 2.0,  [expr evaluate:@"sqrt(2.0)^2" ], 1.0E-12, @"error evaluation");

    XCTAssertNotEqualWithAccuracy(0.0,  [expr evaluate:@"rand - rand" ], 0.0, @"error evaluation"); // this COULD technically fail ;)
}

- (void)testIntegerFunctions
{
    XCTAssertEqualWithAccuracy( 4.0,  [expr evaluate:@"ceil(pi)" ],0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 3.0,  [expr evaluate:@"floor(pi)" ],0.0, @"error evaluation");
}
- (void)testBooleanFunctions
{
    XCTAssertEqualWithAccuracy( 5.0,  [expr evaluate:@"and(0xf,0x5)" ],0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(10.0,  [expr evaluate:@"xor(0xf,0x5)" ],0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(15.0,  [expr evaluate:@"or(0x5,0xa)" ],0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(0.0,   [expr evaluate:@"gt(4,6)" ],0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(1.0,   [expr evaluate:@"le(4,6)" ],0.0, @"error evaluation");
}

- (void)testTrigonometricFunctions
{
    XCTAssertEqualWithAccuracy(   5.0,[expr evaluate:@"hypot(3,4)" ],  0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(  45.0,[expr evaluate:@"deg(angle(3,3))" ],  0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(- 45.0,[expr evaluate:@"deg(angle(-3,3))" ],  0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(-150.0,[expr evaluate:@"deg(angle(-1,-sqrt(3.0)))" ],  0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(   1.0,[expr evaluate:@"sin(pi/2.0)" ],  1.0E-12, @"error evaluation");
}

- (void)testNArgumentFunctions
{
    XCTAssertEqualWithAccuracy(17.5,  [expr evaluate:@"avg(10,10,20,30)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(-7.0,  [expr evaluate:@"min(-5,0,-7,8)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 8.0,  [expr evaluate:@"max(-5,0,-7,8)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(pow(2*4*6*8*20,1.0/5.0),  [expr evaluate:@"geo(2,4,6,8,20)" ], 1.0E-12, @"error evaluation");
}

- (void)testFibonacciFunction
{
    XCTAssertEqualWithAccuracy( 0.0,  [expr evaluate:@"fib(0)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 1.0,  [expr evaluate:@"fib(1)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 1.0,  [expr evaluate:@"fib(2)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 2.0,  [expr evaluate:@"fib(3)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 3.0,  [expr evaluate:@"fib(4)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 5.0,  [expr evaluate:@"fib(5)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy( 8.0,  [expr evaluate:@"fib(6)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(13.0,  [expr evaluate:@"fib(7)" ], 0.0, @"error evaluation");
    XCTAssertEqualWithAccuracy(39088169.0,  [expr evaluate:@"fib(38)" ], 0.0, @"error evaluation");
}





/*
{ "avg",  avgn},
{ "geo",  geon},
{ "max",  maxn},
{ "min",  minn},

{ "rand",   equirand},
{ "rnd",    equirand},
{ "drnd",   drand},// density random
{ "grnd",   gaussRandom},// gaussian random

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
*/


@end

