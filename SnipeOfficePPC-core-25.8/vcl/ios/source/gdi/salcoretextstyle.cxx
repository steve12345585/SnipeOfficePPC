/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "ios/common.h"
#include "outfont.hxx"
#include "ios/salcoretextfontutils.hxx"
#include "ios/salcoretextstyle.hxx"

CoreTextStyleInfo::CoreTextStyleInfo() :
    m_fake_bold(false),
    m_fake_italic(false),
    m_matrix(CGAffineTransformIdentity),
    m_stretch_factor(1.0),
    m_CTParagraphStyle(NULL),
    m_CTFont(NULL),
    m_color(NULL),
    m_font_data(NULL)
{
    msgs_debug(style,"create <-->");
}

CoreTextStyleInfo::~CoreTextStyleInfo()
{
    msgs_debug(style,"destroy (font:%p) <-->", m_CTFont);
    SafeCFRelease(m_CTFont);
    SafeCFRelease(m_CTParagraphStyle);
    SafeCFRelease(m_color);
}

long CoreTextStyleInfo::GetFontStretchedSize() const
{
    CGFloat size = CTFontGetSize(m_CTFont);
    return static_cast<long>(size * m_stretch_factor + 0.5);
}

void CoreTextStyleInfo::SetFont(FontSelectPattern* requested_font)
{
    msgs_debug(style,"req(%p) release font %p -->", requested_font, m_CTFont);
    SafeCFRelease(m_CTFont);
    if (!requested_font)
    {
        m_font_data = NULL;
        return;
    }
    const ImplCoreTextFontData* font_data = static_cast<const ImplCoreTextFontData*>(requested_font->mpFontData);

    m_font_data = (ImplCoreTextFontData*)font_data;
    m_matrix = CGAffineTransformIdentity;
    CGFloat font_size = (CGFloat)requested_font->mfExactHeight;

    // enable bold-emulation if needed
    if ( (requested_font->GetWeight() >= WEIGHT_BOLD) &&
        (font_data->GetWeight() < WEIGHT_SEMIBOLD) )
    {
        /* FIXME: add support for fake bold */
        m_fake_bold = true;
    }
    if ( ((requested_font->GetSlant() == ITALIC_NORMAL) || (requested_font->GetSlant() == ITALIC_OBLIQUE)) &&
        !((font_data->GetSlant() == ITALIC_NORMAL) || (font_data->GetSlant() == ITALIC_OBLIQUE)) )
    {
#define kRotationForItalicText 10
        m_fake_italic = true;
        /* about 6 degree of slant */
        m_matrix = CGAffineTransformMake( 1, 0, -tanf( kRotationForItalicText * acosf(0) / 90 ), 1, 0, 0);
    }

    // prepare font stretching
    if ( (requested_font->mnWidth != 0) && (requested_font->mnWidth != requested_font->mnHeight) )
    {
        m_stretch_factor = (float)requested_font->mnWidth / requested_font->mnHeight;
        m_matrix = CGAffineTransformScale(m_matrix, m_stretch_factor, 1.0F );
    }

    /* FIXME: pass attribute to take into accout 'VerticalStyle' */
    /* FIXME: how to deal with 'rendering options' i.e anti-aliasing, does it even matter in CoreText ? */
    m_CTFont = CTFontCreateCopyWithAttributes(font_data->GetCTFont(), font_size, &m_matrix, NULL);
    msgs_debug(style,"font %p <--", m_CTFont);
}

void CoreTextStyleInfo::SetColor(SalColor color)
{
    msgs_debug(style, "r:%d g:%d b:%d -->", SALCOLOR_RED(color), SALCOLOR_GREEN(color), SALCOLOR_BLUE(color));
    SafeCFRelease(m_color);
    CGColorSpaceRef rgb_space = CGColorSpaceCreateDeviceRGB();
    CGFloat c[] = { SALCOLOR_RED(color) / 255.0, SALCOLOR_GREEN(color) / 255.0, SALCOLOR_BLUE(color) / 255.0, 1.0 };
    m_color = CGColorCreate(rgb_space, c);
    CGColorSpaceRelease(rgb_space);
    msgs_debug(style,"color=%p <--", m_color);
}

void CoreTextStyleInfo::SetColor(void)
{
    msgs_debug(style, "null -->");
    SafeCFRelease(m_color);
    msgs_debug(style,"color=%p <--", m_color);
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
