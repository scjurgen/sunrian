//
//  SunrianTests.m
//  SunrianTests
//
//  Created by jay on 25/12/13.
//  Copyright (c) 2013 Jurgen Schwietering. All rights reserved.
//

#import <XCTest/XCTest.h>

@interface SunrianTests : XCTestCase

@end

@implementation SunrianTests


+ (double)compareImage:(UIImage*)refImage resImage:(UIImage*)resImage
{
    int width = refImage.size.width;
    int height = refImage.size.height;
    if (resImage.size.width != width)
        return 1.0;
    if (resImage.size.height != height)
        return 1.0;
    CGColorSpaceRef colourSpace = CGColorSpaceCreateDeviceRGB();
    
    const NSUInteger bytesPerPixel = 4;
    
    unsigned char *refMemoryPool = (unsigned char *)calloc(width*height*bytesPerPixel, 1);
    unsigned char *resMemoryPool = (unsigned char *)calloc(width*height*bytesPerPixel, 1);
    
    // draw images into the same kind of image context (colorSpace and BitmapOrder)
    CGContextRef refContext = CGBitmapContextCreate(refMemoryPool, width, height, 8, width * bytesPerPixel, colourSpace, kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast);
    CGContextRef resContext = CGBitmapContextCreate(resMemoryPool, width, height, 8, width * bytesPerPixel, colourSpace, kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colourSpace);
    
    CGContextDrawImage(refContext, CGRectMake(0, 0, width, height), [refImage CGImage]);
    CGContextDrawImage(resContext, CGRectMake(0, 0, width, height), [resImage CGImage]);
    double error=0.0;
    double maxError=0.0;
    for (int y = 0; y < height; y++)
    {
        unsigned char *refLinePointer = &refMemoryPool[y * width * bytesPerPixel];
        unsigned char *resLinePointer = &resMemoryPool[y * width * bytesPerPixel];
        
        for(int x = 0; x < width; x++)
        {
            for (int b=0; b < bytesPerPixel; b++)
                error += fabs(refLinePointer[b]-resLinePointer[b])/256.0; // maybe we should do convolusion
            refLinePointer += bytesPerPixel;
            resLinePointer += bytesPerPixel;
            maxError += bytesPerPixel;
        }
    }
    //NSLog(@"Error for image %d:%d e=%f max=%f d=%f",width,height,error,maxError,error/maxError);
    CGContextRelease(refContext);
    CGContextRelease(resContext);
    free(refMemoryPool);
    free(resMemoryPool);

    return error/maxError;
}


- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testTheTest
{
    double res;
    res = [SunrianTests compareImage:[UIImage imageNamed:@"images-7.jpg"] resImage:[UIImage imageNamed:@"images-7.jpg"]];
    XCTAssertEqualWithAccuracy(res, 0.0f, 1E-3, @"Images should be the same d=%f", res);
    res = [SunrianTests compareImage:[UIImage imageNamed:@"images-7.jpg"] resImage:[UIImage imageNamed:@"images-5.jpg"]];
    XCTAssertEqualWithAccuracy(res, 0.2631898341049383f, 1E-3, @"Images should be very different d=%f", res);
}

- (void)testGenerative
{
    double res;
    res = [SunrianTests compareImage:[UIImage imageNamed:@"images-1.jpg"] resImage:[UIImage imageNamed:@"images-2.jpg"]];
    XCTAssertEqualWithAccuracy(res, 0.0f, 1E-10, @"Images should be the same d=%f", res);
}


@end


