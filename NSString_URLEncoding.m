//
//  NSString_URLEncoding.m
//  Tumblor
//
//  Created by orta on 23/07/2008.
//  Copyright 2008 ortatherox. All rights reserved.
//

#import "NSString_URLEncoding.h"


@implementation NSString (URLEnc)

- (NSString *)urlEncodeValue  {
  return [self stringByAddingPercentEscapesUsingEncoding:  NSASCIIStringEncoding];

}
  

@end
