//
//  ApplicationController.m
//  Done
//
//  Created by benmaslen on 08/08/2009.
//  Copyright 2009 ortatherox.com. All rights reserved.
//

#import "ApplicationController.h"
#define WORK 0

#define HIDE 0
#define CLOSE 1
#define NOTHING 2

@implementation ApplicationController

-(void) awakeFromNib{
  [self updateTimer];
  [messageWindow setReleasedWhenClosed:YES] ;
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(messageSent) name:@"ORMessageSent" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationDidBecomeActive:) name:NSApplicationDidBecomeActiveNotification object:NSApp];
}

- (void)windowDidResignMain:(NSNotification *)notification {
  [self updateTimer];
  [messageWindow miniaturize:self];

}

- (BOOL)windowShouldClose:(id)sender{
  [messageWindow miniaturize:self];
  return NO;
}


-(void) updateTimer{  
  
  if(mainTimer != NULL){
    log(@"invalidating old timer");
    [mainTimer invalidate];
  }
  int minutes = 60;
  int time = minutes * 15;
  NSNumber * interval= [[NSUserDefaults standardUserDefaults] objectForKey:@"interval"] ;
  if(interval){
    time += ([interval intValue] * 60 * 15);
  }
  log(@"waiting %i seconds", time);

  mainTimer = [NSTimer timerWithTimeInterval:time
                                      target:self
                                    selector:@selector(openWindow)
                                    userInfo:nil
                                     repeats:YES];
  [[NSRunLoop currentRunLoop] addTimer:mainTimer forMode:NSDefaultRunLoopMode];
  
}


-(void) openWindow {
  
#ifdef  MAC
  NSDate *now = [NSDate date];
  NSCalendar *calendar = [NSCalendar currentCalendar];
  NSDateComponents *components = [calendar components:NSHourCalendarUnit fromDate:now];
  int hour =[components hour];
  log("time is %i o clock", hour);
  //NSNumber* timezone = [[NSUserDefaults standardUserDefaults] objectForKey:@"timezone"];
  if([timezone intValue] == WORK) { 
    if(hour < 9 && hour > 17){
      return;
    }
  }else{
    if(hour > 9 && hour < 17){
      return;
    }    
  }
#endif
  
  if([[[[NSWorkspace sharedWorkspace] activeApplication] objectForKey:@"NSApplicationName"] isEqual: @"Done"]){
      //Currently the frontmost App, no need to be annoying
    return;
  }


  NSURL *soundfileURL = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource: @"Hic2" ofType:@"wav"]]; 
  NSSound *sound = [[NSSound alloc] initWithContentsOfURL:soundfileURL byReference:NO];
  [sound play];
  [NSApp arrangeInFront:self];
  [messageWindow makeKeyAndOrderFront:self];
  [NSApp activateIgnoringOtherApps:YES];
}


- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
	[NSApp arrangeInFront:self];
	[messageWindow makeKeyAndOrderFront:self];
	[NSApp activateIgnoringOtherApps:YES];
}


-(void) messageSent {
  NSNumber * action = [[NSUserDefaults standardUserDefaults] objectForKey:@"actionAfterPosting"];
  switch ([action intValue] ) {
    case HIDE:
      [NSApp hide: self];
      break;
    case CLOSE:
      //  http://www.cocoadev.com/index.pl?TerminateOrStopGood
      [NSApp stop:self];
      if(NSApp && [NSApp isRunning])
        [NSApp stop:self];
      if(NSApp && [NSApp isRunning])
        [NSApp stop:self];
      break;
    default:
      break;
  }  
  [self updateTimer];
}

-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
  return NO;
}

- (BOOL)applicationShouldHandleReopen:(NSApplication *)theApplication hasVisibleWindows:(BOOL)flag{
	[NSApp arrangeInFront:self];
	[messageWindow makeKeyAndOrderFront:self];
	[NSApp activateIgnoringOtherApps:YES];
	return YES;
}

- (void)controlTextDidChange:(NSNotification *)nd {
	NSTextField *urlTextField = [nd object];
  NSString *url = [NSString stringWithFormat:@"%@?test" ,  [urlTextField stringValue]];
  if([self testURL:url] == true){
    [connectionImageView setImage:[NSImage imageNamed:@"tick"]];
    return;
  }
  [connectionImageView setImage:[NSImage imageNamed:@"cross"]];
}

- (bool)testURL: (NSString *) url{
  NSString *returnData = [NSString stringWithContentsOfURL:[NSURL URLWithString: url]];
  
  //TODO: Improve testing to check for 'TEST OK'
  if(returnData != nil){
    return true;
  }
  return false;
}

- (IBAction)openSite:(id)sender{
  NSString * url= [[NSUserDefaults standardUserDefaults] objectForKey:@"address"];
  NSURL *site = [NSURL URLWithString:url];
  [[NSWorkspace sharedWorkspace] openURL:site];  
}



@end
