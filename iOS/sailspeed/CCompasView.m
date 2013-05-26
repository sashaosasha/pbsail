//
//  CCompasView.m
//  sailspeed
//
//  Created by Alex Ognev on 5/20/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import "CCompasView.h"

@implementation CCompasView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    [super setOpaque:NO];
     return self;
}

- (void)drawRect:(CGRect)rect
{
    static const int half_width = 3;
    static const int arrow_half_width = 6;
    static const int arrow_length = 8;
    float r = fmin(rect.size.width, rect.size.height) * 0.33;
    float points[][2] = {
        {-half_width, -r}, {-half_width, r-arrow_length},  // One side
        {-arrow_half_width, r-arrow_length}, {0, r}, {arrow_half_width, r-arrow_length}, // Arrow tip
        {half_width, r-8}, {half_width, -r}, // Other side
        {-half_width, -r} // back
    };
    
    CGContextRef theContext = UIGraphicsGetCurrentContext();
    CGMutablePathRef path = CGPathCreateMutable();
    
    float bearing = self.compassBearing;
    float mid_x = rect.origin.x + rect.size.width / 2;
    float mid_y = rect.origin.y + rect.size.height / 2;
    float ax = cos((bearing - 90.0) * M_PI / 180.0);
    float ay = sin((bearing - 90.0) * M_PI / 180.0);
    
    CGPathMoveToPoint(path, NULL, mid_x + ax*points[0][1]-ay*points[0][0], mid_y + ay*points[0][1] + ax*points[0][0]);
    for (int i = 1; i < sizeof(points)/sizeof(points[0]); ++i)
    {
        CGPathAddLineToPoint(path, NULL, mid_x + ax*points[i][1]-ay*points[i][0], mid_y + ay*points[i][1] + ax*points[i][0]);
    }
    
    CGContextSetLineWidth(theContext, 2.0);
    CGContextSetStrokeColorWithColor(theContext, [UIColor redColor].CGColor);
    CGContextSetFillColorWithColor(theContext, [UIColor blackColor].CGColor);
    CGContextAddPath(theContext, path);
    CGContextFillPath(theContext);
    CGContextStrokePath(theContext);
    
    CGPathRelease(path);
}



@end
