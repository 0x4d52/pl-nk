//
//  PAEBufferCapture.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
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
    
    PAEBufferCapturePeer (PAEBufferCapture* p) throw()
    :   peer (p)
    {
        int n = 8;
        
        while (n--)
        {
            freeQueue.push (QueueBuffer (peer.numChannels, BlockSize::getDefault().getValue()));
        }
    }
    
    ResultCode run() throw()
    {        
        while (!getShouldExit())
        {
            QueueBuffer queueBuffer = liveQueue.pop();
            
            if (queueBuffer != BufferQueue::getNullValue())
            {                
                [peer write:queueBuffer];
                freeQueue.push (queueBuffer);
            }
            
            lock.wait (0.01);
        }
        
        delete this;
        
        return PlankResult_ThreadWasDeleted;
    }
    
    void changed (Channel const& source, Text const& message, Dynamic const& payload) throw()
    {
        if (message == Text::getMessageQueueBuffer())
        {
            QueueBuffer buffer = payload.as<QueueBuffer>();
            liveQueue.push (buffer);
            lock.signal();
        }
    }
    
    BufferQueue& getFreeQueue() throw() { return freeQueue; }
    BufferQueue& getLiveQueue() throw() { return liveQueue; }
    
private:
    PAEBufferCapture* peer;
    BufferQueue freeQueue, liveQueue;
    Lock lock;
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
    return [[PAEBufferCapture alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        _peer = new PAEBufferCapturePeer (self);
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
    _peer->setShouldExit();
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
        int queueBufferFramesThisTime;
        
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
            
            if (self.delegate)
            {
                self.buffer = buffer;
                if (![self.delegate bufferReady:self])
                    break;
            }
        }
    }
    
}

@end



