/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#ifndef PLONK_JSON_H
#define PLONK_JSON_H


class JSON
{
public:
    inline JSON() throw() : json (0) { }
    inline JSON (const int value) throw() : json (pl_JSON_Int (value)) { }
    inline JSON (const float value) throw() : json (pl_JSON_Float (value)) { }
    inline JSON (const double value) throw() : json (pl_JSON_Double (value)) { }
    inline JSON (IntArray const& values) throw() : json (pl_JSON_IntArray (values.getArray(), values.length())) { }
    inline JSON (FloatArray const& values) throw() : json (pl_JSON_FloatArray (values.getArray(), values.length())) { }
    inline JSON (DoubleArray const& values) throw() : json (pl_JSON_DoubleArray (values.getArray(), values.length())) { }
    inline JSON (Text const& text) throw() : json (pl_JSON_String (text.getArray())) { }
    inline JSON (const char* text) throw() : json (pl_JSON_String (text)) { }
    
    inline JSON (const int value, const bool useBinary) throw()
    : json (useBinary ? pl_JSON_IntBinary (value) : pl_JSON_Int (value))
    { }
    
    inline JSON (const float value, const bool useBinary) throw()
    : json (useBinary ? pl_JSON_FloatBinary (value) : pl_JSON_Float (value))
    { }
    
    inline JSON (const double value, const bool useBinary) throw()
    : json (useBinary ? pl_JSON_DoubleBinary (value) : pl_JSON_Double (value))
    { }
    
    inline JSON (IntArray const& values, const bool useBinary) throw()
    : json (useBinary ? pl_JSON_IntArrayBinary (values.getArray(), values.length()) : pl_JSON_IntArray (values.getArray(), values.length()))
    { }
    
    inline JSON (FloatArray const& values, const bool useBinary) throw()
    : json (useBinary ? pl_JSON_FloatArrayBinary (values.getArray(), values.length()) : pl_JSON_FloatArray (values.getArray(), values.length()))
    { }
    
    inline JSON (DoubleArray const& values, const bool useBinary) throw()
    : json (useBinary ? pl_JSON_DoubleArrayBinary (values.getArray(), values.length()) : pl_JSON_DoubleArray (values.getArray(), values.length()))
    { }

    inline JSON (BinaryFile const& file) throw()
    : json (0)
    {
        if (file.canRead())
        {
            json = pl_JSON_FromFile (file->getInternal()->getPeerRef());
        }
    }
    
    static JSON array() throw() { return JSON (pl_JSON_Array()); }
    static JSON object() throw() { return JSON (pl_JSON_Object()); }
    static JSON null() throw() { return JSON (pl_JSON_Null()); }
    
    inline JSON (JSON const& other) throw()
    : json (pl_JSON_IncrementRefCount (other.json))
    {
    }
    
    inline JSON& operator= (JSON const& other) throw()
    {
        if (this != &other)
        {
            PlankJSONRef temp = pl_JSON_IncrementRefCount (other.json);
            pl_JSON_DecrementRefCount (json);
            json = temp;
        }
        
        return *this;
    }

    inline ~JSON()
    {
        pl_JSON_DecrementRefCount (json);
    }
    
    inline ResultCode toFile (BinaryFile const& file) throw()
    {        
        return file.canWrite() ? pl_JSON_WriteToFile (json, file.getInternal()->getPeerRef()) : PlankResult_FileWriteError;
    }
    
    inline bool isEmpty() const throw() { return json == 0; }
    inline bool isInt() const throw() { return pl_JSON_IsInt (json); }
    inline bool isFloat() const throw() { return pl_JSON_IsFloat (json); }
    inline bool isDouble() const throw() { return pl_JSON_IsDouble (json); }
    inline bool isBool() const throw() { return pl_JSON_IsBool (json); }
    inline bool isNull() const throw() { return pl_JSON_IsNull (json); }
    inline bool isArray() const throw() { return pl_JSON_IsArray (json); }
    inline bool isObject() const throw() { return pl_JSON_IsObject (json); }
    inline bool isString() const throw() { return pl_JSON_IsString (json); }
    inline bool isIntArrayEncoded() const throw() { return pl_JSON_IsIntArrayEncoded (json); }
    inline bool isFloatArrayEncoded() const throw() { return pl_JSON_IsFloatArrayEncoded (json); }
    inline bool isDoubleArrayEncoded() const throw() { return pl_JSON_IsDoubleArrayEncoded (json); }
    inline bool isError() const throw() { return pl_JSON_IsError (json); }

    inline bool operator== (JSON const& other) const throw() { return json == other.json; }
    inline bool operator!= (JSON const& other) const throw() { return json != other.json; }
        
    operator int () const throw() { return pl_JSON_IntGet (json); }
    operator PlankUI () const throw() { return PlankUI (pl_JSON_IntGet (json)); }
    operator float () const throw() { return pl_JSON_FloatGet (json); }
    operator double () const throw() { return pl_JSON_DoubleGet (json); }
    operator Text () const throw() { return pl_JSON_StringGet (json); }
    operator const char* () const throw() { return pl_JSON_StringGet (json); }
       
    int getInt() const throw() { return pl_JSON_IntGet (json); }
    UnsignedInt getUnsignedInt() const throw() { return PlankUI (pl_JSON_IntGet (json)); }
    float getFloat() const throw() { return pl_JSON_FloatGet (json); }
    double getDouble() const throw() { return pl_JSON_DoubleGet (json); }
    Text getText() const throw() { return pl_JSON_StringGet (json); }
    const char* getCString() const throw() { return pl_JSON_StringGet (json); }
                
    operator IntArray () const throw() { return getIntArray();  }
    operator FloatArray () const throw() { return getFloatArray(); }
    operator DoubleArray () const throw() { return getDoubleArray(); }

    IntArray getIntArray() const throw()
    {
        if (isIntArrayEncoded() || isArray())
        {
            PlankDynamicArray array;
            pl_DynamicArray_Init (&array);
            
            if (pl_JSON_IntArrayGet (json, &array) == PlankResult_OK)
            {
                IntArray values ((int)pl_DynamicArray_GetSize (&array),
                                 static_cast<int*> (pl_DynamicArray_GetArray (&array)),
                                 false, // not null terminated
                                 true); // shouldTakeOwnership
                return values;
            }
            else
            {
                pl_DynamicArray_DeInit (&array);
            }
        }
        
        return IntArray::getNull();
    }
    
    FloatArray getFloatArray() const throw()
    {
        if (isFloatArrayEncoded() || isArray())
        {
            PlankDynamicArray array;
            pl_DynamicArray_Init (&array);
            
            if (pl_JSON_FloatArrayGet (json, &array) == PlankResult_OK)
            {
                FloatArray values ((int)pl_DynamicArray_GetSize (&array),
                                   static_cast<float*> (pl_DynamicArray_GetArray (&array)),
                                   false, // not null terminated
                                   true); // shouldTakeOwnership
                return values;
            }
            else
            {
                pl_DynamicArray_DeInit (&array);
            }
        }
        
        return FloatArray::getNull();
    }
    
    DoubleArray getDoubleArray() const throw()
    {
        if (isDoubleArrayEncoded() || isArray())
        {
            PlankDynamicArray array;
            pl_DynamicArray_Init (&array);
            
            if (pl_JSON_DoubleArrayGet (json, &array) == PlankResult_OK)
            {
                DoubleArray values ((int)pl_DynamicArray_GetSize (&array),
                                    static_cast<double*> (pl_DynamicArray_GetArray (&array)),
                                    false, // not null terminated
                                    true); // shouldTakeOwnership
                return values;
            }
            else
            {
                pl_DynamicArray_DeInit (&array);
            }
        }
        
        return DoubleArray::getNull();
    }

    inline JSON operator[] (const int index) const throw()
    {
        return JSON (pl_JSON_ArrayAt (json, index));
    }

    inline JSON operator[] (const Long index) const throw()
    {
        return JSON (pl_JSON_ArrayAt (json, index));
    }

    inline JSON operator[] (const char* key) const throw()
    {
        return JSON (pl_JSON_ObjectAtKey (json, key));
    }

    inline JSON& put (const Long index, JSON const& item) throw()
    {
        if (isArray())
        {
            pl_JSON_ArrayPut (json, index, pl_JSON_IncrementRefCount (item.json));
        }
        
        return *this;
    }
        
    inline JSON& add (JSON const& item) throw()
    {
        if (isArray())
        {
            pl_JSON_ArrayAppend (json, pl_JSON_IncrementRefCount (item.json));
        }
        
        return *this;
    }

    inline JSON& add (const char* key, JSON const& item) throw()
    {
        if (isObject())
        {
            pl_JSON_ObjectPutKey (json, key, pl_JSON_IncrementRefCount (item.json));
        }
        
        return *this;
    }

    inline Long length() const throw()
    {
        if (isArray())
        {
            return pl_JSON_ArrayGetSize (json);
        }
        else if (isObject())
        {            
            return pl_JSON_ObjectGetSize (json);
        }
        else if (isString())
        {
            return strlen (pl_JSON_StringGet (json));
        }
        else if (!json || isNull())
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
  
    static JSON versionString (const UnsignedChar ex, const UnsignedChar major, const UnsignedChar minor, const UnsignedChar micro) throw()
    {
        return pl_JSON_VersionString (ex, major, minor, micro);
    }
        
    static UnsignedInt versionCode (const UnsignedChar ex, const UnsignedChar major, const UnsignedChar minor, const UnsignedChar micro) throw()
    {
        return pl_JSON_VersionCode (ex, major, minor, micro);
    }
        
    UnsignedInt getVersion() const throw()
    {
        return pl_JSON_ObjectGetVersion (json);
    }
        
    void setType (const char* type) throw()
    {
        pl_JSON_ObjectSetType (json, type);
    }
        
    void setVersionString (const UnsignedChar ex, const UnsignedChar major, const UnsignedChar minor, const UnsignedChar micro) throw()
    {
        pl_JSON_ObjectSetVersionString (json, ex, major, minor, micro);
    }
        
    void setVersionCode (const UnsignedChar ex, const UnsignedChar major, const UnsignedChar minor, const UnsignedChar micro) throw()
    {
        pl_JSON_ObjectSetVersionCode (json, ex, major, minor, micro);
    }
        
    bool isObjectType (const char* type) const throw()
    {
        return pl_JSON_IsObjectType (json, type);
    }
        
    Text dump() const throw()
    {
        return json_dumps ((json_t*)json, 0);
    }
        
    PlankJSONRef getInternal() throw() { return json; }
    const PlankJSONRef getInternal() const throw() { return json; }
        
private:
    inline JSON (PlankJSONRef other) throw()
    : json (pl_JSON_IncrementRefCount (other))
    {
    }
        
    PlankJSONRef json;
};


#endif // PLONK_JSON_H
