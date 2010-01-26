//
//  DoingController.h
//  Done
//
//  Created by benmaslen on 07/08/2009.
//  Copyright 2009 ortatherox.com. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MessageController.h"

#ifdef  MAC
#import <BWToolkitFramework/BWSheetController.h>
#endif

@interface DoingController : NSObject {
  IBOutlet NSTextField  *messageTextField;
  IBOutlet MessageController *messageController;
  
  IBOutlet NSTextField  *usernameTextField;
  IBOutlet NSTextField  *passwordTextField;
  IBOutlet NSTextField  *serverTextField;
  
#ifdef  MAC
  IBOutlet BWSheetController * sheetController;
#endif
  
}

- (IBAction) send:(id)sender;
- (void) sent;
- (BOOL)shouldCloseSheet:(id)sender;


@end
