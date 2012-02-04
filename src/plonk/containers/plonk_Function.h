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

#ifndef PLONK_FUNCTION_H
#define PLONK_FUNCTION_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"



//------------------------------------------------------------------------------

class FunctionContainerBase
{
public:
    FunctionContainerBase() throw()  { }
    virtual ~FunctionContainerBase() { }
};

//------------------------------------------------------------------------------
// Functions
// 

template<class ReturnType,
         class ArgType1, 
         class ArgType2,
         class ArgType3>
class FunctionContainer : public FunctionContainerBase
{
public:
    typedef ReturnType  Return;
    typedef ArgType1    Arg1;
    typedef ArgType2    Arg2;
    typedef ArgType3    Arg3;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    typedef typename TypeUtility<ArgType1>::OriginalType Arg1OriginalType;
    typedef typename TypeUtility<ArgType2>::OriginalType Arg2OriginalType;
    typedef typename TypeUtility<ArgType3>::OriginalType Arg3OriginalType;
    
    FunctionContainer (Arg1PassType argument1, 
                       Arg2PassType argument2,
                       Arg3PassType argument3) throw() 
    :   value1 (argument1),
        value2 (argument2),
        value3 (argument3)
    { 
    }
    
    inline ReturnType operator()() throw() 
    { 
        return function (value1, value2, value3); 
    }
    
    virtual ReturnType function (Arg1PassType argument1, 
                                 Arg1PassType argument2,
                                 Arg1PassType argument3) = 0;
    
    static const int argumentCount;
    static const bool isProxy;
    
private:
    Arg1OriginalType value1;
    Arg2OriginalType value2;
    Arg3OriginalType value3;
};

template<class ReturnType, class ArgType1, class ArgType2, class ArgType3>
const int FunctionContainer<ReturnType, ArgType1, ArgType2, ArgType3>::argumentCount = 3;

template<class ReturnType, class ArgType1, class ArgType2, class ArgType3>
const bool FunctionContainer<ReturnType, ArgType1, ArgType2, ArgType3>::isProxy = false;


//------------------------------------------------------------------------------

template<class ReturnType>
class FunctionContainer<ReturnType> : public FunctionContainerBase
{
public:
    typedef ReturnType Return;
    
    FunctionContainer() throw()  { }    
    inline ReturnType operator()() throw() { return function(); }
    virtual ReturnType function() = 0;
    
    static const int argumentCount;
    static const bool isProxy;
};

template<class ReturnType>
const int FunctionContainer<ReturnType>::argumentCount = 0;

template<class ReturnType>
const bool FunctionContainer<ReturnType>::isProxy = false;


//------------------------------------------------------------------------------

template<class ReturnType, class ArgType1>
class FunctionContainer<ReturnType, ArgType1> : public FunctionContainerBase
{
public:
    typedef ReturnType  Return;
    typedef ArgType1    Arg1;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType1>::OriginalType Arg1OriginalType;    
    
    FunctionContainer (Arg1PassType argument1) throw() 
    :   value1 (argument1) 
    { 
    }
    
    inline ReturnType operator()() throw() 
    { 
        return function (value1); 
    }
    
    virtual ReturnType function (Arg1PassType argument1) = 0;
    
    static const int argumentCount;
    static const bool isProxy;
    
private:
    Arg1OriginalType value1;
};

template<class ReturnType, class ArgType1>
const int FunctionContainer<ReturnType, ArgType1>::argumentCount = 1;

template<class ReturnType, class ArgType1>
const bool FunctionContainer<ReturnType, ArgType1>::isProxy = false;

//------------------------------------------------------------------------------

template<class ReturnType, class ArgType1, class ArgType2>
class FunctionContainer<ReturnType, ArgType1, ArgType2> : public FunctionContainerBase
{
public:
    typedef ReturnType  Return;
    typedef ArgType1    Arg1;
    typedef ArgType2    Arg2;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType1>::OriginalType Arg1OriginalType;
    typedef typename TypeUtility<ArgType2>::OriginalType Arg2OriginalType;    
    
    FunctionContainer (Arg1PassType argument1, 
                       Arg2PassType argument2) throw() 
    :   value1 (argument1),
        value2 (argument2) 
    { 
    }
    
    inline ReturnType operator()() throw() 
    { 
        return function (value1, value2); 
    }
    
    virtual ReturnType function (Arg1PassType argument1, 
                                 Arg2PassType argument2) = 0;
    
    static const int argumentCount;
    static const bool isProxy;
    
private:
    Arg1OriginalType value1;
    Arg2OriginalType value2;
};

template<class ReturnType, class ArgType1, class ArgType2>
const int FunctionContainer<ReturnType, ArgType1, ArgType2>::argumentCount = 2;

template<class ReturnType, class ArgType1, class ArgType2>
const bool FunctionContainer<ReturnType, ArgType1, ArgType2>::isProxy = false;


//------------------------------------------------------------------------------
// Function proxies
//

template<class ReturnType,
         class ArgType1, 
         class ArgType2,
         class ArgType3>
class FunctionContainerProxy
{
public:
    typedef ReturnType  Return;
    typedef ArgType1    Arg1;
    typedef ArgType2    Arg2;
    typedef ArgType3    Arg3;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    typedef typename TypeUtility<ArgType1>::OriginalType Arg1OriginalType;
    typedef typename TypeUtility<ArgType2>::OriginalType Arg2OriginalType;
    typedef typename TypeUtility<ArgType3>::OriginalType Arg3OriginalType;    
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType, Arg3PassType);

    FunctionContainerProxy (ProxyFunction proxyFunction,
                            Arg1PassType argument1, 
                            Arg2PassType argument2,
                            Arg3PassType argument3) throw() 
    :   function (proxyFunction),
        value1 (argument1),
        value2 (argument2),
        value3 (argument3)
    { 
    }
    
    inline ReturnType operator()() throw() 
    { 
        return function (value1, value2, value3); 
    }
    
    static const int argumentCount;
    static const bool isProxy;
    
private:
    ProxyFunction function;
    Arg1OriginalType value1;
    Arg2OriginalType value2;
    Arg3OriginalType value3;
};

template<class ReturnType, class ArgType1, class ArgType2, class ArgType3>
const int FunctionContainerProxy<ReturnType, ArgType1, ArgType2, ArgType3>::argumentCount = 3;

template<class ReturnType, class ArgType1, class ArgType2, class ArgType3>
const bool FunctionContainerProxy<ReturnType, ArgType1, ArgType2, ArgType3>::isProxy = true;

//------------------------------------------------------------------------------

template<class ReturnType>
class FunctionContainerProxy<ReturnType>
{
public:
    typedef ReturnType (*ProxyFunction) (void);
    typedef ReturnType Return;
    
    FunctionContainerProxy (ProxyFunction proxyFunction) throw()
    :   function (proxyFunction)
    { 
    }
    
    inline ReturnType operator()() throw() { return function(); }
    
    static const int argumentCount;
    static const bool isProxy;
    
private:
    ProxyFunction function;
};

template<class ReturnType>
const int FunctionContainerProxy<ReturnType>::argumentCount = 0;

template<class ReturnType>
const bool FunctionContainerProxy<ReturnType>::isProxy = true;


//------------------------------------------------------------------------------

template<class ReturnType, class ArgType1>
class FunctionContainerProxy<ReturnType, ArgType1>
{
public:
    typedef ReturnType  Return;
    typedef ArgType1    Arg1;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType1>::OriginalType Arg1OriginalType;    
    typedef ReturnType (*ProxyFunction) (Arg1PassType);

    FunctionContainerProxy (ProxyFunction proxyFunction,
                   Arg1PassType argument1) throw() 
    :   function (proxyFunction),
        value1 (argument1) 
    { 
    }
    
    inline ReturnType operator()() throw() 
    { 
        return function (value1); 
    }
        
    static const int argumentCount;
    static const bool isProxy;
    
private:
    ProxyFunction function;
    Arg1OriginalType value1;
};

template<class ReturnType, class ArgType1>
const int FunctionContainerProxy<ReturnType, ArgType1>::argumentCount = 1;

template<class ReturnType, class ArgType1>
const bool FunctionContainerProxy<ReturnType, ArgType1>::isProxy = true;

//------------------------------------------------------------------------------

template<class ReturnType, class ArgType1, class ArgType2>
class FunctionContainerProxy<ReturnType, ArgType1, ArgType2>
{
public:
    typedef ReturnType  Return;
    typedef ArgType1    Arg1;
    typedef ArgType2    Arg2;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType1>::OriginalType Arg1OriginalType;
    typedef typename TypeUtility<ArgType2>::OriginalType Arg2OriginalType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType);

    FunctionContainerProxy (ProxyFunction proxyFunction,
                            Arg1PassType argument1, 
                            Arg2PassType argument2) throw() 
    :   function (proxyFunction),
        value1 (argument1),
        value2 (argument2) 
    { 
    }
    
    inline ReturnType operator()() throw() 
    { 
        return function (value1, value2); 
    }
    
    static const int argumentCount;
    static const bool isProxy;
    
private:
    ProxyFunction function;
    Arg1OriginalType value1;
    Arg2OriginalType value2;
};

template<class ReturnType, class ArgType1, class ArgType2>
const int FunctionContainerProxy<ReturnType, ArgType1, ArgType2>::argumentCount = 2;

template<class ReturnType, class ArgType1, class ArgType2>
const bool FunctionContainerProxy<ReturnType, ArgType1, ArgType2>::isProxy = true;

//------------------------------------------------------------------------------
// FunctionCallerInternals
//

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2,
         class ArgType3>
class FunctionCallerInternal 
:   public SmartPointer
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    
    FunctionCallerInternal (Arg1PassType argument1, 
                            Arg2PassType argument2,
                            Arg3PassType argument3) throw()
    :   function (argument1, argument2, argument3)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

template<class FunctionType, class ReturnType>
class FunctionCallerInternal<FunctionType, ReturnType> 
:   public SmartPointer
{
public:
    FunctionCallerInternal() throw()
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

template<class FunctionType, class ReturnType, class ArgType1>
class FunctionCallerInternal<FunctionType, ReturnType, ArgType1> 
:   public SmartPointer
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    
    FunctionCallerInternal (Arg1PassType argument1) throw()
    :   function (argument1)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2>
class FunctionCallerInternal<FunctionType, ReturnType, ArgType1, ArgType2>
:   public SmartPointer
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    
    FunctionCallerInternal (Arg1PassType argument1, 
                            Arg2PassType argument2) throw()
    :   function (argument1, argument2)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

//------------------------------------------------------------------------------
// FunctionProxyCallerInternals
//

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2,
         class ArgType3>
class FunctionProxyCallerInternal 
:   public SmartPointer
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType, Arg3PassType);

    FunctionProxyCallerInternal (ProxyFunction proxyFunction,
                                 Arg1PassType argument1, 
                                 Arg2PassType argument2,
                                 Arg3PassType argument3) throw()
    :   function (proxyFunction, argument1, argument2, argument3)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

template<class FunctionType, class ReturnType>
class FunctionProxyCallerInternal<FunctionType, ReturnType> 
:   public SmartPointer
{
public:
    typedef ReturnType (*ProxyFunction) (void);

    FunctionProxyCallerInternal (ProxyFunction proxyFunction) throw()
    :   function (proxyFunction)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

template<class FunctionType, class ReturnType, class ArgType1>
class FunctionProxyCallerInternal<FunctionType, ReturnType, ArgType1> 
:   public SmartPointer
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType);

    FunctionProxyCallerInternal (ProxyFunction proxyFunction,
                                 Arg1PassType argument1) throw()
    :   function (proxyFunction, argument1)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2>
class FunctionProxyCallerInternal<FunctionType, ReturnType, ArgType1, ArgType2>
:   public SmartPointer
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType);

    FunctionProxyCallerInternal (ProxyFunction proxyFunction,
                                 Arg1PassType argument1, 
                                 Arg2PassType argument2) throw()
    :   function (proxyFunction, argument1, argument2)
    {
    }
    
    inline ReturnType call() throw()
    {
        return function();
    }
    
private:
    FunctionType function;
};

//------------------------------------------------------------------------------
// FunctionCallerBases
//

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2,
         class ArgType3>
class FunctionCallerBase 
:   public SmartPointerContainer<FunctionCallerInternal<FunctionType, 
                                                        ReturnType, 
                                                        ArgType1, 
                                                        ArgType2, 
                                                        ArgType3> >
{
public:
    typedef FunctionCallerInternal<FunctionType, 
                                   ReturnType, 
                                   ArgType1, 
                                   ArgType2, 
                                   ArgType3>    Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    
    FunctionCallerBase (Arg1PassType argument1, 
                        Arg2PassType argument2,
                        Arg3PassType argument3) throw()
    :   Base (new Internal (argument1, argument2, argument3))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

template<class FunctionType, class ReturnType>
class FunctionCallerBase<FunctionType, ReturnType>
:   public SmartPointerContainer<FunctionCallerInternal<FunctionType, 
                                                        ReturnType> >
{
public:
    typedef FunctionCallerInternal<FunctionType, ReturnType>    Internal;
    typedef SmartPointerContainer<Internal>                     Base;
    
    FunctionCallerBase() throw()
    :   Base (new Internal())
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

template<class FunctionType, class ReturnType, class ArgType1>
class FunctionCallerBase<FunctionType, ReturnType, ArgType1>
:   public SmartPointerContainer<FunctionCallerInternal<FunctionType, 
                                                        ReturnType, 
                                                        ArgType1> >
{
public:
    typedef FunctionCallerInternal<FunctionType, 
                                   ReturnType, 
                                   ArgType1>    Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    
    FunctionCallerBase (Arg1PassType argument1) throw()
    :   Base (new Internal (argument1))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2>
class FunctionCallerBase<FunctionType, ReturnType, ArgType1, ArgType2>
:   public SmartPointerContainer<FunctionCallerInternal<FunctionType, 
                                                        ReturnType,     
                                                        ArgType1, 
                                                        ArgType2> >
{
public:
    typedef FunctionCallerInternal<FunctionType, 
                                   ReturnType, 
                                   ArgType1, 
                                   ArgType2>    Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    
    FunctionCallerBase (Arg1PassType argument1, 
                        Arg2PassType argument2) throw()
    :   Base (new Internal (argument1, argument2))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

//------------------------------------------------------------------------------
// FunctionProxyCallerBases
//

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2,
         class ArgType3>
class FunctionProxyCallerBase 
:   public SmartPointerContainer<FunctionProxyCallerInternal<FunctionType, 
                                                             ReturnType, 
                                                             ArgType1, 
                                                             ArgType2, 
                                                             ArgType3> >
{
public:
    typedef FunctionProxyCallerInternal<FunctionType, 
                                        ReturnType, 
                                        ArgType1, 
                                        ArgType2, 
                                        ArgType3>       Internal;
    typedef SmartPointerContainer<Internal>             Base;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType, Arg3PassType);

    FunctionProxyCallerBase (ProxyFunction proxyFunction,
                             Arg1PassType argument1, 
                             Arg2PassType argument2,
                             Arg3PassType argument3) throw()
    :   Base (new Internal (proxyFunction, argument1, argument2, argument3))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

template<class FunctionType, class ReturnType>
class FunctionProxyCallerBase<FunctionType, ReturnType>
:   public SmartPointerContainer<FunctionProxyCallerInternal<FunctionType, 
                                                             ReturnType> >
{
public:
    typedef ReturnType (*ProxyFunction) (void);
    typedef FunctionProxyCallerInternal<FunctionType, ReturnType>   Internal;
    typedef SmartPointerContainer<Internal>                         Base;
    
    FunctionProxyCallerBase (ProxyFunction proxyFunction) throw()
    :   Base (new Internal (proxyFunction))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

template<class FunctionType, class ReturnType, class ArgType1>
class FunctionProxyCallerBase<FunctionType, ReturnType, ArgType1>
:   public SmartPointerContainer<FunctionProxyCallerInternal<FunctionType, 
                                                             ReturnType, 
                                                             ArgType1> >
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType);
    typedef FunctionProxyCallerInternal<FunctionType, 
                                        ReturnType, 
                                        ArgType1>       Internal;
    typedef SmartPointerContainer<Internal>             Base;
    
    FunctionProxyCallerBase (ProxyFunction proxyFunction,
                             Arg1PassType argument1) throw()
    :   Base (new Internal (proxyFunction, argument1))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

template<class FunctionType,
         class ReturnType,
         class ArgType1, 
         class ArgType2>
class FunctionProxyCallerBase<FunctionType, ReturnType, ArgType1, ArgType2>
:   public SmartPointerContainer<FunctionProxyCallerInternal<FunctionType, 
                                                             ReturnType,     
                                                             ArgType1, 
                                                             ArgType2> >
{
public:
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType);
    typedef FunctionProxyCallerInternal<FunctionType, 
                                        ReturnType, 
                                        ArgType1, 
                                        ArgType2>       Internal;
    typedef SmartPointerContainer<Internal>             Base;
    
    FunctionProxyCallerBase (ProxyFunction proxyFunction,
                             Arg1PassType argument1, 
                             Arg2PassType argument2) throw()
    :   Base (new Internal (proxyFunction, argument1, argument2))
    {
    }
    
    inline ReturnType operator()() throw()
    {
        return this->getInternal()->call();
    }    
};

//------------------------------------------------------------------------------

template<class FunctionType, int argumentCount, bool isProxy>
class FunctionCaller
{
};

//------------------------------------------------------------------------------

template<class FunctionType>
class FunctionCaller<FunctionType, 0, false>
:   public FunctionCallerBase<FunctionType, typename FunctionType::Return>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef FunctionCallerBase<FunctionType, ReturnType> Base;
    
    FunctionCaller() throw()
    {
    }    
};


template<class FunctionType>
class FunctionCaller<FunctionType, 1, false>
:   public FunctionCallerBase<FunctionType,
                              typename FunctionType::Return,
                              typename FunctionType::Arg1>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef typename FunctionType::Arg1   ArgType1;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    
    typedef FunctionCallerBase<FunctionType, 
                               ReturnType, 
                               ArgType1>    Base;
    
    FunctionCaller (Arg1PassType argument1) throw()
    :   Base (argument1)
    {
    }    
};

template<class FunctionType>
class FunctionCaller<FunctionType, 2, false>
:   public FunctionCallerBase<FunctionType,
                              typename FunctionType::Return,
                              typename FunctionType::Arg1,
                              typename FunctionType::Arg2>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef typename FunctionType::Arg1   ArgType1;
    typedef typename FunctionType::Arg2   ArgType2;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    
    typedef FunctionCallerBase<FunctionType, 
                               ReturnType, 
                               ArgType1, 
                               ArgType2>    Base;
    
    FunctionCaller (Arg1PassType argument1, 
                    Arg2PassType argument2) throw()
    :   Base (argument1, argument2)
    {
    }    
};

template<class FunctionType>
class FunctionCaller<FunctionType, 3, false>
:   public FunctionCallerBase<FunctionType,
                              typename FunctionType::Return,
                              typename FunctionType::Arg1,
                              typename FunctionType::Arg2,
                              typename FunctionType::Arg3>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef typename FunctionType::Arg1   ArgType1;
    typedef typename FunctionType::Arg2   ArgType2;
    typedef typename FunctionType::Arg3   ArgType3;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    
    typedef FunctionCallerBase<FunctionType, 
                               ReturnType, 
                               ArgType1, 
                               ArgType2, 
                               ArgType3>    Base;
    
    FunctionCaller (Arg1PassType argument1, 
                    Arg2PassType argument2,
                    Arg3PassType argument3) throw()
    :   Base (argument1, argument2, argument3)
    {
    }    
};

//------------------------------------------------------------------------------

template<class FunctionType>
class FunctionCaller<FunctionType, 0, true>
:   public FunctionProxyCallerBase<FunctionType, typename FunctionType::Return>
{
public:
    typedef typename FunctionType::Return ReturnType;    
    typedef ReturnType (*ProxyFunction) (void);
    typedef FunctionProxyCallerBase<FunctionType, ReturnType> Base;
    
    FunctionCaller (ProxyFunction proxyFunction) throw()
    :   Base (proxyFunction)
    {
    }    
};


template<class FunctionType>
class FunctionCaller<FunctionType, 1, true>
:   public FunctionProxyCallerBase<FunctionType,
                                   typename FunctionType::Return,
                                   typename FunctionType::Arg1>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef typename FunctionType::Arg1   ArgType1;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    
    typedef ReturnType (*ProxyFunction) (Arg1PassType);

    typedef FunctionProxyCallerBase<FunctionType, 
                                    ReturnType, 
                                    ArgType1>       Base;
    
    FunctionCaller (ProxyFunction proxyFunction,
                    Arg1PassType argument1) throw()
    :   Base (proxyFunction, argument1)
    {
    }    
};

template<class FunctionType>
class FunctionCaller<FunctionType, 2, true>
:   public FunctionProxyCallerBase<FunctionType,
                                   typename FunctionType::Return,
                                   typename FunctionType::Arg1,
                                   typename FunctionType::Arg2>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef typename FunctionType::Arg1   ArgType1;
    typedef typename FunctionType::Arg2   ArgType2;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType);
    
    typedef FunctionProxyCallerBase<FunctionType, 
                                    ReturnType, 
                                    ArgType1, 
                                    ArgType2>       Base;
    
    FunctionCaller (ProxyFunction proxyFunction,
                    Arg1PassType argument1, 
                    Arg2PassType argument2) throw()
    :   Base (proxyFunction, argument1, argument2)
    {
    }    
};

template<class FunctionType>
class FunctionCaller<FunctionType, 3, true>
:   public FunctionProxyCallerBase<FunctionType,
                                   typename FunctionType::Return,
                                   typename FunctionType::Arg1,
                                   typename FunctionType::Arg2,
                                   typename FunctionType::Arg3>
{
public:
    typedef typename FunctionType::Return ReturnType;
    typedef typename FunctionType::Arg1   ArgType1;
    typedef typename FunctionType::Arg2   ArgType2;
    typedef typename FunctionType::Arg3   ArgType3;
    typedef typename TypeUtility<ArgType1>::PassType Arg1PassType;
    typedef typename TypeUtility<ArgType2>::PassType Arg2PassType;
    typedef typename TypeUtility<ArgType3>::PassType Arg3PassType;
    
    typedef ReturnType (*ProxyFunction) (Arg1PassType, Arg2PassType, Arg3PassType);

    typedef FunctionProxyCallerBase<FunctionType, 
                                    ReturnType, 
                                    ArgType1, 
                                    ArgType2, 
                                    ArgType3>       Base;
    
    FunctionCaller (ProxyFunction proxyFunction,
                    Arg1PassType argument1, 
                    Arg2PassType argument2,
                    Arg3PassType argument3) throw()
    :   Base (proxyFunction, argument1, argument2, argument3)
    {
    }    
};


class Function
{
public:

    template<class ReturnType, class Arg1CallType>
    static inline FunctionCaller<FunctionContainer<ReturnType>, 0, false> 
    create() throw()
    {        
        typedef FunctionCaller<FunctionContainer<ReturnType>, 1, false> FunctionCallerType;
        return FunctionCallerType();
    }
    
    template<class ReturnType, class Arg1CallType>
    static inline FunctionCaller<FunctionContainer<ReturnType, 
                                                   typename TypeUtility<Arg1CallType>::OriginalType>, 1, false> 
    create (Arg1CallType argument1) throw()
    {
        typedef typename TypeUtility<Arg1CallType>::OriginalType ArgType1;
        
        typedef FunctionCaller<FunctionContainer<ReturnType, ArgType1>, 1, false> FunctionCallerType;
        return FunctionCallerType (argument1);
    }
    
    template<class ReturnType, class Arg1CallType, class Arg2CallType>
    static inline FunctionCaller<FunctionContainer<ReturnType, 
                                                   typename TypeUtility<Arg1CallType>::OriginalType,
                                                   typename TypeUtility<Arg2CallType>::OriginalType>, 2, false> 
    create (Arg1CallType argument1, Arg2CallType argument2) throw()
    {
        typedef typename TypeUtility<Arg1CallType>::OriginalType ArgType1;
        typedef typename TypeUtility<Arg2CallType>::OriginalType ArgType2;
        
        typedef FunctionCaller<FunctionContainer<ReturnType, ArgType1, ArgType2>, 2, false> FunctionCallerType;
        return FunctionCallerType (argument1, argument2);
    }    
    
    template<class ReturnType, class Arg1CallType, class Arg2CallType, class Arg3CallType>
    static inline FunctionCaller<FunctionContainer<ReturnType, 
                                                   typename TypeUtility<Arg1CallType>::OriginalType,
                                                   typename TypeUtility<Arg2CallType>::OriginalType,
                                                   typename TypeUtility<Arg3CallType>::OriginalType>, 3, false> 
    create (Arg1CallType argument1, Arg2CallType argument2, Arg3CallType argument3) throw()
    {
        typedef typename TypeUtility<Arg1CallType>::OriginalType ArgType1;
        typedef typename TypeUtility<Arg2CallType>::OriginalType ArgType2;
        typedef typename TypeUtility<Arg3CallType>::OriginalType ArgType3;
        
        typedef FunctionCaller<FunctionContainer<ReturnType, ArgType1, ArgType2, ArgType3>, 3, false> FunctionCallerType;
        return FunctionCallerType (argument1, argument2, argument3);
    }
    
    template<class ReturnType>
    static inline FunctionCaller<FunctionContainerProxy<ReturnType>, 0, true> 
    create (ReturnType (*proxyFunction) (void)) throw()
    {
        typedef FunctionCaller<FunctionContainerProxy<ReturnType>, 0, true> FunctionCallerType;
        return FunctionCallerType (proxyFunction);
    }
    
    template<class ReturnType, class Arg1CallType>
    static inline FunctionCaller<FunctionContainerProxy<ReturnType, 
                                                        typename TypeUtility<Arg1CallType>::OriginalType>, 1, true> 
    create (ReturnType (*proxyFunction) (typename TypeUtility<Arg1CallType>::PassType),
            Arg1CallType argument1) throw()
    {
        typedef typename TypeUtility<Arg1CallType>::OriginalType ArgType1;
        typedef FunctionCaller<FunctionContainerProxy<ReturnType, ArgType1>, 1, true> FunctionCallerType;
        return FunctionCallerType (proxyFunction, argument1);
    }
    
    template<class ReturnType, class Arg1CallType, class Arg2CallType>
    static inline FunctionCaller<FunctionContainerProxy<ReturnType, 
                                                        typename TypeUtility<Arg1CallType>::OriginalType,
                                                        typename TypeUtility<Arg2CallType>::OriginalType>, 2, true> 
    create (ReturnType (*proxyFunction) (typename TypeUtility<Arg1CallType>::PassType,
                                         typename TypeUtility<Arg2CallType>::PassType),
            Arg1CallType argument1, Arg2CallType argument2) throw()
    {
        typedef typename TypeUtility<Arg1CallType>::OriginalType ArgType1;
        typedef typename TypeUtility<Arg2CallType>::OriginalType ArgType2;

        typedef FunctionCaller<FunctionContainerProxy<ReturnType, ArgType1, ArgType2>, 2, true> FunctionCallerType;
        return FunctionCallerType (proxyFunction, argument1, argument2);
    }
    
    template<class ReturnType, class Arg1CallType, class Arg2CallType, class Arg3CallType>
    static inline FunctionCaller<FunctionContainerProxy<ReturnType, 
                                                        typename TypeUtility<Arg1CallType>::OriginalType,
                                                        typename TypeUtility<Arg2CallType>::OriginalType,
                                                        typename TypeUtility<Arg3CallType>::OriginalType>, 3, true> 
    create (ReturnType (*proxyFunction) (typename TypeUtility<Arg1CallType>::PassType,
                                         typename TypeUtility<Arg2CallType>::PassType,
                                         typename TypeUtility<Arg3CallType>::PassType),
            Arg1CallType argument1, Arg2CallType argument2, Arg3CallType argument3) throw()
    {
        typedef typename TypeUtility<Arg1CallType>::OriginalType ArgType1;
        typedef typename TypeUtility<Arg2CallType>::OriginalType ArgType2;
        typedef typename TypeUtility<Arg3CallType>::OriginalType ArgType3;
        
        typedef FunctionCaller<FunctionContainerProxy<ReturnType, ArgType1, ArgType2, ArgType3>, 3, true> FunctionCallerType;
        return FunctionCallerType (proxyFunction, argument1, argument2, argument3);
    }
    
    
};


#endif // PLONK_FUNCTION_H
