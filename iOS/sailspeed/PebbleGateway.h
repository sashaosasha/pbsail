//
//  PebbleGateway.h
//  sailspeed
//
//  Created by Alex Ognev on 5/23/13.
//  Copyright (c) 2013 Alex Ognev. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <PebbleKit/PebbleKit.h>

@protocol PBGatewayDelegate
@required
- (void) noWatchConnection;
- (void) didWatchConnect;
- (void) onError:(NSString*)message;
@end

@interface PebbleGateway : NSObject<PBPebbleCentralDelegate>

- (void) sendUpdate:(NSNumber*)speed
          windSpeed:(NSNumber*)wind
           windGust:(NSNumber*)gust
            windDir:(NSNumber*)dir
              units:(NSString*)units;

@property (retain) id delegate;

@end
