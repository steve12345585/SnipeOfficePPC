/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef OOX_HELPER_TEXTINPUTSTREAM_HXX
#define OOX_HELPER_TEXTINPUTSTREAM_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <rtl/ustring.hxx>

namespace com { namespace sun { namespace star {
    namespace io { class XInputStream; }
    namespace io { class XTextInputStream; }
    namespace uno { class XComponentContext; }
} } }

namespace oox {

class BinaryInputStream;

// ============================================================================

class TextInputStream
{
public:
    explicit            TextInputStream(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rxInStrm,
                            rtl_TextEncoding eTextEnc );

    explicit            TextInputStream(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
                            BinaryInputStream& rInStrm,
                            rtl_TextEncoding eTextEnc );

                        ~TextInputStream();

    /** Returns true, if no more text is available in the stream.
     */
    bool                isEof() const;

    /** Reads a text line from the stream.

        If the last line in the stream is not terminated with line-end
        character(s), the stream will immediately go into EOF state and return
        the text line. Otherwise, if the last character in the stream is a
        line-end character, the next call to this function will turn the stream
        into EOF state and return an empty string.
     */
    ::rtl::OUString     readLine();

    /** Reads a text portion from the stream until the specified character is
        found.

        If the end of the stream is not terminated with the specified
        character, the stream will immediately go into EOF state and return the
        remaining text portion. Otherwise, if the last character in the stream
        is the specified character (and caller specifies to read and return it,
        see parameter bIncludeChar), the next call to this function will turn
        the stream into EOF state and return an empty string.

        @param cChar
            The separator character to be read to.

        @param bIncludeChar
            True = if found, the specified character will be read from stream
                and included in the returned string.
            False = the specified character will neither be read from the
                stream nor included in the returned string, but will be
                returned as first character in the next call of this function
                or readLine().
     */
    ::rtl::OUString     readToChar( sal_Unicode cChar, bool bIncludeChar );

    // ------------------------------------------------------------------------

    /** Creates a UNO text input stream object from the passed UNO input stream.
     */
    static ::com::sun::star::uno::Reference< ::com::sun::star::io::XTextInputStream >
                        createXTextInputStream(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rxInStrm,
                            rtl_TextEncoding eTextEnc );

    // ------------------------------------------------------------------------
private:
    void                init(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rxInStrm,
                            rtl_TextEncoding eTextEnc );

    /** Adds the pending character in front of the passed string, if existing. */
    ::rtl::OUString     createFinalString( const ::rtl::OUString& rString );

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XTextInputStream >
                        mxTextStrm;
    sal_Unicode         mcPendingChar;
};

// ============================================================================

} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
