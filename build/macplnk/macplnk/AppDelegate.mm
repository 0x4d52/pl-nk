//
//  AppDelegate.m
//  macplnk
//
//  Created by Martin Robinson on 19/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"



@implementation AppDelegate

@synthesize window = _window;

- (id)init
{
    if (self = [super init])
    {
        host = new AudioHost;
        host->getHostName().print ("Host");
        host->getNativeHostName().print ("Native");
        host->getInputName().print ("Input");
        host->getOutputName().print ("Output");
    }
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    host->startHost();
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    [[NSNotificationCenter defaultCenter] removeObject:self];
    
    host->stopHost();
    delete host;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

-(IBAction)buttonChanged:(NSButton*)sender
{
    host->setGate([sender intValue] == 1 ? true : false);
}


@end
