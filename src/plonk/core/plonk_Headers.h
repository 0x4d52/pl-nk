/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-12
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of University of the West of England, Bristol nor 
   the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF THE WEST OF ENGLAND, BRISTOL BE 
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 
 This software makes use of third party libraries. For more information see:
 doc/license.txt included in the distribution.
 -------------------------------------------------------------------------------
 */

#ifndef PLONKHEADERS_H
#define PLONKHEADERS_H

#include "plonk_CoreForwardDeclarations.h"
#include "plonk_Memory.h"
#include "plonk_SmartPointer.h"
#include "plonk_WeakPointer.h"
#include "plonk_SmartPointerContainer.h"
#include "plonk_WeakPointerContainer.h"
#include "plonk_Deleter.h"
#include "plonk_Sender.h"
#include "plonk_SenderContainer.h"
#include "plonk_Receiver.h"
#include "plonk_TypeUtility.h"
#include "plonk_Lock.h"
#include "plonk_Thread.h"

#include "../containers/plonk_ContainerForwardDeclarations.h"
#include "../containers/plonk_Atomic.h"
#include "../containers/plonk_DynamicContainer.h"
#include "../containers/plonk_ObjectArrayInternal.h"
#include "../containers/plonk_SimpleArray.h"
#include "../containers/plonk_SimpleStack.h"
#include "../containers/plonk_SimpleQueue.h"
#include "../containers/plonk_SimpleLinkedList.h"
#include "../containers/plonk_ObjectArray.h"
#include "../containers/plonk_ObjectArray2D.h"
#include "../containers/plonk_NumericalArray.h"
#include "../containers/plonk_NumericalArray2D.h"
#include "../containers/plonk_Collections.h"
#include "../containers/plonk_Text.h"
#include "../containers/plonk_TextArray.h"
#include "../containers/plonk_BreakPoints.h"
#include "../containers/plonk_Wavetable.h"
#include "../containers/plonk_Signal.h"
#include "../containers/plonk_Int24.h"
#include "../containers/plonk_Function.h"
#include "../containers/plonk_LockFreeQueue.h"
#include "../containers/plonk_LockFreeStack.h"

#include "../containers/variables/plonk_VariableForwardDeclarations.h"
#include "../containers/variables/plonk_Variable.h"
#include "../containers/variables/plonk_VariableInternal.h"
#include "../containers/variables/plonk_BinaryOpVariable.h"
#include "../containers/variables/plonk_UnaryOpVariable.h"
#include "../containers/variables/plonk_TypeVariable.h"

#include "../maths/plonk_Constants.h"
#include "../maths/plonk_Endian.h"

#include "../random/plonk_RNG.h"

#include "../files/plonk_TextFile.h"
#include "../files/plonk_BinaryFile.h"

#include "../files/audio/plonk_AudioFileReader.h"

#include "../fft/plonk_FFTEngine.h"
#include "../fft/plonk_FFTEngineInternal.h"

#include "../graph/plonk_GraphForwardDeclarations.h"

#include "../graph/channel/plonk_Channel.h"
#include "../graph/channel/plonk_ChannelInternalBase.h"
#include "../graph/channel/plonk_ChannelInternal.h"
#include "../graph/channel/plonk_ChannelData.h"
#include "../graph/channel/plonk_ProxyChannelInternal.h"
#include "../graph/channel/plonk_ProxyOwnerChannelInternal.h"


#include "../graph/utility/plonk_InputDictionary.h"
#include "../graph/utility/plonk_BlockSize.h"
#include "../graph/utility/plonk_SampleRate.h"
#include "../graph/utility/plonk_Bus.h"
#include "../graph/utility/plonk_TimeStamp.h"
#include "../graph/utility/plonk_ProcessInfo.h"
#include "../graph/utility/plonk_ProcessInfoInternal.h"

#include "../graph/info/plonk_InfoHeaders.h"

#include "../graph/plonk_Unit.h"

#include "../graph/converters/plonk_TypeChannel.h"
#include "../graph/converters/plonk_ResampleChannel.h"
#include "../graph/converters/plonk_OverlapMakeChannel.h"
#include "../graph/converters/plonk_OverlapMixChannel.h"

#include "../graph/simple/plonk_BinaryOpChannel.h"
#include "../graph/simple/plonk_ConstantChannel.h"
#include "../graph/simple/plonk_LinearPanChannel.h"
#include "../graph/simple/plonk_UnaryOpChannel.h"
#include "../graph/simple/plonk_BusReadChannel.h"
#include "../graph/simple/plonk_BusWriteChannel.h"
#include "../graph/simple/plonk_Mixers.h"
#include "../graph/simple/plonk_BlockChannel.h"
#include "../graph/simple/plonk_VariableChannel.h"
#include "../graph/simple/plonk_AtomicVariableChannel.h"

#include "../graph/generators/plonk_Saw.h"
#include "../graph/generators/plonk_Table.h"
#include "../graph/generators/plonk_SignalPlay.h"

#include "../graph/filters/plonk_FilterForwardDeclarations.h"
#include "../graph/filters/plonk_FilterTypes.h"
#include "../graph/filters/plonk_FilterForms.h"
#include "../graph/filters/plonk_FilterShapes.h"
#include "../graph/filters/plonk_Filter.h"
#include "../graph/filters/plonk_FilterCoeffs1Param.h"
#include "../graph/filters/plonk_FilterCoeffs2Param.h"
#include "../graph/filters/plonk_FilterCoeffs3Param.h"
#include "../graph/filters/plonk_FilterShortcuts.h"
#include "../graph/filters/shapes/plonk_FilterShapesSimple.h"
#include "../graph/filters/shapes/plonk_FilterShapesP2Z2.h"
#include "../graph/filters/shapes/plonk_FilterShapesB.h"

#include "../graph/delay/plonk_DelayForwardDeclarations.h"
#include "../graph/delay/plonk_DelayBase.h"
#include "../graph/delay/plonk_DelayTypes.h"
#include "../graph/delay/plonk_DelayForms.h"
#include "../graph/delay/plonk_DelayFormDelay.h"
#include "../graph/delay/plonk_DelayFormCombDecay.h"
#include "../graph/delay/plonk_DelayFormCombFB.h"
#include "../graph/delay/plonk_DelayFormAllpassDecay.h"
#include "../graph/delay/plonk_DelayFormAllpassFFFB.h"
#include "../graph/delay/plonk_Delay1Param.h"
#include "../graph/delay/plonk_Delay2Param.h"
#include "../graph/delay/plonk_Delay3Param.h"
#include "../graph/delay/plonk_Delay4Param.h"

#include "../graph/control/plonk_EnvelopeChannel.h"

#include "../graph/fft/plonk_FFTChannel.h"
#include "../graph/fft/plonk_IFFTChannel.h"

#include "../hosts/plonk_AudioHostBase.h"

#endif // PLONKHEADERS_H