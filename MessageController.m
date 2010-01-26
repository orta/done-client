//
//  MessageController.m
//  Done
//
//  Created by benmaslen on 07/08/2009.
//  Copyright 2009 ortatherox.com. All rights reserved.
//

#import "MessageController.h"


@implementation MessageController


- (void)awakeFromNib {
  [swirlThing setHidden:true];
}

-(void) sendString: (NSString*) string withSender:(id) sender {  
  responseData = [[NSMutableData data] retain];
  baseURL = [[NSURL URLWithString:string] retain];
  NSURLRequest *request = [NSURLRequest requestWithURL:baseURL];
  [[NSURLConnection alloc] initWithRequest:request delegate:self];
  [swirlThing startAnimation:self];
  [swirlThing setHidden:false];
  [sendButton setEnabled:false];
  lastSender = sender;
}

- (NSURLRequest *)connection:(NSURLConnection *)connection
  willSendRequest:(NSURLRequest *)request
  redirectResponse:(NSURLResponse *)redirectResponse {
  [baseURL autorelease];
  baseURL = [[request URL] retain];
  return request;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
  [responseData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
  [responseData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
  [[NSAlert alertWithError:error] runModal];
  [swirlThing stopAnimation:self];
  [swirlThing setHidden:true];
  [sendButton setEnabled:true];
  
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
  // Once this method is invoked, "responseData" contains the complete result
  NSString*response = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
  log(@"/%@/", response);
  [swirlThing stopAnimation:self];
  [swirlThing setHidden:true];
  [sendButton setEnabled:true];
  
  if( [response compare:@"OK"] == NSOrderedSame){
    [[NSNotificationCenter defaultCenter] postNotificationName:@"ORMessageSent" object:@""];     
  }
}


@end
