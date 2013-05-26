//
//  WeatherProvider.m
//  sailspeed
//
//  Created by Alex Ognev on 5/20/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import "WeatherProvider.h"

@implementation WeatherProvider

- (void) weatherUpdate:(CLLocation*)location {
    // Fetch weather at current location using openweathermap.org's JSON API:
    CLLocationCoordinate2D coordinate = location.coordinate;
    NSString *apiURLString = [NSString stringWithFormat:@"http://api.openweathermap.org/data/2.1/find/city?lat=%f&lon=%f&cnt=1", coordinate.latitude, coordinate.longitude];
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:apiURLString]];
    NSOperationQueue *queue = [[NSOperationQueue alloc] init];
    [NSURLConnection sendAsynchronousRequest:request queue:queue completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
        NSHTTPURLResponse *httpResponse = nil;
        if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
            httpResponse = (NSHTTPURLResponse *) response;
        }
        
        // Check for error or non-OK statusCode:
        if (error || httpResponse.statusCode != 200) {
            [[[UIAlertView alloc] initWithTitle:nil message:@"Error fetching weather" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
            return;
        }
        
        // Parse the JSON response:
        NSError *jsonError = nil;
        NSDictionary *root = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonError];
        @try {
            if (jsonError == nil && root) {
                // TODO: type checking / validation, this is really dangerous...
                NSDictionary *firstListItem = [root[@"list"] objectAtIndex:0];
                NSDictionary *main = firstListItem[@"main"];
                NSDictionary *wind = firstListItem[@"wind"];
                
                // Get the temperature:
                //NSNumber *temperatureNumber = main[@"temp"]; // in degrees Kelvin
                //int temperature = [temperatureNumber integerValue] - 273.15;
                
                self.windSpeed = [wind[@"speed"] floatValue];
                self.windGust = [wind[@"gust"] floatValue];
                self.windAngle = [wind[@"deg"] integerValue];
                return;
            }
        }
        @catch (NSException *exception) {
        }
        [[[UIAlertView alloc] initWithTitle:nil message:@"Error parsing response" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
        }];
}

@end
