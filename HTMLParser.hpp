#ifndef WK_HTMLPARSER_HPP
#define WK_HTMLPARSER_HPP

#include <XKit/String.hpp>
#include <XKit/Source.hpp>

namespace WK
{

class HTMLParser
{
public:
    typedef void (*StartCallback)(
        const XK::String& tagName,
        void* ctx);

    typedef void (*EndCallback)(
        const XK::String& tagName,
        void* ctx);

    typedef void (*AttribCallback)(
        const XK::String& name,
        const XK::String& value,
        void* ctx);

    typedef void (*HookCallback)(
        const XK::String& name,
        void* ctx);

    typedef void (*TextCallback)(
        const XK::String& text,
        void* ctx);

    typedef void (*EntityCallback)(
        const XK::String& text,
        void* ctx);

    typedef void (*MacroTagCallback)(
        const XK::String& name,
        void* ctx);

    typedef void (*ArgTagCallback)(
        int num,
        void* ctx);

    typedef void (*StartGroupCallback)(
        void* ctx);

    typedef void (*EndGroupCallback)(
        void* ctx);

    typedef void (*StartMathCallback)(
        bool isDisplay,
        void* ctx);

    typedef void (*EndMathCallback)(
        bool isDisplay,
        void* ctx);

    typedef void (*MathTextCallback)(
        const XK::String& str,
        void* ctx);

private:

    XK::Source* mSource;
    void* mCTX;

    StartCallback mStartCallback;
    EndCallback mEndCallback;
    AttribCallback mAttribCallback;
    TextCallback mTextCallback;
    HookCallback mHookCallback;
    EntityCallback mEntityCallback;
    MacroTagCallback mMacroTagCallback;
    ArgTagCallback mArgTagCallback;
    StartGroupCallback mStartGroupCallback;
    EndGroupCallback mEndGroupCallback;
    StartMathCallback mStartMathCallback;
    EndMathCallback mEndMathCallback;
    MathTextCallback mMathTextCallback;

private:

    XK::String getAttrValue();

    void getEntity();
    void getItem();
    void getTag();
    void getHook();
    void getStartTag();
    void getEndTag();
    void getText();
    void getMacroTag();
    void getArgTag();
    void getMathNode();

public:

    HTMLParser();

    void setStartCallback(StartCallback cb);
    void setEndCallback(EndCallback cb);
    void setAttribCallback(AttribCallback cb);
    void setTextCallback(TextCallback cb);
    void setHookCallback(HookCallback cb);
    void setEntityCallback(EntityCallback cb);
    void setMacroTagCallback(MacroTagCallback cb);
    void setArgTagCallback(ArgTagCallback cb);

    void setStartGroupCallback(StartGroupCallback cb);
    void setEndGroupCallback(EndGroupCallback cb);

    void setStartMathCallback(StartMathCallback cb);
    void setEndMathCallback(EndMathCallback cb);
    void setMathTextCallback(MathTextCallback cb);

    void parse(void* ctx, XK::Source* src);
};

}

#endif /* WK_HTMLPARSER_HPP */
