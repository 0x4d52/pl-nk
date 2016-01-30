//
//  PAEBufferView.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEBufferView ()
@property (strong, nonatomic) NSArray* bufferChannelViews;
@property (nonatomic) CGFloat samplesPerPixel;
@end

@interface PAEBufferChannelView : UIView
@property (weak, nonatomic) PAEBufferView* parent;
@property (strong, nonatomic) PAEBuffer* buffer;
@property (nonatomic) int channel;
@end

@implementation PAEBufferChannelView

-(void)drawRect:(CGRect)rect
{
    self.opaque = self.parent.opaque;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextClearRect (context, rect);
    
    [self.parent.backgroundColor setFill];
    
    CGContextFillRect (context, rect);
    CGContextStrokePath (context);
    
    CGContextSetLineWidth (context, self.parent.traceWidth);

    [self.parent.borderColor setStroke];
    
    CGContextStrokeRect (context, rect);

    const CGFloat width = rect.size.width;
    const CGFloat height = rect.size.height;

    const int channel = self.channel;
    const CGFloat samplesPerPixel = self.parent.samplesPerPixel;

    [self.parent.traceColor setStroke];

    if (samplesPerPixel >= 1.0)
    {
        CGFloat blockStart = 0;
        CGFloat blockEnd = samplesPerPixel;
        
        for (int i = 0; i < int (width); ++i)
        {
            PAERange range = [self.buffer limitsBetween:int (blockStart + 0.5)
                                                    end:int (blockEnd + 0.5)
                                                channel:channel];
            
            range.minimum = (0.5 - range.minimum * 0.5) * height;
            range.maximum = (0.5 - range.maximum * 0.5) * height;

            CGContextMoveToPoint (context, i + 0.5, range.maximum - 0.5);
            CGContextAddLineToPoint (context, i + 0.5, range.minimum + 0.5);
            
            blockStart = blockEnd;
            blockEnd += samplesPerPixel;
        }
        
        CGContextStrokePath (context);
    }
    else
    {
        CGFloat index = 0;
        CGFloat value = (0.5 - [self.buffer sampleAtIndex:0 channel:channel] * 0.5) * height;
        CGContextMoveToPoint (context, 0.5, value);

        for (int i = 0; i < int (width); ++i)
        {
            CGFloat value = (0.5 - [self.buffer sampleAtIndex:int (index + 0.5) channel:channel] * 0.5) * height;
            CGContextAddLineToPoint (context, i + 0.5, value);
            index += samplesPerPixel;
        }
        
        CGContextStrokePath (context);
    }
}

@end

////////////////////////////////////////////////////////////////////////////////

@implementation PAEBufferView

@synthesize bufferChannelViews = _bufferChannelViews;
@synthesize buffer = _buffer;

-(void)commonInit
{
    self.borderColor = [UIColor blackColor];
    self.traceColor = [UIColor colorWithWhite:0.5 alpha:1.0];
    self.traceWidth = 1.0;
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    if (self = [super initWithCoder:aDecoder])
    {
        [self commonInit];
    }
    
    return self;
}

-(id)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        [self commonInit];
    }
    
    return self;
}

-(PAEBuffer*)buffer
{
    return _buffer;
}

-(void)setBuffer:(PAEBuffer *)buffer
{
    if (buffer == _buffer)
    {
        for (PAEBufferChannelView* bufferChannelView in self.bufferChannelViews)
        {
            [bufferChannelView performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];
        }
    }
    else if (buffer.numChannels != _buffer.numChannels)
    {
        _buffer = nil;
        
        for (UIView* view in self.subviews)
        {
            [view performSelector:@selector(removeFromSuperview)];
        }
     
        NSMutableArray* subviews;
        
        if (buffer && buffer.numChannels > 0)
        {
            subviews = [[NSMutableArray alloc] initWithCapacity:buffer.numChannels];

            for (int i = 0; i < buffer.numChannels; ++i)
            {
                PAEBufferChannelView* bufferChannelView = [[PAEBufferChannelView alloc] init];
                bufferChannelView.parent = self;
                bufferChannelView.buffer = buffer;
                bufferChannelView.channel = i;
                [self addSubview:bufferChannelView];
                [subviews addObject:bufferChannelView];
            }
        }
        
        _buffer = buffer;
        self.bufferChannelViews = [NSArray arrayWithArray:subviews];
    }
    else
    {
        _buffer = buffer;
    }
}

-(NSArray*)bufferChannelViews
{
    return _bufferChannelViews;
}

-(void)setBufferChannelViews:(NSArray *)bufferChannelViews
{
    _bufferChannelViews = bufferChannelViews;
    [self layoutIfNeeded];
}

-(void)layoutSubviews
{
    if (self.bufferChannelViews.count > 0)
    {
        CGRect frame = self.bounds;
        CGFloat channelHeight = frame.size.height / self.bufferChannelViews.count;
        frame.size.height = channelHeight;
        
        for (PAEBufferChannelView* bufferChannelView in self.bufferChannelViews)
        {
            bufferChannelView.frame = frame;
            frame.origin.y += channelHeight;
        }
        
        self.samplesPerPixel = self.buffer.numFrames / frame.size.width;
    }
}

@end







