//
//  SRInternalDisk.h
//
//  Created by Jürgen Schwietering on 3/9/11.
//  Copyright (c) 2011 Jürgen Schwietering. All rights reserved.
//

@interface SRInternalDisk : NSObject

+ (unsigned long long)freeSpace;
+ (unsigned long long)totalSpace;

@end