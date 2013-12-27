//
//  SRInternalDisk.m
//
//  Created by Jürgen Schwietering on 3/9/11.
//  Copyright (c) 2011 Jürgen Schwietering. All rights reserved.
//

#import "SRInternalDisk.h"


@implementation SRInternalDisk

+ (unsigned long long)freeSpace
{
    NSError *error = nil;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    if (paths == nil)
        return 0L;
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject]
                                                                                       error: &error];
    if (dictionary)
    {
        // NSFileSystemSize
        NSNumber *freeFileSystemSizeInBytes = dictionary[NSFileSystemFreeSize];
        return [freeFileSystemSizeInBytes unsignedLongLongValue];
    } else
    {
        NSLog(@"Error while obtaining system memory info: domain: '%@', code: '%xh'", [error domain], [error code]);
        return 0L;
    }
    return 0L;
}


+ (unsigned long long)totalSpace
{
    NSError *error = nil;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    if (paths == nil)
        return 0L;
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject]
                                                                                       error: &error];
    if (dictionary)
    {
        NSNumber *fileSystemSizeInBytes = dictionary[NSFileSystemSize];
        return [fileSystemSizeInBytes unsignedLongLongValue];
    } else
    {
        NSLog(@"Error while obtaining system memory info: domain: '%@', code: '%xh'", [error domain], [error code]);
        return 0L;
    }
    return 0L;
}
@end
