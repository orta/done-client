//
//  ApplicationController.h
//  Done
//
//  Created by benmaslen on 08/08/2009.
//  Copyright 2009 ortatherox.com. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ApplicationController : NSObject {
  NSTimer *mainTimer;
  IBOutlet NSWindow  *messageWindow;
  IBOutlet NSImageView  *connectionImageView;
	IBOutlet NSButton *buttonOpenAtLogin;
}

-(void) openWindow;
-(void) updateTimer;
-(void) messageSent;
- (IBAction)openSite:(id)sender;
- (bool)testURL: (NSString *) url;

@end
