//
//  NumericExpressions.m
//  NumericExpressions
//
//  Created by Schwietering, Jürgen on 27.12.13.
//  Copyright (c) 2013 Schwietering, Jürgen. All rights reserved.
//

#import "NumericExpressions.h"
#import "SRExpressionsParser.h"

@interface NumericExpressions()
{
    SRExpressionsParser expressionParser;
}
@end

@implementation NumericExpressions


- (double)evaluate:(NSString*)expression
{
    return [self evaluate:expression error:nil];
}

- (double)evaluate:(NSString*)expression error:(EVALEXPR_RESULT *)error
{
    int a;
    double result;
    EVALEXPR_RESULT errorVal = expressionParser.evaluate([expression UTF8String], &result, &a);
    if (errorVal != E_OK)
    {
        NSLog(@"%@ %f: %d",expression,result,errorVal);
        return NAN;
    }
    if (error != nil)
    {
        *error=errorVal;
    }
    return result;
}

@end
