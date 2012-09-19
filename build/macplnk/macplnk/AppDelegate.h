//
//  AppDelegate.h
//  macplnk
//
//  Created by Martin Robinson on 19/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "AudioHost.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    AudioHost* host;
    IBOutlet NSWindow *window;
}

-(IBAction)buttonTapped:(NSButton*)sender;

@property (assign) IBOutlet NSWindow *window;

@end
