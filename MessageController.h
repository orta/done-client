//
//  MessageController.h
//  Done
//
//  Created by benmaslen on 07/08/2009.
//  Copyright 2009 ortatherox.com. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MessageController : NSObject {
	NSMutableData *responseData;
	NSURL *baseURL;
  IBOutlet NSButton* sendButton;
  IBOutlet NSProgressIndicator* swirlThing;
  id lastSender;
}
-(void) sendString: (NSString*) string withSender:(id) sender;

@end
