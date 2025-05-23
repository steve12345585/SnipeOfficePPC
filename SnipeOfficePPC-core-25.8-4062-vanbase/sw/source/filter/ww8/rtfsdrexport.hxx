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

#ifndef _RTFSdrEXPORT_HXX_
#define _RTFSdrEXPORT_HXX_

#include <filter/msfilter/escherex.hxx>
#include <editeng/outlobj.hxx>
#include <rtl/strbuf.hxx>

#include <map>

class RtfExport;
class RtfAttributeOutput;

/// Handles export of drawings using RTF markup
class RtfSdrExport : public EscherEx
{
    RtfExport &m_rExport;

    RtfAttributeOutput &m_rAttrOutput;

    const SdrObject* m_pSdrObject;

    /// Remember the shape type.
    sal_uInt32 m_nShapeType;

    /// Remember the shape flags.
    sal_uInt32 m_nShapeFlags;

    /// Remember style, the most important shape attribute ;-)
    rtl::OStringBuffer *m_pShapeStyle;

    std::map<rtl::OString,rtl::OString> m_aShapeProps;

    /// Remember which shape types we had already written.
    bool *m_pShapeTypeWritten;

public:
                        RtfSdrExport( RtfExport &rExport );
    virtual             ~RtfSdrExport();

    /// Export the sdr object as Sdr.
    ///
    /// Call this when you need to export the object as Sdr in RTF.
    sal_uInt32 AddSdrObject( const SdrObject& rObj );

protected:
    /// Start the shape for which we just collected the information.
    ///
    /// Returns the element's tag number, -1 means we wrote nothing.
    using EscherEx::StartShape;
    virtual sal_Int32   StartShape();

    /// End the shape.
    ///
    /// The parameter is just what we got from StartShape().
    using EscherEx::EndShape;
    virtual void        EndShape( sal_Int32 nShapeElement );

    virtual void        Commit( EscherPropertyContainer& rProps, const Rectangle& rRect );

private:

    virtual void OpenContainer( sal_uInt16 nEscherContainer, int nRecInstance = 0 );
    virtual void CloseContainer();

    using EscherEx::EnterGroup;
    virtual sal_uInt32 EnterGroup( const String& rShapeName, const Rectangle* pBoundRect = 0 );
    virtual void LeaveGroup();

    virtual void AddShape( sal_uInt32 nShapeType, sal_uInt32 nShapeFlags, sal_uInt32 nShapeId = 0 );

private:
    /// Add starting and ending point of a line to the m_pShapeAttrList.
    void AddLineDimensions( const Rectangle& rRectangle );

    /// Add position and size to the OStringBuffer.
    void AddRectangleDimensions( rtl::OStringBuffer& rBuffer, const Rectangle& rRectangle );

    void WriteOutliner(const OutlinerParaObject& rParaObj);

    /// Exports the pib property of the shape
    void impl_writeGraphic();
};

#endif // _RTFSdrEXPORT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
