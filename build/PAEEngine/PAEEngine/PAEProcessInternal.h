//
//  PAEProcessInternal.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAEProcess.h"
#include "plonk.h"

@interface PAEProcess ()
@property (nonatomic) Unit inputUnit;
@property (nonatomic, readonly) Unit patchUnit;
@end
