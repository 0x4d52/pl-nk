//
//  PAEBufferCapture.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEBufferCapture ()
-(void)write:(QueueBuffer)queueBuffer;
@end

////////////////////////////////////////////////////////////////////////////////

class PAEBufferCapturePeer : public Channel::Receiver,
                             public Threading::Thread
{
public:
    PAEBufferCapturePeer (PAEBufferCapture* p, const int bufferSize_, const int numBuffers_) throw()
    :   peer (p),
        bufferSize (bufferSize_),
        numBuffers (numBuffers_)
    {
        int n = numBuffers;
        
        while (n--)
        {
            freeQueue.push (QueueBuffer (peer.numChannels, bufferSize));
        }        
    }
    
    ResultCode run() throw()
    {
        const QueueBuffer null = BufferQueue::getNullValue();
                
        while (peer && !getShouldExit())
        {
            if (liveQueue.length() > 0)
            {
                QueueBuffer queueBuffer = liveQueue.pop();
            
                if (queueBuffer != null)
                {
                    [peer write:queueBuffer];
                    freeQueue.push (queueBuffer);
                }
            }
            
            if (liveQueue.length() == 0)
            {
                event.wait (0.0);
            }
            else
            {
                Threading::yield();
            }
            
        }
        
        delete this;
        
        return PlankResult_ThreadWasDeleted;
    }
    
    void changed (Channel const& source, Text const& message, Dynamic const& payload) throw()
    {
        if (message == Text::getMessageQueueBuffer())
        {
            liveQueue.push (payload.as<QueueBuffer>());
            event.signal();
        }
    }
    
    void end() throw()
    {
        peer = nil;
        Lock e = event;
        setShouldExit();
        e.signal();
    }
    
    BufferQueue& getFreeQueue() throw() { return freeQueue; }
    BufferQueue& getLiveQueue() throw() { return liveQueue; }
    
private:
    __weak PAEBufferCapture* peer;
    BufferQueue freeQueue, liveQueue;
    Lock event;
    const int bufferSize;
    const int numBuffers;
};

////////////////////////////////////////////////////////////////////////////////

@implementation PAEBufferCapture
{
    PAEBufferCapturePeer* _peer;
    Unit _record;
    int _framePosition;
}

+(PAEBufferCapture*)bufferCaptureWithNumInputs:(int)numInputs
{
    return [[PAEBufferCapture alloc] initWithNumInputs:numInputs
                                            bufferSize:512
                                            numBuffers:64];
}

+(PAEBufferCapture*)bufferCaptureWithNumInputs:(int)numInputs bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    return [[PAEBufferCapture alloc] initWithNumInputs:numInputs
                                            bufferSize:bufferSize
                                            numBuffers:numBuffers];
}

-(id)initWithNumInputs:(int)numInputs bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    if (self = [super initWithNumInputs:numInputs])
    {
        _peer = new PAEBufferCapturePeer (self, bufferSize, numBuffers);
        _peer->start();
        _record = BufferQueueRecord::ar (self.patchUnit, _peer->getFreeQueue());
        _record.addReceiverToChannels (_peer);
        self.outputUnit = _record;
    }
    
    return self;
}

-(void)dealloc
{
    _record.removeReceiverFromChannels (_peer);
    _peer->end(); // deletes itself when the thread exits
}

-(void)write:(QueueBuffer)queueBuffer
{
    PAEBuffer* buffer = self.buffer;
    
    const unsigned int bufferChannels = buffer.numChannels;
    const int frameStride = buffer.frameStride;
    
    int queueBufferPosition = 0;
    int bufferFramePosition;
    
    while (queueBufferPosition < queueBuffer.getNumFrames())
    {
        int queueBufferFramesThisTime = 0;
        
        for (unsigned int channel = 0; channel < bufferChannels; ++channel)
        {
            bufferFramePosition = _framePosition;
            
            int bufferFramesRemaining = buffer.numFrames - bufferFramePosition;
            queueBufferFramesThisTime = plonk::min (bufferFramesRemaining, queueBuffer.getNumFrames() - queueBufferPosition);
            
            float *bufferSamples = [buffer samples:channel] + bufferFramePosition * frameStride;
            const float* queueBufferSamples = queueBuffer.getBuffers().wrapAt (channel).getArray() + queueBufferPosition;
            
            for (int i = 0; i < queueBufferFramesThisTime; ++i)
            {
                *bufferSamples = *queueBufferSamples++;
                bufferSamples += frameStride;
            }
        }
        
        queueBufferPosition += queueBufferFramesThisTime;
        _framePosition += queueBufferFramesThisTime;
        
        if (_framePosition == buffer.numFrames)
        {
            _framePosition = 0;
            
            if (self.captureDelegate)
            {
                self.buffer = buffer;
                if (![self.captureDelegate bufferReady:self])
                    break;
            }
        }
    }
    
}

@end



