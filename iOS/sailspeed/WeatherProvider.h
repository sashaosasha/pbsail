//
//  WeatherProvider.h
//  sailspeed
//
//  Created by Alex Ognev on 5/20/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface WeatherProvider : NSObject

- (void) weatherUpdate:(CLLocation*)location;

@property float windSpeed;
@property float windGust;
@property int windAngle;

@end
