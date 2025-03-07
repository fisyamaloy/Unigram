// WARNING! All changes made in this file will be lost!
// This file was created automatically
#include "StyleBasic.hpp"

namespace
{
bool inited = false;

class Module_StyleBasic : public Style::internal::ModuleBase
{
public:
    Module_StyleBasic() { Style::internal::registerModule(this); }

    void start(int scale) override { Style::internal::init_StyleBasic(scale); }
};
Module_StyleBasic registrator;

int            _defaultFontSize      = 0;
Style::font    _defaultFont          = {Qt::Uninitialized};
Style::font    _semiboldFont         = {Qt::Uninitialized};
Style::font    _italicFont           = {Qt::Uninitialized};
Style::font    _boldFont             = {Qt::Uninitialized};
Style::font    _underlineFont        = {Qt::Uninitialized};
int            _defaultMargin        = 0;
Style::margins _defaultMargins       = {0, 0, 0, 0};
int            _minWidth             = 0;
int            _minHeight            = 0;
int            _shadowPadding        = 0;
int            _titleBarHeight       = 0;
int            _titleBarMaxIconWidth = 0;
int            _authBlockWidth       = 0;
int            _authBlockSpacing     = 0;
int            _chatWidgetMinWidth   = 0;
int            _channelListMinWidth  = 0;
int            _channelListMaxWidth  = 0;

}  // namespace

namespace st
{
const int&            defaultFontSize(_defaultFontSize);
const Style::font&    defaultFont(_defaultFont);
const Style::font&    semiboldFont(_semiboldFont);
const Style::font&    italicFont(_italicFont);
const Style::font&    boldFont(_boldFont);
const Style::font&    underlineFont(_underlineFont);
const int&            defaultMargin(_defaultMargin);
const Style::margins& defaultMargins(_defaultMargins);
const int&            minWidth(_minWidth);
const int&            minHeight(_minHeight);
const int&            shadowPadding(_shadowPadding);
const int&            titleBarHeight(_titleBarHeight);
const int&            titleBarMaxIconWidth(_titleBarMaxIconWidth);
const int&            authBlockWidth(_authBlockWidth);
const int&            authBlockSpacing(_authBlockSpacing);
const int&            chatWidgetMinWidth(_chatWidgetMinWidth);
const int&            channelListMinWidth(_channelListMinWidth);
const int&            channelListMaxWidth(_channelListMaxWidth);
}  // namespace st

namespace Style
{
namespace internal
{
namespace
{
int  px1   = 1;
int  px8   = 8;
int  px9   = 9;
int  px15  = 15;
int  px16  = 16;
int  px20  = 20;
int  px30  = 30;
int  px200 = 200;
int  px300 = 300;
int  px400 = 400;
int  px500 = 500;
int  px600 = 600;
int  px800 = 800;
void initPxValues(int scale)
{
    px1   = ConvertScale(1, scale);
    px8   = ConvertScale(8, scale);
    px9   = ConvertScale(9, scale);
    px15  = ConvertScale(15, scale);
    px16  = ConvertScale(16, scale);
    px20  = ConvertScale(20, scale);
    px30  = ConvertScale(30, scale);
    px200 = ConvertScale(200, scale);
    px300 = ConvertScale(300, scale);
    px400 = ConvertScale(400, scale);
    px500 = ConvertScale(500, scale);
    px600 = ConvertScale(600, scale);
    px800 = ConvertScale(800, scale);
}

}  // namespace

void init_StyleBasic(int scale)
{
    if (inited) return;
    inited = true;

    initPxValues(scale);

    _defaultFontSize      = px15;
    _defaultFont          = {px15, 0, 0};
    _semiboldFont         = {px15, 16, 0};
    _italicFont           = {px15, 2, 0};
    _boldFont             = {px15, 1, 0};
    _underlineFont        = {px15, 4, 0};
    _defaultMargin        = px8;
    _defaultMargins       = {px8, px8, px8, px8};
    _minWidth             = px800;
    _minHeight            = px600;
    _shadowPadding        = px1;
    _titleBarHeight       = px30;
    _titleBarMaxIconWidth = px20;
    _authBlockWidth       = px500;
    _authBlockSpacing     = px16;
    _chatWidgetMinWidth   = px400;
    _channelListMinWidth  = px200;
    _channelListMaxWidth  = px300;
}

}  // namespace internal
}  // namespace Style
