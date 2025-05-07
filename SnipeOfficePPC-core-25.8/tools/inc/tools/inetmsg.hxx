/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _TOOLS_INETMSG_HXX
#define _TOOLS_INETMSG_HXX

#include "tools/toolsdllapi.h"
#include <sal/types.h>

#include <rtl/textenc.h>

#include <tools/inetmime.hxx>
#include <tools/stream.hxx>
#include <tools/string.hxx>
#include <vector>

class DateTime;

/*=======================================================================
 *
 * INetMessageHeader Interface.
 *
 *=====================================================================*/
class INetMessageHeader
{
    rtl::OString m_aName;
    rtl::OString m_aValue;

public:
    INetMessageHeader()
    {}

    INetMessageHeader (
        const rtl::OString& rName, const rtl::OString& rValue)
        : m_aName (rName), m_aValue (rValue)
    {}

    INetMessageHeader (
        const INetMessageHeader& rHdr)
        : m_aName (rHdr.m_aName), m_aValue (rHdr.m_aValue)
    {}

    ~INetMessageHeader()
    {}

    INetMessageHeader& operator= (const INetMessageHeader& rHdr)
    {
        m_aName  = rHdr.m_aName;
        m_aValue = rHdr.m_aValue;
        return *this;
    }

    const rtl::OString& GetName() const { return m_aName; }
    const rtl::OString& GetValue() const { return m_aValue; }

    friend SvStream& operator<< (
        SvStream& rStrm, const INetMessageHeader& rHdr)
    {
        write_lenPrefixed_uInt8s_FromOString<sal_uInt16>(rStrm, rHdr.m_aName);
        write_lenPrefixed_uInt8s_FromOString<sal_uInt16>(rStrm, rHdr.m_aValue);
        return rStrm;
    }

    friend SvStream& operator>> (
        SvStream& rStrm, INetMessageHeader& rHdr)
    {
        rHdr.m_aName = read_lenPrefixed_uInt8s_ToOString<sal_uInt16>(rStrm);
        rHdr.m_aValue = read_lenPrefixed_uInt8s_ToOString<sal_uInt16>(rStrm);
        return rStrm;
    }
};

/*=======================================================================
 *
 * INetMessage Interface.
 *
 *=====================================================================*/
 typedef ::std::vector< INetMessageHeader* > HeaderList_impl;
class INetMessage
{
    HeaderList_impl m_aHeaderList;

    sal_uIntPtr     m_nDocSize;
    UniString       m_aDocName;
    SvLockBytesRef  m_xDocLB;

    void ListCleanup_Impl();
    void ListCopy (const INetMessage& rMsg);

protected:
    UniString GetHeaderName_Impl (
        sal_uIntPtr nIndex, rtl_TextEncoding eEncoding) const
    {
        if ( nIndex < m_aHeaderList.size() ) {
            return UniString( m_aHeaderList[ nIndex ]->GetName(), eEncoding );
        } else {
            return UniString();
        }
    }

    UniString GetHeaderValue_Impl (
        sal_uIntPtr nIndex, INetMIME::HeaderFieldType eType) const
    {
        if ( nIndex < m_aHeaderList.size() ) {
            return INetMIME::decodeHeaderFieldBody(eType, m_aHeaderList[ nIndex ]->GetValue());
        } else {
            return UniString();
        }
    }

    void SetHeaderField_Impl (
        const INetMessageHeader &rHeader, sal_uIntPtr &rnIndex)
    {
        INetMessageHeader *p = new INetMessageHeader (rHeader);
        if (m_aHeaderList.size() <= rnIndex)
        {
            rnIndex = m_aHeaderList.size();
            m_aHeaderList.push_back( p );
        }
        else
        {
            delete m_aHeaderList[ rnIndex ];
            m_aHeaderList[ rnIndex ] = p;
        }
    }

    void SetHeaderField_Impl (
        INetMIME::HeaderFieldType  eType,
        const rtl::OString        &rName,
        const UniString           &rValue,
        sal_uIntPtr                     &rnIndex);

    virtual SvStream& operator<< (SvStream& rStrm) const;
    virtual SvStream& operator>> (SvStream& rStrm);

public:
    INetMessage() : m_nDocSize(0) {}
    virtual ~INetMessage();

    INetMessage (const INetMessage& rMsg)
        : m_nDocSize (rMsg.m_nDocSize),
          m_aDocName (rMsg.m_aDocName),
          m_xDocLB   (rMsg.m_xDocLB)
    {
        ListCopy (rMsg);
    }

    INetMessage& operator= (const INetMessage& rMsg)
    {
        m_nDocSize = rMsg.m_nDocSize;
        m_aDocName = rMsg.m_aDocName;
        m_xDocLB   = rMsg.m_xDocLB;
        ListCopy (rMsg);
        return *this;
    }

    sal_uIntPtr GetHeaderCount() const { return m_aHeaderList.size(); }

    UniString GetHeaderName (sal_uIntPtr nIndex) const
    {
        return GetHeaderName_Impl (nIndex, RTL_TEXTENCODING_ASCII_US);
    }

    UniString GetHeaderValue (sal_uIntPtr nIndex) const
    {
        return GetHeaderValue_Impl (nIndex, INetMIME::HEADER_FIELD_TEXT);
    }

    INetMessageHeader GetHeaderField (sal_uIntPtr nIndex) const
    {
        if ( nIndex < m_aHeaderList.size() ) {
            return INetMessageHeader( *m_aHeaderList[ nIndex ] );
        } else {
            return INetMessageHeader();
        }
    }

    virtual sal_uIntPtr SetHeaderField (
        const INetMessageHeader &rField,
        sal_uIntPtr nIndex = ((sal_uIntPtr)-1)
    );

    sal_uIntPtr GetDocumentSize() const { return m_nDocSize; }
    void  SetDocumentSize (sal_uIntPtr nSize) { m_nDocSize = nSize; }

    const UniString& GetDocumentName() const { return m_aDocName; }
    void  SetDocumentName (const UniString& rName) { m_aDocName = rName; }

    SvLockBytes* GetDocumentLB() const { return m_xDocLB; }
    void         SetDocumentLB (SvLockBytes *pDocLB) { m_xDocLB = pDocLB; }

    friend SvStream& operator<< (
        SvStream& rStrm, const INetMessage& rMsg)
    {
        return rMsg.operator<< (rStrm);
    }

    friend SvStream& operator>> (
        SvStream& rStrm, INetMessage& rMsg)
    {
        return rMsg.operator>> (rStrm);
    }
};

/*=======================================================================
 *
 * INetRFC822Message Interface.
 *
 *=====================================================================*/
#define INETMSG_RFC822_BCC                 0
#define INETMSG_RFC822_CC                  1
#define INETMSG_RFC822_COMMENTS            2
#define INETMSG_RFC822_DATE                3
#define INETMSG_RFC822_FROM                4
#define INETMSG_RFC822_IN_REPLY_TO         5
#define INETMSG_RFC822_KEYWORDS            6
#define INETMSG_RFC822_MESSAGE_ID          7
#define INETMSG_RFC822_REFERENCES          8
#define INETMSG_RFC822_REPLY_TO            9
#define INETMSG_RFC822_RETURN_PATH        10
#define INETMSG_RFC822_SENDER             11
#define INETMSG_RFC822_SUBJECT            12
#define INETMSG_RFC822_TO                 13

#define INETMSG_RFC822_X_MAILER           14
#define INETMSG_RFC822_RETURN_RECEIPT_TO  15

#define INETMSG_RFC822_NUMHDR             16

class TOOLS_DLLPUBLIC INetRFC822Message : public INetMessage
{
    sal_uIntPtr m_nIndex[INETMSG_RFC822_NUMHDR];

protected:
    virtual SvStream& operator<< (SvStream& rStrm) const;
    virtual SvStream& operator>> (SvStream& rStrm);

public:
    INetRFC822Message();
    INetRFC822Message (const INetRFC822Message& rMsg);
    virtual ~INetRFC822Message();

    INetRFC822Message& operator= (const INetRFC822Message& rMsg);

    static sal_Bool ParseDateField (
        const UniString& rDateField, DateTime& rDateTime);

    using INetMessage::SetHeaderField;
    virtual sal_uIntPtr SetHeaderField (
        const INetMessageHeader &rHeader,
        sal_uIntPtr nIndex = ((sal_uIntPtr)-1)
    );

    /** Header fields.
     */
    UniString GetBCC() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_BCC],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetCC() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_CC],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetComments() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_COMMENTS],
            INetMIME::HEADER_FIELD_TEXT);
    }

    UniString GetDate() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_DATE],
            INetMIME::HEADER_FIELD_STRUCTURED);
    }

    UniString GetFrom() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_FROM],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetInReplyTo() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_IN_REPLY_TO],
            INetMIME::HEADER_FIELD_ADDRESS); // ??? MESSAGE_ID ???
    }

    UniString GetKeywords() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_KEYWORDS],
            INetMIME::HEADER_FIELD_PHRASE);
    }

    UniString GetMessageID() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_MESSAGE_ID],
            INetMIME::HEADER_FIELD_MESSAGE_ID);
    }

    UniString GetReferences() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_REFERENCES],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetReplyTo() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_REPLY_TO],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetReturnPath() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_RETURN_PATH],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetReturnReceiptTo() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_RETURN_RECEIPT_TO],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetSender() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_SENDER],
            INetMIME::HEADER_FIELD_ADDRESS);
    }

    UniString GetSubject() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_SUBJECT],
            INetMIME::HEADER_FIELD_TEXT);
    }

    UniString GetTo() const
    {
        return GetHeaderValue_Impl (
            m_nIndex[INETMSG_RFC822_TO],
            INetMIME::HEADER_FIELD_TEXT);
    }

    /** Stream operators.
     */
    friend SvStream& operator<< (
        SvStream& rStrm, const INetRFC822Message& rMsg)
    {
        return rMsg.operator<< (rStrm);
    }

    friend SvStream& operator>> (
        SvStream& rStrm, INetRFC822Message& rMsg)
    {
        return rMsg.operator>> (rStrm);
    }
};

/*=======================================================================
 *
 * INetMIMEMessage Interface.
 *
 *=====================================================================*/
#define INETMSG_MIME_VERSION                    0
#define INETMSG_MIME_CONTENT_DESCRIPTION        1
#define INETMSG_MIME_CONTENT_DISPOSITION        2
#define INETMSG_MIME_CONTENT_ID                 3
#define INETMSG_MIME_CONTENT_TYPE               4
#define INETMSG_MIME_CONTENT_TRANSFER_ENCODING  5

#define INETMSG_MIME_NUMHDR                     6

enum INetMessageContainerType
{
    INETMSG_MESSAGE_RFC822,
    INETMSG_MULTIPART_MIXED,
    INETMSG_MULTIPART_ALTERNATIVE,
    INETMSG_MULTIPART_DIGEST,
    INETMSG_MULTIPART_PARALLEL,
    INETMSG_MULTIPART_RELATED,
    INETMSG_MULTIPART_FORM_DATA
};

class INetMIMEMessage;
typedef ::std::vector< INetMIMEMessage* > INetMIMEMessgeList_impl;
class TOOLS_DLLPUBLIC INetMIMEMessage : public INetRFC822Message
{
    sal_uIntPtr             m_nIndex[INETMSG_MIME_NUMHDR];

    INetMIMEMessage*        pParent;
    INetMIMEMessgeList_impl aChildren;
    rtl::OString            m_aBoundary;
    sal_Bool                bHeaderParsed;

    friend class INetMIMEMessageStream;

    const rtl::OString& GetMultipartBoundary() const { return m_aBoundary; }
    void SetMultipartBoundary (const rtl::OString& rBnd) { m_aBoundary = rBnd; }

    void CleanupImp();
    void CopyImp    (const INetMIMEMessage& rMsg);
    void SetHeaderParsed() { bHeaderParsed = sal_True; }

protected:
    virtual SvStream& operator<< (SvStream& rStrm) const;
    virtual SvStream& operator>> (SvStream& rStrm);

public:
    INetMIMEMessage();
    INetMIMEMessage (const INetMIMEMessage& rMsg);
    virtual ~INetMIMEMessage();

    INetMIMEMessage& operator= (const INetMIMEMessage& rMsg);

    sal_Bool HeaderParsed() const { return bHeaderParsed; }

    virtual INetMIMEMessage* CreateMessage (
        const INetMIMEMessage& rMsg) const;

    using INetRFC822Message::SetHeaderField;
    virtual sal_uIntPtr SetHeaderField (
        const INetMessageHeader &rHeader,
        sal_uIntPtr nIndex = ((sal_uIntPtr)-1)
    );

    /** Header fields.
     */
    void      SetMIMEVersion (const UniString& rVersion);
    UniString GetMIMEVersion() const
    {
        return GetHeaderValue (m_nIndex[INETMSG_MIME_VERSION]);
    }

    UniString GetContentDescription() const
    {
        return GetHeaderValue (m_nIndex[INETMSG_MIME_CONTENT_DESCRIPTION]);
    }

    void      SetContentDisposition (const UniString& rDisposition);
    UniString GetContentDisposition() const
    {
        return GetHeaderValue (m_nIndex[INETMSG_MIME_CONTENT_DISPOSITION]);
    }

    UniString GetContentID() const
    {
        return GetHeaderValue (m_nIndex[INETMSG_MIME_CONTENT_ID]);
    }

    void      SetContentType (const UniString& rType);
    UniString GetContentType() const
    {
        return GetHeaderValue (m_nIndex[INETMSG_MIME_CONTENT_TYPE]);
    }

    void      SetContentTransferEncoding (const rtl::OUString& rEncoding);
    rtl::OUString GetContentTransferEncoding() const
    {
        return GetHeaderValue (m_nIndex[INETMSG_MIME_CONTENT_TRANSFER_ENCODING]);
    }

    virtual void GetDefaultContentType (UniString& rContentType);

    /** Message container methods.
     */
    sal_Bool IsContainer() const
    {
        return (IsMessage() || IsMultipart());
    }
    sal_Bool IsMessage() const
    {
        UniString aType (GetContentType());
        return (aType.CompareIgnoreCaseToAscii("message/", 8) == 0);
    }
    sal_Bool IsMultipart() const
    {
        UniString aType (GetContentType());
        return (aType.CompareIgnoreCaseToAscii("multipart/", 10) == 0);
    }

    INetMIMEMessage* GetChild (sal_uIntPtr nIndex) const
    {
        return ( nIndex < aChildren.size() ) ? aChildren[ nIndex ] : NULL;
    }
    INetMIMEMessage* GetParent() const { return pParent; }

    sal_Bool EnableAttachChild (
        INetMessageContainerType eType = INETMSG_MULTIPART_MIXED);
    sal_Bool AttachChild (
        INetMIMEMessage& rChildMsg, sal_Bool bOwner = sal_True);

    /** Stream operators.
     */
    friend SvStream& operator<< (
        SvStream& rStrm, const INetMIMEMessage& rMsg)
    {
        return rMsg.operator<< (rStrm);
    }

    friend SvStream& operator>> (
        SvStream& rStrm, INetMIMEMessage& rMsg)
    {
        return rMsg.operator>> (rStrm);
    }
};

#endif /* !_TOOLS_INETMSG_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
