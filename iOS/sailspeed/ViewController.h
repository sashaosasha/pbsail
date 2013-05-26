//
//  ViewController.h
//  sailspeed
//
//  Created by Alex Ognev on 5/20/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import "WeatherProvider.h"
#import "CCompasView.h"
#import "PebbleGateway.h"

WeatherProvider *weatherSource;
CLLocationSpeed speed;
NSTimer *timer;
PebbleGateway* pebble;

NSString *rest_name;

@protocol CoreLocationControllerDelegate
@required
- (void)locationUpdate:(CLLocation *)location; // Our location updates are sent here
- (void)locationError:(NSError *)error; // Any errors are sent here
@end

@interface ViewController : UIViewController<CLLocationManagerDelegate, PBGatewayDelegate>

@property (weak, nonatomic) IBOutlet UILabel *statusLabel;
@property (nonatomic, retain) CLLocationManager *locManager;
@property (nonatomic, assign) id delegate;

@property (weak, nonatomic) IBOutlet UILabel *speedLabel;
@property (weak, nonatomic) IBOutlet UILabel *speedLabel2;
@property (retain) NSTimer *timer;

- (IBAction)didStart:(id)sender;
@property (weak, nonatomic) IBOutlet UIButton *startButton;

- (IBAction)didStop:(id)sender;
@property (weak, nonatomic) IBOutlet UIButton *stopButton;

@property (weak, nonatomic) IBOutlet UIImageView *arrowImage;
@property (weak, nonatomic) IBOutlet UIImageView *dialImage;
@property (weak, nonatomic) IBOutlet UILabel *weatherLabel;
@end

