//
//  DoingController.m
//  Done
//
//  Created by benmaslen on 07/08/2009.
//  Copyright 2009 ortatherox.com. All rights reserved.
//

#import "DoingController.h"
#import "NSString_URLEncoding.h"


@implementation DoingController

-(void)awakeFromNib{
  // If details are missing ask for them
  log([[NSUserDefaults standardUserDefaults] objectForKey:@"username"]);
  if([[NSUserDefaults standardUserDefaults] objectForKey:@"username"] == nil){
    #ifdef  MAC
    [sheetController performSelector:@selector(openSheet:)  withObject:self afterDelay:0.5];
    #endif

  }
  
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sent) name:@"ORMessageSent" object:nil];

}

- (IBAction)send:(id)sender{
  [[NSUserDefaults standardUserDefaults] synchronize];
  // If details are missing ask for them
  if( ([[NSUserDefaults standardUserDefaults] objectForKey:@"username"] == nil) ||
    ([[NSUserDefaults standardUserDefaults] objectForKey:@"password"] == nil) ||
    ([[NSUserDefaults standardUserDefaults] objectForKey:@"address"] == nil) ){
      #ifdef MAC
      [sheetController performSelector:@selector(openSheet:)  withObject:self afterDelay:0.5];
      #endif
    }
  
  NSString * username= [[NSUserDefaults standardUserDefaults] objectForKey:@"username"];
  NSString * password= [[NSUserDefaults standardUserDefaults] objectForKey:@"password"];
  NSString * address= [[NSUserDefaults standardUserDefaults] objectForKey:@"address"];
  NSString * url = [NSString stringWithFormat:@"%@?author=%@&message=%@", address, username, [[messageTextField stringValue] urlEncodeValue]];
  log(url);
  [messageController sendString:url withSender:self];
}

-(void) sent{
  [messageTextField setStringValue:@""];
}

- (BOOL)shouldCloseSheet:(id)sender{
  return true;
}


@end
