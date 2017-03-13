#include <WK/HTMLParser.hpp>
#include <WK/Util.hpp>
#include <stdio.h>

using namespace XK;

namespace WK
{

HTMLParser::HTMLParser()
{
    mSource = nullptr;

    mStartCallback = nullptr;
    mEndCallback = nullptr;
    mAttribCallback = nullptr;
    mTextCallback = nullptr;
    mHookCallback = nullptr;
    mEntityCallback = nullptr;
    mMacroTagCallback = nullptr;
    mArgTagCallback = nullptr;
    mStartGroupCallback = nullptr;
    mEndGroupCallback = nullptr;
    mStartMathCallback = nullptr;
    mEndMathCallback = nullptr;
    mMathTextCallback = nullptr;
}

void
HTMLParser::setStartCallback(StartCallback cb)
{
    mStartCallback = cb;
}

void
HTMLParser::setEndCallback(EndCallback cb)
{
    mEndCallback = cb;
}

void
HTMLParser::setAttribCallback(AttribCallback cb)
{
    mAttribCallback = cb;
}

void
HTMLParser::setTextCallback(TextCallback cb)
{
    mTextCallback = cb;
}

void
HTMLParser::setHookCallback(HookCallback cb)
{
    mHookCallback = cb;
}

void
HTMLParser::setEntityCallback(EntityCallback cb)
{
    mEntityCallback = cb;
}

void
HTMLParser::setMacroTagCallback(MacroTagCallback cb)
{
    mMacroTagCallback = cb;
}

void
HTMLParser::setArgTagCallback(ArgTagCallback cb)
{
    mArgTagCallback = cb;
}

void
HTMLParser::setStartGroupCallback(StartGroupCallback cb)
{
    mStartGroupCallback = cb;
}

void
HTMLParser::setEndGroupCallback(EndGroupCallback cb)
{
    mEndGroupCallback = cb;
}

void
HTMLParser::setStartMathCallback(StartMathCallback cb)
{
    mStartMathCallback = cb;
}

void
HTMLParser::setEndMathCallback(EndMathCallback cb)
{
    mEndMathCallback = cb;
}

void
HTMLParser::setMathTextCallback(MathTextCallback cb)
{
    mMathTextCallback = cb;
}

String
HTMLParser::getAttrValue()
{
    String ret;

    mSource->eatHorizontalSpaces();

    uint32_t maybeQuote = mSource->lookChar();

    bool hasDQ = (maybeQuote == '"');
    bool hasSQ = (maybeQuote == '\'');

    if( hasDQ || hasSQ )
        mSource->getChar();

    while(true)
    {
        uint32_t ch = mSource->lookChar();

        if( hasDQ )
        {
            if( ch == '"' )
            {
                mSource->getChar();
                return ret;
            }
        }
        else if( hasSQ )
        {
            if( ch == '\'' )
            {
                mSource->getChar();
                return ret;
            }
        }
        else
        {
            bool isEndChar =
                ch == 0 || ch == '>' || (ch <= 0x7F && isspace(ch));

            if( isEndChar )
                return ret;
        }

        ret += mSource->getChar();
    }

    return ret;
}

void
HTMLParser::getEndTag()
{
    static RegExp w("\\w+");

    mSource->getChar();
    mSource->getChar();

    String tagName;

    mSource->eatHorizontalSpaces();
    mSource->scan(tagName, w);
    mSource->eatHorizontalSpaces();

    if( mEndCallback )
        mEndCallback(tagName, mCTX);

    if( mSource->lookChar() == '>' )
        mSource->getChar();
}

void
HTMLParser::getStartTag()
{
    static RegExp w("\\w+");

    mSource->getChar();

    String tagName;
    mSource->scan(tagName, w);

    if( mStartCallback )
        mStartCallback(tagName, mCTX);

    while(true)
    {
        uint32_t ch = mSource->lookChar();
        if( ch == '>' )
        {
            mSource->getChar();
            break;
        }

        String attrName;

        mSource->eatSpaces();
        mSource->scan(attrName, w);
        mSource->eatSpaces();

        ch = mSource->lookChar();
        if( ch == '=' )
        {
            mSource->getChar();

            mSource->eatSpaces();
            String attrValue = getAttrValue();
            mSource->eatSpaces();

            if( mAttribCallback )
                mAttribCallback(attrName, attrValue, mCTX);
        }
        else
        {
            if( mAttribCallback )
                mAttribCallback(attrName, "", mCTX);
        }
    }

    if( IsVoidTag(tagName) && mEndCallback )
        mEndCallback(tagName, mCTX);
}

void
HTMLParser::getHook()
{
    mSource->getChar();
    mSource->getChar();

    String name;
    mSource->scan(name, "[^>]*");

    if( mHookCallback )
        mHookCallback(name, mCTX);

    if( mSource->lookChar() == '>' )
        mSource->getChar();

    if( mSource->lookChar() == '>' )
        mSource->getChar();
}

void
HTMLParser::getTag()
{
    uint32_t ch = mSource->lookChar(1);
    if( ch == '/' )
        getEndTag();
    else if( ch == '<' )
        getHook();
    else
        getStartTag();
}

void
HTMLParser::getEntity()
{
    String value;

    mSource->getChar();
    mSource->scan(value, "[^\\s;]*");
    if( mSource->lookChar() == ';' )
        mSource->getChar();

    if( mEntityCallback )
        mEntityCallback(value, mCTX);
}

void
HTMLParser::getText()
{
    static RegExp re("[^$\\{\\}#<&\\\\]*");

    String text;
    mSource->scan(text, re);
    if( mTextCallback )
        mTextCallback(text, mCTX);
}

void
HTMLParser::getMacroTag()
{
    static RegExp w("\\w+");

    String name;

    mSource->getChar();
    mSource->scan(name, w);

    if( mMacroTagCallback )
        mMacroTagCallback(name, mCTX);
}

void
HTMLParser::getArgTag()
{
    int64_t num;

    mSource->getChar();
    mSource->scanInteger(&num);

    if( mArgTagCallback )
        mArgTagCallback(num, mCTX);
}

void
HTMLParser::getMathNode()
{
    String text;

    bool isDisplay = false;

    mSource->getChar();
    uint32_t ch = mSource->lookChar();

    if( ch == '$' )
    {
        isDisplay = true;
        mSource->getChar();
    }

    if( mStartMathCallback )
        mStartMathCallback(isDisplay, mCTX);

    bool inEscape = false;
    while(true)
    {
        uint32_t ch = mSource->getChar();

        if( inEscape )
        {
            if( ch != '$' )
                text += '\\';
            text += ch;
            inEscape = false;
            continue;
        }

        if( ch == '\\' )
        {
            inEscape = true;
            continue;
        }

        if( ch == '%' )
        {
            int64_t x;
            if( mSource->scanInteger(&x) )
            {
                if( mMathTextCallback )
                    mMathTextCallback(text, mCTX);

                if( mArgTagCallback )
                    mArgTagCallback(x, mCTX);

                text = "";
                continue;
            }
        }
        else if( ch == '$' )
        {
            if( isDisplay && mSource->lookChar() == '$' )
                mSource->getChar();
            break;
        }

        text += ch;
    }

    if( mMathTextCallback )
        mMathTextCallback(text, mCTX);

    if( mEndMathCallback )
        mEndMathCallback(isDisplay, mCTX);
}

void
HTMLParser::getItem()
{
    uint32_t ch = mSource->lookChar();

    if( ch == '<' ) {
        getTag();
    }
    else if( ch == '&' ) {
        getEntity();
    }
    else if( ch == '\\' ) {
        getMacroTag();
    }
    else if( ch == '#' ) {
        getArgTag();
    }
    else if( ch == '{' )
    {
        mSource->getChar();
        if( mStartGroupCallback )
            mStartGroupCallback(mCTX);
    }
    else if( ch == '}' )
    {
        mSource->getChar();
        if( mEndGroupCallback )
            mEndGroupCallback(mCTX);
    }
    else if( ch == '$' )
    {
        getMathNode();
    }
    else
    {
        getText();
    }
}

void
HTMLParser::parse(void* ctx, Source* src)
{
    mSource = src;
    mCTX = ctx;

    while( mSource->lookChar() )
        getItem();
}

}
