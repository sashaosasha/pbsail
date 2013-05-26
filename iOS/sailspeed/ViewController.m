//
//  ViewController.m
//  sailspeed
//
//  Created by Alex Ognev on 5/20/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import "ViewController.h"
#import "CCompasView.h"

#define kRequiredAccuracy 500.0 //meters
#define kMaxAge 10.0 //seconds
#define kMPHtoKnots 0.868976
#define kWeatherUpdateSeconds 20
#define kEMAAlpha 0.16

NSDate* lastWeatherRequest = nil;

@implementation ViewController {
    CCompasView* _compassView;
    float speedEma;
    float speed;
}

float degreesToRadians(float a)
{
    return a * 3.14 / 180;
}

- (void) decayEma
{
    self->speedEma = (1.0 - kEMAAlpha) * self->speedEma + kEMAAlpha * self->speed;
}

- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation {
    
    NSTimeInterval ageInSeconds = [newLocation.timestamp timeIntervalSinceNow];
    
    //ensure you have an accurate and non-cached reading
    if( newLocation.horizontalAccuracy <= kRequiredAccuracy && fabs(ageInSeconds) <= kMaxAge )
    {
        //get current speed
        self->speed = fmax(0.0, newLocation.speed);
        [self decayEma];
        [_speedLabel setText:[NSString stringWithFormat:@"%.1f mph, %.1f kts",
                              self->speed,
                              self->speed * kMPHtoKnots]];
        [_speedLabel2 setText:[NSString stringWithFormat:@"%.1f mph, %.1f kts",
                              self->speedEma,
                              self->speedEma * kMPHtoKnots]];
    
    
        float requestAge = fabs([lastWeatherRequest timeIntervalSinceNow]);
        if (lastWeatherRequest == nil || requestAge > kWeatherUpdateSeconds) {
            // set weather
            [weatherSource weatherUpdate:newLocation];
            NSString* weather = [NSString stringWithFormat:@"Wind %2.0f, Gust: %2.0f @%d",
                                 weatherSource.windSpeed * kMPHtoKnots,
                                 weatherSource.windGust * kMPHtoKnots,
                                 weatherSource.windAngle];
            [_weatherLabel setText:weather];
            
            lastWeatherRequest = [NSDate date];
            
            
            float angle = degreesToRadians(weatherSource.windAngle - 90);
            float w = self.arrowImage.bounds.size.width;
            float h = self.arrowImage.bounds.size.height;
            
     //       CGAffineTransform where = CGAffineTransformMakeRotation(angle);
            CGAffineTransform where  = CGAffineTransformConcat(
                                                CGAffineTransformConcat(
                                                               CGAffineTransformMakeTranslation(-w/2, -h/2),
                                                               CGAffineTransformMakeRotation(angle)),
                                                CGAffineTransformMakeTranslation(w/2,h/2));
            [self.arrowImage setTransform:where];
            self->_compassView.compassBearing = weatherSource.windAngle;
            [self->_compassView setNeedsDisplay];
        }
        
        [pebble sendUpdate:[NSNumber numberWithDouble:self->speedEma]
                 windSpeed:[NSNumber numberWithDouble:weatherSource.windSpeed * kMPHtoKnots]
                  windGust:[NSNumber numberWithDouble:weatherSource.windGust * kMPHtoKnots]
                   windDir:[NSNumber numberWithInt32:weatherSource.windAngle]];
    }
}

-(void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error {
    [_speedLabel setText: @"ERRROR!"];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    self.locManager = [[CLLocationManager alloc] init]; // Create new instance of locMgr
    self.locManager.delegate = self; // Set the delegate as self.
    
    [_stopButton setEnabled:false];
    [_stopButton setAlpha:0.5];
    
    weatherSource = [WeatherProvider alloc];
    pebble = [[PebbleGateway alloc] init];
    pebble.delegate = self;
    
    self->_compassView = [[CCompasView alloc] initWithFrame:[self.dialImage bounds]];
    [self.dialImage addSubview:self->_compassView];
    [self.arrowImage setHidden:YES];
    
    self->speedEma = 0.0;
    self->speed = 0.0;
    
    [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(decayEma) userInfo:nil repeats:YES];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)didStart:(id)sender {
    [self.locManager startUpdatingLocation];
    [_startButton setEnabled:false];
    [_startButton setAlpha:0.5];
    [_stopButton setEnabled:true];
    [_stopButton setAlpha:1.0];
}
- (IBAction)didStop:(id)sender {
    [self.locManager stopUpdatingLocation];
    [_startButton setEnabled:true];
    [_startButton setAlpha:1.0];
    [_stopButton setEnabled:false];
    [_stopButton setAlpha:0.5];
}

- (void) noWatchConnection{
    [self.statusLabel setText:@"No watch connection...."];

}
- (void) didWatchConnect{
    [self.statusLabel setText:@"connected to pebble."];
}

- (void) onError:(NSString*)message{
    [self.statusLabel setText:message];
}

@end
