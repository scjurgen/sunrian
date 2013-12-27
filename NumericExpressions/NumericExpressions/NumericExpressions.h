//
//  NumericExpressions.h
//  NumericExpressions
//
//  Created by Schwietering, Jürgen on 27.12.13.
//  Copyright (c) 2013 Schwietering, Jürgen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SRErrorCodes.h"

@interface NumericExpressions : NSObject

- (double)evaluate:(NSString *)expression;
- (double)evaluate:(NSString *)expression error:(EVALEXPR_RESULT*)error;

@end
