/*
Copyright (c) 2017 Patrick Rutkowski. All Rights Reserved.

This file may not be copied nor distributed without the express permission of
the author.

This file is distributed on an "AS IS" basis WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED. THE AUTHOR HEREBY DISCLAIMS ALL SUCH WARRANTIES,
INCLUDING WITHOUT LIMITATION ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
*/

#include <XKit/Unicode.hpp>
#include <string.h>
#include <wchar.h>

const char* UErrorNames[] =
{
    "UErrorNone",
    "UErrorMalformedSeq",
    "UErrorIncompleteSeq"
};

/*
The below table is sourced from here:

Unicode Standard version 6.0, D92, Page 94,
Table 3-7: Well-Formed UTF-8 Byte Sequences

Two adjacent values in this table form a numerical range, and together they
are considered one logical column in the table. Each row is terminated by
empty ranges, and this creates extra logical columns which you won't see
printed in the Unicode standard. A UTF8 code unit in a well formed sequence
must fall into the range specified in the column which it corresponds to.
*/

#define ZERO 0x00

const uint8_t UTF8ByteSeqFormats[] =
{
    0x00, 0x7F, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO,
    0xC2, 0xDF, 0x80, 0xBF, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO,
    0xE0, 0xE0, 0xA0, 0xBF, 0x80, 0xBF, ZERO, ZERO, ZERO, ZERO,
    0xE1, 0xEC, 0x80, 0xBF, 0x80, 0xBF, ZERO, ZERO, ZERO, ZERO,
    0xED, 0xED, 0x80, 0x9F, 0x80, 0xBF, ZERO, ZERO, ZERO, ZERO,
    0xEE, 0xEF, 0x80, 0xBF, 0x80, 0xBF, ZERO, ZERO, ZERO, ZERO,
    0xF0, 0xF0, 0x90, 0xBF, 0x80, 0xBF, 0x80, 0xBF, ZERO, ZERO,
    0xF1, 0xF3, 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF, ZERO, ZERO,
    0xF4, 0xF4, 0x80, 0x8F, 0x80, 0xBF, 0x80, 0xBF, ZERO, ZERO
};

#define UTF8_TABLE_COLS 5
#define UTF8_TABLE_ROWS 9

static
bool
UTF8CodeUnitInRange(uint8_t codeUnit, size_t row, size_t col)
{
    size_t pairIndex = (UTF8_TABLE_COLS * 2 * row) + (col * 2);
    uint8_t left  = UTF8ByteSeqFormats[pairIndex+0];
    uint8_t right = UTF8ByteSeqFormats[pairIndex+1];
    return codeUnit >= left && codeUnit <= right;
}

static
bool
UTF8IsEmptyRange(size_t row, size_t col)
{
    size_t pairIndex = (UTF8_TABLE_COLS * 2 * row) + (col * 2);
    uint8_t left  = UTF8ByteSeqFormats[pairIndex+0];
    uint8_t right = UTF8ByteSeqFormats[pairIndex+1];
    return left == 0x00 && right == 0x00;
}

bool
XK::UnicodeIsCodePoint(uint32_t codepoint)
{
    return codepoint <= 0x10FFFF;
}

bool
XK::UnicodeIsScalar(uint32_t scalar)
{
    return UnicodeIsCodePoint(scalar) &&
           ! UnicodeIsSurrogate(scalar);
}

int
XK::UnicodeClassifyUTF8(uint8_t byte)
{
    if( (byte & 0x80) == 0x00 )
        return 1;
    else if( (byte & 0xE0) == 0xC0 )
        return 2;
    else if( (byte & 0xF0) == 0xE0 )
        return 3;
    else if( (byte & 0xF8) == 0xF0 )
        return 4;

    return -1;
}

bool
XK::UnicodeIsContinuation(uint8_t byte)
{
    return (byte & 0xC0) == 0x80;
}

bool
XK::UnicodeIsHighSurrogate(uint32_t codepoint)
{
    return codepoint >= 0xD800 &&
           codepoint <= 0xDBFF;
}

bool
XK::UnicodeIsLowSurrogate(uint32_t codepoint)
{
    return codepoint >= 0xDC00 &&
           codepoint <= 0xDFFF;
}

bool
XK::UnicodeIsSurrogate(uint32_t codepoint)
{
    return UnicodeIsHighSurrogate(codepoint)
           || UnicodeIsLowSurrogate(codepoint);
}

size_t
XK::UnicodeUTF8Next(
    const uint8_t* buff,
    size_t index,
    size_t count)
{
    index++;

    while(true)
    {
        if( index >= count )
            return count;

        uint8_t cu = buff[index];
        int type = UnicodeClassifyUTF8(cu);

        if( type == -1 )
            index++;
        else
            break;
    }

    return index;
}

size_t
XK::UnicodeUTF8Prev(
    const uint8_t* buff,
    size_t index)
{
    index--;

    while(true)
    {
        if( index == SIZE_MAX )
            return SIZE_MAX;

        uint8_t cu = buff[index];
        int type = UnicodeClassifyUTF8(cu);

        if( type == -1 )
            index--;
        else
            break;
    }

    return index;
}

size_t
XK::UnicodeUTF16Next(
    const uint16_t* buff,
    size_t index,
    size_t count)
{
    if( index == count )
        return index;

    // If we're on the start of a code sequence then push past it
    if( ! UnicodeIsLowSurrogate(buff[index]) )
        index++;

    // Push forward until we hit the start of a sequence
    while(index < count)
    {
        if( ! UnicodeIsLowSurrogate(buff[index]) )
            break;
        index++;
    }

    return index;
}

size_t
XK::UnicodeUTF16Prev(
    const uint16_t* buff,
    size_t index,
    size_t count)
{
    // TODO: Is this OK?
    (void)count;

    if( index == 0 )
        return 0;

    // If we're on the start of a code sequence then push past it
    if( ! UnicodeIsLowSurrogate(buff[index]) )
        index--;

    // Push forward until we hit the start of a sequence
    while(index > 0)
    {
        if( ! UnicodeIsLowSurrogate(buff[index]) )
            break;
        index--;
    }

    return index;
}

size_t
XK::UnicodeUTF32Next(
    const uint32_t* buff,
    size_t index,
    size_t count)
{
    // TODO: Is this OK?
    (void)count;

    while(index < count && ! UnicodeIsScalar(buff[index]) )
        index++;
    return index;
}

size_t
XK::UnicodeUTF32Prev(
    const uint32_t* buff,
    size_t index,
    size_t count)
{
    // TODO: Is this OK?
    (void)count;

    while(index > 0 && ! UnicodeIsScalar(buff[index]) )
        index--;
    return index;
}

XK::UError
XK::UnicodeDecodeUTF8(
    uint32_t*      scalar,
    const uint8_t* codeUnits,
    size_t         codeUnitCount,
    size_t*        advance)
{
    size_t seqIndex = 0;
    uint8_t codeUnit;

    /* This is a row index into UTF8ByteSeqFormats. The column index will be
     * given by seqIndex. */
    size_t row;

    size_t remainingCodeUnits;

    codeUnit = codeUnits[seqIndex++];

    /* Each row in UTF8ByteSeqFormats represents a set of possible sequences.
     * This loop determines which row describes the sequence in "codeUnits."
     */
    for(row = 0; row < UTF8_TABLE_ROWS; row++)
    {
        if( UTF8CodeUnitInRange(codeUnit, row, 0) )
            break;
    }

    /* No matching row was found, return an error. */
    if( row == UTF8_TABLE_ROWS )
    {
        if( scalar )
            *scalar = 0xFFFD; /* REPLACEMENT_CHARACTER */

        if( advance != NULL )
            *advance = 1;

        return UErrorMalformedSeq;
    }

    /* Read in successive code units until we run out of columns in
     * UTF8ByteSeqFormats, or until we find an invalid code unit. */
    while(true)
    {
        if( UTF8IsEmptyRange(row, seqIndex) )
            break;

        remainingCodeUnits = codeUnitCount - seqIndex;
        if( remainingCodeUnits == 0 )
        {
            if( scalar )
                *scalar = 0xFFFD; /* REPLACEMENT_CHARACTER */

            if( advance )
                *advance = seqIndex;

            return UErrorIncompleteSeq;
        }

        codeUnit = codeUnits[seqIndex++];

        if( ! UTF8CodeUnitInRange(codeUnit, row, seqIndex - 1) )
        {
            if( scalar )
                *scalar = 0xFFFD; /* REPLACEMENT_CHARACTER */

            if( advance )
                *advance = seqIndex - 1;

            return UErrorMalformedSeq;
        }
    }

    if( advance )
        *advance = seqIndex;

    if( scalar == NULL )
        return UErrorNone;

    /*
    At this point seqIndex will happen to also be the length of the code unit
    sequence which we just processed. We use that length to figure out how to
    convert the sequence into a scalar. The conversion is done according to
    the table found here:

    Unicode Standard version 6.0, D92, Page 94,
    Table 3-6: UTF-8 Bit Distribution
    */

    if( seqIndex == 1 )
    {
        *scalar = codeUnits[0];
    }
    else if( seqIndex == 2 )
    {
        *scalar =
            ((uint32_t)codeUnits[0] & 0x1F) << (6*1) |
            ((uint32_t)codeUnits[1] & 0x3F) << (6*0);
    }
    else if( seqIndex == 3 )
    {
        *scalar =
            ((uint32_t)codeUnits[0] & 0x0F) << (6*2) |
            ((uint32_t)codeUnits[1] & 0x3F) << (6*1) |
            ((uint32_t)codeUnits[2] & 0x3F) << (6*0);
    }
    else if( seqIndex == 4 )
    {
        *scalar =
            ((uint32_t)codeUnits[0] & 0x07) << (6*3) |
            ((uint32_t)codeUnits[1] & 0x3F) << (6*2) |
            ((uint32_t)codeUnits[2] & 0x3F) << (6*1) |
            ((uint32_t)codeUnits[3] & 0x3F) << (6*0);
    }

    return UErrorNone;
}

XK::UError
XK::UnicodeDecodeUTF16(
    uint32_t*       scalar,
    const uint16_t* buff,
    size_t          codeUnitCount,
    size_t*         advance)
{
    if( UnicodeIsHighSurrogate(buff[0]) )
    {
        if( codeUnitCount < 2 )
        {
            if( advance )
                *advance = 1;

            return UErrorIncompleteSeq;
        }

        if( ! UnicodeIsLowSurrogate(buff[1]) )
        {
            if( advance )
                *advance = 1;

            return UErrorMalformedSeq;
        }

        /*
        The combining of two surrogates into a scalar
        is done as per the table found here:

        Unicode Standard version 6.0, D91, Page 93,
        Table 3-5: UTF-16 Bit Distribution
        */

        if( scalar != NULL )
        {
            *scalar =
                ((uint32_t)buff[0] & 0x3FF) << 10 |
                ((uint32_t)buff[1] & 0x3FF) << 0;

            *scalar += 0x10000;
        }

        if( advance != NULL )
            *advance = 2;

        return UErrorNone;
    }
    else if( UnicodeIsLowSurrogate(buff[0]) )
    {
        if( advance )
            *advance = 1;

        return UErrorMalformedSeq;
    }
    else
    {
        if( scalar != NULL )
            *scalar = buff[0];

        if( advance )
            *advance = 1;

        return UErrorNone;
    }
}

void
XK::UnicodeEncodeUTF8(
    uint32_t scalar,
    uint8_t* seq,
    size_t* seqLength)
{
    /*
    The combining of multiple code units into a
    scalar is done as per the table found here:

    Unicode Standard version 6.0, D92, Page 94,
    Table 3-6: UTF-8 Bit Distribution
    */

    if( ! UnicodeIsScalar(scalar) )
    {
        UnicodeEncodeUTF8(0xFFFD, seq, seqLength);
    }
    else if( scalar <= 0x7F )
    {
        if( seq != NULL )
            seq[0] = (uint8_t)scalar;

        if( seqLength != NULL )
            *seqLength = 1;
    }
    else if( scalar <= 0x7FF )
    {
        if( seq != NULL )
        {
            seq[0] = 0xC0 | ((scalar >> 6) & 0x1F);
            seq[1] = 0x80 | ((scalar >> 0) & 0x3F);
        }

        if( seqLength != NULL )
            *seqLength = 2;
    }
    else if( scalar <= 0xFFFF )
    {
        if( seq != NULL )
        {
            seq[0] = 0xE0 | ((scalar >> 12) & 0x0F);
            seq[1] = 0x80 | ((scalar >>  6) & 0x3F);
            seq[2] = 0x80 | ((scalar >>  0) & 0x3F);
        }

        if( seqLength != NULL )
            *seqLength = 3;
    }
    else if( scalar <= 0x10FFFF )
    {
        if( seq != NULL )
        {
            seq[0] = 0xF0 | ((scalar >> 18) & 0x07);
            seq[1] = 0x80 | ((scalar >> 12) & 0x3F);
            seq[2] = 0x80 | ((scalar >>  6) & 0x3F);
            seq[3] = 0x80 | ((scalar >>  0) & 0x3F);
        }

        if( seqLength != NULL )
            *seqLength = 4;
    }
}

void
XK::UnicodeEncodeUTF16(
    uint32_t scalar,
    uint16_t* seq,
    size_t* seqLength)
{
    if( ! UnicodeIsScalar(scalar) )
    {
        UnicodeEncodeUTF16(0xFFFD, seq, seqLength);
    }
    else if( scalar <= 0xFFFF )
    {
        if( seq != NULL )
            seq[0] = (uint16_t)scalar;

        if( seqLength )
            *seqLength = 1;
    }
    else if( scalar <= 0x10FFFF )
    {
        if( seq != NULL )
        {
            scalar -= 0x10000;
            seq[0] = 0xD800 | ((scalar >> 10) & 0x3FF);
            seq[1] = 0xDC00 | ((scalar >>  0) & 0x3FF);
        }

        if( seqLength )
            *seqLength = 2;
    }
}

size_t
XK::UnicodeConvertedLength(
    const void* input,
    size_t codeUnitCount,
    UForm inputForm,
    UForm targetForm)
{
    size_t result = 0;

    const uint8_t*  input8  = (uint8_t*)input;
    const uint16_t* input16 = (uint16_t*)input;
    const uint32_t* input32 = (uint32_t*)input;

    size_t inputIndex = 0;

    uint32_t scalar;
    size_t advance;
    size_t seqLength = 0;

    while( inputIndex < codeUnitCount )
    {
        size_t remainingCodeUnits = codeUnitCount - inputIndex;

        /* Decode a scalar */
        if( inputForm == UFormUTF8 )
        {
            UnicodeDecodeUTF8(
                &scalar,
                &input8[inputIndex],
                remainingCodeUnits,
                &advance);

            inputIndex += advance;
        }
        else if( inputForm == UFormUTF16 )
        {
            UnicodeDecodeUTF16(
                &scalar,
                &input16[inputIndex],
                remainingCodeUnits,
                &advance);

            inputIndex += advance;
        }
        else
        {
            scalar = input32[inputIndex++];

            if( ! UnicodeIsScalar(scalar) )
                scalar = 0xFFFD;
        }

        /* Measure what the decoded scalar's length would be in the
         * target encoding form. */

        if( targetForm == UFormUTF8 ) {
            UnicodeEncodeUTF8(scalar, NULL, &seqLength);
        }
        else if( targetForm == UFormUTF16 ) {
            UnicodeEncodeUTF16(scalar, NULL, &seqLength);
        }
        else {
            seqLength = 1;
        }

        result += seqLength;
    }

    return result;
}

#include <stdlib.h>

XK::Data
XK::UnicodeConvert(
    const void* input,
    size_t codeUnitCount,
    UForm inputForm,
    UForm outputForm,
    bool* isBMPOnly)
{
    const uint8_t*  input8  = (uint8_t*)input;
    const uint16_t* input16 = (uint16_t*)input;
    const uint32_t* input32 = (uint32_t*)input;

    size_t inputIndex = 0;

    uint32_t scalar;
    size_t advance;
    size_t seqLength;

    Data output;

    if( isBMPOnly )
        *isBMPOnly = true;

    if( codeUnitCount == (size_t)-1 )
        codeUnitCount = 1 + UnicodeStrlen(input, inputForm);

    while( inputIndex < codeUnitCount )
    {
        size_t remainingCodeUnits = codeUnitCount - inputIndex;

        /* Decode a scalar */
        if( inputForm == UFormUTF8 )
        {
            UnicodeDecodeUTF8(
                &scalar,
                &input8[inputIndex],
                remainingCodeUnits,
                &advance);

            inputIndex += advance;
        }
        else if( inputForm == UFormUTF16 )
        {
            UnicodeDecodeUTF16(
                &scalar,
                &input16[inputIndex],
                remainingCodeUnits,
                &advance);

            inputIndex += advance;
        }
        else
        {
            scalar = input32[inputIndex++];

            if( ! UnicodeIsScalar(scalar) )
                scalar = 0xFFFD;
        }

        if( isBMPOnly && scalar > 0xFFFF )
            *isBMPOnly = false;

        /* Encode the scalar into the desired output form. */
        if( outputForm == UFormUTF8 )
        {
            uint8_t seq[4];
            UnicodeEncodeUTF8(scalar, seq, &seqLength);
            output.append(seq, seqLength);
        }
        else if( outputForm == UFormUTF16 )
        {
            uint16_t seq[2];
            UnicodeEncodeUTF16(scalar, seq, &seqLength);
            output.append(seq, 2 * seqLength);
        }
        else if( outputForm == UFormUTF32 )
        {
            output.append(&scalar, 4);
        }
    }

    return output;
}

/*
void*
XK::UTF8ToUTF16(const void* input)
{
    return UnicodeConvert(
        input, -1, UFormUTF8, UFormUTF16);
}

void*
XK::UTF8ToUTF32(const void* input)
{
    return UnicodeConvert(
        input, -1, UFormUTF8, UFormUTF32);
}

void*
XK::UTF16ToUTF8(const void* input)
{
    return UnicodeConvert(
        input, -1, UFormUTF16, UFormUTF8);
}

void*
XK::UTF16ToUTF32(const void* input)
{
    return UnicodeConvert(
        input, -1, UFormUTF16, UFormUTF32);
}

void*
XK::UTF32ToUTF8(const void* input)
{
    return UnicodeConvert(
        input, -1, UFormUTF32, UFormUTF8);
}

void*
XK::UTF32ToUTF16(const void* input)
{
    return UnicodeConvert(
        input, -1, UFormUTF32, UFormUTF16);
}
*/

size_t
XK::UnicodeStrlen(const void* str, UForm form)
{
    size_t len = 0;

    if( form == UFormUTF8 )
    {
        len = strlen((const char*)str);
    }
    else if( form == UFormUTF16 )
    {
        if( sizeof(wchar_t) == 2 )
            return wcslen((const wchar_t*)str);

        const uint16_t* start = (const uint16_t*)str;
        const uint16_t* end = (const uint16_t*)str;

        while( *end )
            end++;

        len = end - start;
    }
    else if( form == UFormUTF32 )
    {
        if( sizeof(wchar_t) == 4 )
            return wcslen((const wchar_t*)str);

        const uint32_t* start = (const uint32_t*)str;
        const uint32_t* end = (const uint32_t*)str;

        while( *end )
            end++;

        len = end - start;
    }

    return len;
}

int
XK::UnicodeStrcmp(const void* avp, const void* bvp, UForm form)
{
    if( form == UFormUTF8 )
    {
        return strcmp((const char*)avp, (const char*)bvp);
    }
    else if( form == UFormUTF16 )
    {
        const uint16_t* a = (const uint16_t*)avp;
        const uint16_t* b = (const uint16_t*)bvp;

        while( *a == *b )
        {
            a++;
            b++;

            if( *a == 0 )
                return 0;
        }

        if( *a < *b )
            return -1;
        else
            return +1;
    }
    else if( form == UFormUTF32 )
    {
        const uint32_t* a = (const uint32_t*)avp;
        const uint32_t* b = (const uint32_t*)bvp;

        while( *a == *b )
        {
            a++;
            b++;

            if( *a == 0 )
                return 0;
        }

        if( *a < *b )
            return -1;
        else
            return +1;
    }

    return 0;
}
