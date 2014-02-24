/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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
 
 This software makes use of third party libraries. See PAEEngine.h
 
 -------------------------------------------------------------------------------
 */

#import <Foundation/Foundation.h>

// core
#import "PAEAudioHost.h"
#import "PAESource.h"
#import "PAEProcess.h"
#import "PAEChannelStrip.h"
#import "PAEMap.h"
#import "PAEBuffer.h"

// sources
#import "PAEAudioInput.h"
#import "PAEConstant.h"
#import "PAEControl.h"
#import "PAETestTone.h"
#import "PAEOscillator.h"
#import "PAEAudioFilePlayer.h"
#import "PAEBufferPlayer.h"
#import "PAEBufferLookup.h"

// processes
#import "PAEMixer.h"
#import "PAEFilter.h"
#import "PAEDelay.h"
#import "PAEFollower.h"
#import "PAEGate.h"
#import "PAECompressor.h"
#import "PAEPan.h"
#import "PAEMidSide.h"
#import "PAEAmplitude.h"
#import "PAESend.h"
#import "PAEBufferCapture.h"

// UI
#import "PAEBufferView.h"

#define PAEENGINE_VERSION "0.1.9"

/**
@mainpage Documentation
 
 PAEEngine is an Objective-C API for simple audio patching on iOS.
 
 <p>By Martin Robinson.
 
 <p>
 This project is built on the cross-platform audio framwork pl-nk (Plonk|Plink|Plank). 
 http://code.google.com/p/pl-nk/
 
 Download the latest PAEEngine pre-built library from here:
 https://dl.dropboxusercontent.com/u/18309733/PAEEngine/PAEEngine.zip
 
 Get started here:
 https://dl.dropboxusercontent.com/u/18309733/PAEEngine/index.html
*/


//    Third-party libraries
//     
//     This software makes use of the following third party libraries.
//     The licenses and restrictions for these are shown below. For most of these you
//     should inlcude their license text.
//     
//     ================================================================================
//     dspfilterscpp
//     -------------
//     
//     "A Collection of Useful C++ Classes for Digital Signal Processing"
//     By Vincent Falco
//     
//     Official project location:
//     http://code.google.com/p/dspfilterscpp/
//     
//     --------------------------------------------------------------------------------
//     
//     License: MIT License (http://www.opensource.org/licenses/mit-license.php)
//     Copyright (c) 2009 by Vincent Falco
//     
//     Permission is hereby granted, free of charge, to any person obtaining a copy
//     of this software and associated documentation files (the "Software"), to deal
//     in the Software without restriction, including without limitation the rights
//     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//     copies of the Software, and to permit persons to whom the Software is
//     furnished to do so, subject to the following conditions:
//     
//     The above copyright notice and this permission notice shall be included in
//     all copies or substantial portions of the Software.
//     
//     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//     THE SOFTWARE.
//     ================================================================================
//     
//     
//     
//     ================================================================================
//     libatomic_ops
//     -------------
//     
//     This software makes use only of the non-GPL parts of libatomic_ops
//     
//     /*
//     * Copyright (c) 1991-1994 by Xerox Corporation.  All rights reserved.
//     * Copyright (c) 1996-1999 by Silicon Graphics.  All rights reserved.
//     * Copyright (c) 1999-2003 by Hewlett-Packard Company. All rights reserved.
//     *
//     *
//     * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
//     * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
//     *
//     * Permission is hereby granted to use or copy this program
//     * for any purpose,  provided the above notices are retained on all copies.
//     * Permission to modify the code and to distribute modified code is granted,
//     * provided the above notices are retained, and a notice that the code was
//     * modified is included with the above copyright notice.
//     *
//     * Some of the machine specific code was borrowed from our GC distribution.
//     */
//    ----------------------------------------
//
//    Copyright (c) ...
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//    SOFTWARE.
//
//    --------------------------------
//
//    A few files in the sysdeps directory were inherited in part from the
//    Boehm-Demers-Weiser conservative garbage collector, and are covered by
//    its license, which is similar in spirit:
//
//    --------------------------------
//
//    Copyright (c) ...
//
//    THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
//    OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
//
//    Permission is hereby granted to use or copy this program
//    for any purpose,  provided the above notices are retained on all copies.
//    Permission to modify the code and to distribute modified code is granted,
//    provided the above notices are retained, and a notice that the code was
//    modified is included with the above copyright notice.
//
//    ================================================================================
//
//    STK
//    ---
//    WWW site: http://ccrma.stanford.edu/software/stk/
//
//    The Synthesis ToolKit in C++ (STK)
//    Copyright (c) 1995-2010 Perry R. Cook and Gary P. Scavone
//
//    Permission is hereby granted, free of charge, to any person obtaining
//    a copy of this software and associated documentation files (the
//                                                                "Software"), to deal in the Software without restriction, including
//    without limitation the rights to use, copy, modify, merge, publish,
//    distribute, sublicense, and/or sell copies of the Software, and to
//    permit persons to whom the Software is furnished to do so, subject to
//    the following conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    Any person wishing to distribute modifications to the Software is
//    asked to send the modifications to the original developer so that they
//    can be incorporated into the canonical version.  This is, however, not
//    a binding provision of this license.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
//    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
//    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
//    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
//    ================================================================================
//
//    Boost
//    -----
//    http://boost.org
//
//    Boost Software License - Version 1.0 - August 17th, 2003
//
//    Permission is hereby granted, free of charge, to any person or organization
//    obtaining a copy of the software and accompanying documentation covered by
//    this license (the "Software") to use, reproduce, display, distribute,
//    execute, and transmit the Software, and to prepare derivative works of the
//    Software, and to permit third-parties to whom the Software is furnished to
//    do so, all subject to the following:
//
//    The copyright notices in the Software and this entire statement, including
//    the above license grant, this restriction and the following disclaimer,
//    must be included in all copies of the Software, in whole or in part, and
//    all derivative works of the Software, unless such copies or derivative
//    works are solely in the form of machine-executable object code generated by
//    a source language processor.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
//    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
//    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
//    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//    DEALINGS IN THE SOFTWARE.
//
//    ================================================================================
//
//    Rig
//    ---
//    https://dev.longitekk.com/Rig/
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions
//    are met:
//
//    1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//    2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
//    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
//    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
//    ================================================================================
//
//    PortAudio
//    ---------
//    http://www.portaudio.com/
//
//    * Copyright (c) 1999-2002 Ross Bencina and Phil Burk
//    *
//    * Permission is hereby granted, free of charge, to any person obtaining
//    * a copy of this software and associated documentation files
//    * (the "Software"), to deal in the Software without restriction,
//    * including without limitation the rights to use, copy, modify, merge,
//    * publish, distribute, sublicense, and/or sell copies of the Software,
//    * and to permit persons to whom the Software is furnished to do so,
//    * subject to the following conditions:
//    *
//    * The above copyright notice and this permission notice shall be
//    * included in all copies or substantial portions of the Software.
//    *
//    * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//    * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//    * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
//    * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
//    * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
//    * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//    */
//
//    /*
//     * The text above constitutes the entire PortAudio license; however,
//     * the PortAudio community also makes the following non-binding requests:
//     *
//     * Any person wishing to distribute modifications to the Software is
//     * requested to send the modifications to the original developer so that
//     * they can be incorporated into the canonical version. It is also
//     * requested that these non-binding requests be included along with the
//     * license above.
//     */
//
//    ================================================================================
//
//    libfixmath
//    ----------
//    http://code.google.com/p/libfixmath/
//
//    NB no license is included with the distibution but the website states
//    the usage of the MIT license
//
//    The MIT License (MIT)
//
//    Copyright (c) <year> <copyright holders>
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy of
//    this software and associated documentation files (the "Software"), to deal in
//    the Software without restriction, including without limitation the rights to
//    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//    of the Software, and to permit persons to whom the Software is furnished to do
//    so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
//    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//    ================================================================================
//
//    Fixed Point Math Library
//    ------------------------
//    http://code.google.com/p/fpmath/
//
//    /*******************************************************************************/
//    /*                                                                             */
//    /*  Copyright (c) 2007-2009: Peter Schregle,                                   */
//    /*  All rights reserved.                                                       */
//    /*                                                                             */
//    /*  This file is part of the Fixed Point Math Library.                        */
//    /*                                                                             */
//    /*  Redistribution of the Fixed Point Math Library and use in source and      */
//    /*  binary forms, with or without modification, are permitted provided that    */
//    /*  the following conditions are met:                                          */
//    /*  1. Redistributions of source code must retain the above copyright notice,  */
//    /*     this list of conditions and the following disclaimer.                   */
//    /*  2. Redistributions in binary form must reproduce the above copyright       */
//    /*     notice, this list of conditions and the following disclaimer in the     */
//    /*     documentation and/or other materials provided with the distribution.    */
//    /*  3. Neither the name of Peter Schregle nor the names of other contributors  */
//    /*     may be used to endorse or promote products derived from this software   */
//    /*     without specific prior written permission.                              */
//    /*                                                                             */
//    /*  THIS SOFTWARE IS PROVIDED BY PETER SCHREGLE AND CONTRIBUTORS 'AS IS' AND   */
//    /*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      */
//    /*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE */
//    /*  ARE DISCLAIMED. IN NO EVENT SHALL PETER SCHREGLE OR CONTRIBUTORS BE LIABLE */
//    /*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL */
//    /*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS    */
//    /*  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)      */
//    /*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,        */
//    /*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN   */
//    /*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE            */
//    /*  POSSIBILITY OF SUCH DAMAGE.                                                */
//    /*                                                                             */
//    /*******************************************************************************/
//
//
//    ================================================================================
//
//    libvorbis / libogg
//    ------------------
//    Xiph.org
//
//    Copyright (c) 2002-2004 Xiph.org Foundation
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions
//    are met:
//
//    - Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    - Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    - Neither the name of the Xiph.org Foundation nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
//    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//    ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//    A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
//    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//                                                  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//                                                  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
//    ================================================================================
//
//    libopus
//    -------
//    Xiph.org
//
//    Copyright 2001-2011 Xiph.Org, Skype Limited, Octasic,
//    Jean-Marc Valin, Timothy B. Terriberry,
//    CSIRO, Gregory Maxwell, Mark Borgerding,
//    Erik de Castro Lopo
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions
//    are met:
//
//    - Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    - Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    - Neither the name of Internet Society, IETF or IETF Trust, nor the
//    names of specific contributors, may be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
//
//    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//    ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
//    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    Opus is subject to the royalty-free patent licenses which are
//    specified at:
//
//    Xiph.Org Foundation:
//    https://datatracker.ietf.org/ipr/1524/
//
//    Skype Limited:
//    https://datatracker.ietf.org/ipr/1602/
//
//    Broadcom Corporation:
//    https://datatracker.ietf.org/ipr/1526/
//
//    ================================================================================
//
//    Jansson
//    -------
//    http://www.digip.org/jansson/
//
//    Copyright (c) 2009-2012 Petri Lehtinen <petri@digip.org>
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in
//    all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//    THE SOFTWARE.
//
//    ================================================================================
//
//    zlib
//    ----
//    http://www.gzip.org
//
//    zlib.h -- interface of the 'zlib' general purpose compression library
//    version 1.2.2, October 3rd, 2004
//
//    Copyright (C) 1995-2004 Jean-loup Gailly and Mark Adler
//
//    This software is provided 'as-is', without any express or implied
//    warranty.  In no event will the authors be held liable for any damages
//    arising from the use of this software.
//
//    Permission is granted to anyone to use this software for any purpose,
//    including commercial applications, and to alter it and redistribute it
//    freely, subject to the following restrictions:
//
//    1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//    2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//    3. This notice may not be removed or altered from any source distribution.
//
//    Jean-loup Gailly jloup@gzip.org
//    Mark Adler madler@alumni.caltech.edu
//
//    ================================================================================
//
//    Juce
//    ----
//    http://www.rawmaterialsoftware.com
//
//    N.B. Juce is not a dependency for Plink/Plonk/Plank but a JuceAudioHost is
//    provided for convenience. To use this you will need to ensure that your product
//    license is compatible with the Juce license terms and the license terms for
//    Plink/Plonk/Plank.
//
//    Juce can be redistributed and/or modified under the terms of the GNU General
//    Public License (Version 2), as published by the Free Software Foundation.
//    A copy of the license is included in the Juce distribution, or can be found
//    online at www.gnu.org/licenses.
//
//    Juce is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//    A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    ------------------------------------------------------------------------------
//
//    To release a closed-source product which uses Juce, commercial licenses are
//    available: visit www.rawmaterialsoftware.com/Juce for more information.
//
//    ================================================================================
//
//    RtAudio
//    -------
//    http://www.music.mcgill.ca/~gary/rtaudio/
//
//    Realtime audio i/o C++ classes.
//
//    RtAudio provides a common API (Application Programming Interface)
//    for realtime audio input/output across Linux (native ALSA, Jack,
//    and OSS), Macintosh OS X (CoreAudio and Jack), and Windows
//    (DirectSound and ASIO) operating systems.
//
//    RtAudio WWW site: http://www.music.mcgill.ca/~gary/rtaudio/
//
//    RtAudio: realtime audio i/o C++ classes
//    Copyright (c) 2001-2013 Gary P. Scavone
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation files
//    (the "Software"), to deal in the Software without restriction,
//    including without limitation the rights to use, copy, modify, merge,
//    publish, distribute, sublicense, and/or sell copies of the Software,
//    and to permit persons to whom the Software is furnished to do so,
//    subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    Any person wishing to distribute modifications to the Software is
//    asked to send the modifications to the original developer so that
//    they can be incorporated into the canonical version.  This is,
//    however, not a binding provision of this license.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
//    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
//    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
//    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//    ================================================================================
//
//    liblfds
//    -------
//
//    http://www.liblfds.org/
//
//    There is no license. You are free to use this software in any way, for any purpose
//
//    ================================================================================

