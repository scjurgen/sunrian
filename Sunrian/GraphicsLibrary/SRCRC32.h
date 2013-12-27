//
//  CRC32StreamCalculator.h
//
//  Created by Jürgen Schwietering on 12/13/12.
//  Copyright (c) 2012 Jürgen Schwietering. All rights reserved.
//

@class SRCRC32;

@protocol SRCRC32Protocol

- (uint32_t)serializeWithCRC32:(SRCRC32 *)crc;
@end

@interface SRCRC32 : NSObject

@property (assign, atomic) uint32_t crcValue;

- (void)reset;
- (uint32_t)calculateSingle:(const unsigned char *)sData length:(NSUInteger)length;

- (void)calculatePartial:(const unsigned char *)sData length:(NSUInteger)length;
- (uint32_t)getCrcValue;

+ (uint32_t)calculate:(NSData *)data;

#pragma mark - pure c bridge
+ (uint32_t)cCalculateString:(const char *)sData;

@end
