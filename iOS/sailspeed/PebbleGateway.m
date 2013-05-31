//
//  PebbleGateway.m
//  sailspeed
//
//  Created by Alex Ognev on 5/23/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import "PebbleGateway.h"
#import <PebbleKit/PebbleKit.h>

@implementation PebbleGateway {
    PBWatch *_targetWatch;
}

- (id)init {
    // We'd like to get called when Pebbles connect and disconnect, so become the delegate of PBPebbleCentral:
    [[PBPebbleCentral defaultCentral] setDelegate:self];
    
    // Initialize with the last connected watch:
    [self setTargetWatch:[[PBPebbleCentral defaultCentral] lastConnectedWatch]];
    self.delegate = nil;
    return self;
}

- (void) sendUpdate:(NSNumber*)speed
          windSpeed:(NSNumber*)wind
           windGust:(NSNumber*)gust
            windDir:(NSNumber*)dir
              units:(NSString*)units
{
    double v = [speed floatValue];
    
    if (_targetWatch == nil || [_targetWatch isConnected] == NO) {
        if (self.delegate != nil) {
            [self.delegate noWatchConnection ];
        }
        return;
    }
    
    NSNumber *speedKey = @(0);
    NSNumber *windKey = @(1);
    NSDictionary *update = @{ speedKey:[NSString stringWithFormat:@"Speed %.1f %@", v, units],
                              windKey:[NSString stringWithFormat:@"Wind %.0f(%.0f) from %d°", [wind floatValue], [gust floatValue], [dir intValue]]};
    [_targetWatch appMessagesPushUpdate:update onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
        if(self.delegate != nil) {
            NSString *message = error ? [error localizedDescription] : @"Update sent!";
            [self.delegate onError:message];
        }
    }];
    return;

}

- (void)setTargetWatch:(PBWatch*)watch {
    _targetWatch = watch;
    
    // Test if the Pebble's firmware supports AppMessages / Weather:
    [watch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        if (isAppMessagesSupported) {

            uint8_t bytes[] = { 0xED, 0xB5, 0x02, 0xF4, 0xEC, 0xB4, 0x4C, 0x6E, 0x9E, 0xD6, 0x80, 0x71, 0x8C, 0x28, 0xC7, 0x98 };
            NSData *uuid = [NSData dataWithBytes:bytes length:sizeof(bytes)];
            [watch appMessagesSetUUID:uuid];
            [watch appMessagesLaunch:^(PBWatch *watch, NSError *error) {
                if(self.delegate != nil) {
                    NSString *message = error ? [error localizedDescription] : @"Update sent!";
                    [self.delegate onError:message];
                }
            }];
            if (self.delegate != nil) {
                [self.delegate didWatchConnect ];
            }

        } else {
            
            if (self.delegate != nil) {
                [self.delegate noWatchConnection];
            }
        }
    }];
}

/*
 *  PBPebbleCentral delegate methods
 */

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidConnect:(PBWatch*)watch isNew:(BOOL)isNew {
    [self setTargetWatch:watch];
}

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidDisconnect:(PBWatch*)watch {
    if (self.delegate != nil) {
        [self.delegate noWatchConnection ];
    }
    if (_targetWatch == watch || [watch isEqual:_targetWatch]) {
        [self setTargetWatch:nil];
    }
}

@end
