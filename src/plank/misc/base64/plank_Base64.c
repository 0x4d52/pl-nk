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

#include "../../core/plank_StandardHeader.h"
#include "plank_Base64.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>


//static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
//    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
//    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
//    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
//    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
//    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
//    'w', 'x', 'y', 'z', '0', '1', '2', '3',
//    '4', '5', '6', '7', '8', '9', '+', '/'};
//static char *decoding_table = NULL;
//static int mod_table[] = {0, 2, 1};
//
//
//char *base64_encode(const unsigned char *data,
//                    size_t input_length,
//                    size_t *output_length) {
//    
//    *output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));
//    
//    char *encoded_data = malloc(*output_length);
//    if (encoded_data == NULL) return NULL;
//    
//    for (int i = 0, j = 0; i < input_length;) {
//        
//        uint32_t octet_a = i < input_length ? data[i++] : 0;
//        uint32_t octet_b = i < input_length ? data[i++] : 0;
//        uint32_t octet_c = i < input_length ? data[i++] : 0;
//        
//        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
//        
//        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
//        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
//        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
//        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
//    }
//    
//    for (int i = 0; i < mod_table[input_length % 3]; i++)
//        encoded_data[*output_length - 1 - i] = '=';
//    
//    return encoded_data;
//}
//
//
//unsigned char *base64_decode(const char *data,
//                             size_t input_length,
//                             size_t *output_length) {
//    
//    if (decoding_table == NULL) build_decoding_table();
//    
//    if (input_length % 4 != 0) return NULL;
//    
//    *output_length = input_length / 4 * 3;
//    if (data[input_length - 1] == '=') (*output_length)--;
//    if (data[input_length - 2] == '=') (*output_length)--;
//    
//    unsigned char *decoded_data = malloc(*output_length);
//    if (decoded_data == NULL) return NULL;
//    
//    for (int i = 0, j = 0; i < input_length;) {
//        
//        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        
//        uint32_t triple = (sextet_a << 3 * 6)
//        + (sextet_b << 2 * 6)
//        + (sextet_c << 1 * 6)
//        + (sextet_d << 0 * 6);
//        
//        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
//        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
//        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
//    }
//    
//    return decoded_data;
//}
//
//
//void build_decoding_table() {
//    
//    decoding_table = malloc(256);
//    
//    for (int i = 0; i < 0x40; i++)
//        decoding_table[encoding_table[i]] = i;
//}
//
//
//void base64_cleanup() {
//    free(decoding_table);
//}
