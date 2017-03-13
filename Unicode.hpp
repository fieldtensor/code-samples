/*
Copyright (c) 2017 Patrick Rutkowski. All Rights Reserved.

This file may not be copied nor distributed without the express permission of
the author.

This file is distributed on an "AS IS" basis WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED. THE AUTHOR HEREBY DISCLAIMS ALL SUCH WARRANTIES,
INCLUDING WITHOUT LIMITATION ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
*/

#ifndef XKIT_UNICODE_HPP
#define XKIT_UNICODE_HPP

namespace XK
{
    enum UError
    {
        UErrorNone,
        UErrorMalformedSeq,
        UErrorIncompleteSeq
    };

    enum UForm
    {
        UFormUTF8  = 1,
        UFormUTF16 = 2,
        UFormUTF32 = 4
    };
}

#include <XKit/Data.hpp>
#include <stdint.h>
#include <stddef.h>

namespace XK
{

extern const char* UErrorNames[];

bool UnicodeIsCodePoint(uint32_t codepoint);
bool UnicodeIsScalar(uint32_t scalar);

int UnicodeClassifyUTF8(uint8_t byte);
bool UnicodeIsContinuation(uint8_t byte);

bool UnicodeIsHighSurrogate(uint32_t codepoint);
bool UnicodeIsLowSurrogate(uint32_t codepoint);
bool UnicodeIsSurrogate(uint32_t codepoint);

int UnicodeClassifyUTF8(uint8_t codeunit);

size_t UnicodeUTF8Next(const uint8_t* buff, size_t index, size_t count);
size_t UnicodeUTF8Prev(const uint8_t* buff, size_t index);
size_t UnicodeUTF16Next(const uint16_t* buff, size_t index, size_t count);
size_t UnicodeUTF16Prev(const uint16_t* buff, size_t index, size_t count);
size_t UnicodeUTF32Next(const uint32_t* buff, size_t index, size_t count);
size_t UnicodeUTF32Prev(const uint32_t* buff, size_t index, size_t count);

UError
UnicodeDecodeUTF8(
    uint32_t*      scalar,
    const uint8_t* codeUnits,
    size_t         codeUnitCount,
    size_t*        advance);

UError
UnicodeDecodeUTF16(
    uint32_t*       scalar,
    const uint16_t* codeUnits,
    size_t          codeUnitCount,
    size_t*         advance);

void
UnicodeEncodeUTF8(
    uint32_t scalar, uint8_t* seq, size_t* seqLength);

void
UnicodeEncodeUTF16(
    uint32_t scalar, uint16_t* seq, size_t* seqLength);

size_t
UnicodeConvertedLength(
    const void* input,
    size_t codeUnitCount,
    UForm inputForm,
    UForm targetForm);

Data
UnicodeConvert(
    const void* input,
    size_t codeUnitCount,
    UForm inputForm,
    UForm outputForm,
    bool* isBMPOnly = NULL);

/*
void* UTF8ToUTF16(const void* input);
void* UTF8ToUTF32(const void* input);
void* UTF16ToUTF8(const void* input);
void* UTF16ToUTF32(const void* input);
void* UTF32ToUTF8(const void* input);
void* UTF32ToUTF16(const void* input);
*/

size_t UnicodeStrlen(const void* str, UForm form);
int UnicodeStrcmp(const void* a, const void* b, UForm form);

}

#endif /* XKIT_UNICODE_HPP */
