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

#ifndef _ESCHER_HXX
#define _ESCHER_HXX

#include <filter/msfilter/escherex.hxx>

const sal_uInt32 nInlineHack = 0x00010001;
class SwFrmFmt;
// #i30669#
class SwFmtHoriOrient;
class SwFmtVertOrient;

class WinwordAnchoring : public EscherExClientRecord_Base
{
public:
    void WriteData(EscherEx& rEx) const;
    void SetAnchoring(const SwFrmFmt& rFmt);

    /** method to perform conversion of positioning attributes with the help
        of corresponding layout information

        #i30669#
        Because most of the Writer object positions doesn't correspond to the
        object positions in WW8, this method converts the positioning
        attributes. For this conversion the corresponding layout information
        is needed. If no layout information exists - e.g. no layout exists - no
        conversion is performed.
        No conversion is performed for as-character anchored objects. Whose
        object positions are already treated special in method <WriteData(..)>.
        Usage of method: Used by method <SetAnchoring(..)>, nothing else

        @param _iorHoriOri
        input/output parameter - containing the current horizontal position
        attributes, which are converted by this method.

        @param _iorVertOri
        input/output parameter - containing the current vertical position
        attributes, which are converted by this method.

        @param _rFrmFmt
        input parameter - frame format of the anchored object

        @return boolean, indicating, if a conversion has been performed.
    */
    static bool ConvertPosition( SwFmtHoriOrient& _iorHoriOri,
                                 SwFmtVertOrient& _iorVertOri,
                                 const SwFrmFmt& _rFrmFmt );

private:
    bool mbInline;
    sal_uInt32 mnXAlign;
    sal_uInt32 mnYAlign;
    sal_uInt32 mnXRelTo;
    sal_uInt32 mnYRelTo;

};

class SwEscherExGlobal : public EscherExGlobal
{
public:
    explicit            SwEscherExGlobal();
    virtual             ~SwEscherExGlobal();

private:
    /** Overloaded to create a new memory stream for picture data. */
    virtual SvStream*   ImplQueryPictureStream();

private:
    boost::shared_ptr< SvStream > mxPicStrm;
};

class SwBasicEscherEx : public EscherEx
{
private:
    void Init();
protected:
    WW8Export& rWrt;
    SvStream* pEscherStrm;
    long mnEmuMul, mnEmuDiv;

    virtual sal_Int32 WriteFlyFrameAttr(const SwFrmFmt& rFmt, MSO_SPT eShapeType,
        EscherPropertyContainer& rPropOpt);
    void WriteBrushAttr(const SvxBrushItem &rBrush,
        EscherPropertyContainer& rPropOpt);
    void WriteOLEPicture(EscherPropertyContainer &rPropOpt,
        sal_uInt32 nShapeFlags, const Graphic &rGraphic, const SdrObject &rObj,
        sal_uInt32 nShapeId, const com::sun::star::awt::Rectangle* pVisArea );
    void WriteGrfAttr(const SwNoTxtNode& rNd,EscherPropertyContainer& rPropOpt);

    sal_Int32 DrawModelToEmu(sal_Int32 nVal) const
        { return BigMulDiv(nVal, mnEmuMul, mnEmuDiv); }

    sal_Int32 ToFract16(sal_Int32 nVal, sal_uInt32 nMax) const;

    virtual void SetPicId(const SdrObject &, sal_uInt32, EscherPropertyContainer &);
    SdrLayerID GetInvisibleHellId() const;

public:
    SwBasicEscherEx(SvStream* pStrm, WW8Export& rWrt);
    sal_Int32 WriteGrfFlyFrame(const SwFrmFmt& rFmt, sal_uInt32 nShapeId);
    //For i120928,to export graphic of bullet
    sal_Int32 WriteGrfBullet(const Graphic&);
    sal_Int32 WriteOLEFlyFrame(const SwFrmFmt& rFmt, sal_uInt32 nShapeId);
    void WriteEmptyFlyFrame(const SwFrmFmt& rFmt, sal_uInt32 nShapeId);
    virtual void WriteFrmExtraData(const SwFrmFmt&);
    virtual void WritePictures();
    virtual ~SwBasicEscherEx();
private:
    //No copying
    SwBasicEscherEx(const SwBasicEscherEx&);
    SwBasicEscherEx& operator=(const SwBasicEscherEx&);
};

class SwEscherEx : public SwBasicEscherEx
{
private:
    std::vector<sal_uLong> aFollowShpIds;
    EscherExHostAppData aHostData;
    WinwordAnchoring aWinwordAnchoring;
    WW8_WrPlcTxtBoxes *pTxtBxs;

    sal_uInt32 GetFlyShapeId(const SwFrmFmt& rFmt,
        unsigned int nHdFtIndex, DrawObjPointerVector &rPVec);
    void MakeZOrderArrAndFollowIds(std::vector<DrawObj>& rSrcArr,
        DrawObjPointerVector& rDstArr);

    sal_Int32 WriteFlyFrm(const DrawObj &rObj, sal_uInt32 &rShapeId,
        DrawObjPointerVector &rPVec);
    sal_Int32 WriteTxtFlyFrame(const DrawObj &rObj, sal_uInt32 nShapeId,
        sal_uInt32 nTxtBox, DrawObjPointerVector &rPVec);
    void WriteOCXControl(const SwFrmFmt& rFmt,sal_uInt32 nShapeId);
    virtual sal_Int32 WriteFlyFrameAttr(const SwFrmFmt& rFmt, MSO_SPT eShapeType,
        EscherPropertyContainer& rPropOpt);

    virtual sal_uInt32 QueryTextID(
        const com::sun::star::uno::Reference<
        com::sun::star::drawing::XShape > &,sal_uInt32);
    virtual void SetPicId(const SdrObject &rSdrObj, sal_uInt32 nShapeId,
        EscherPropertyContainer &rPropOpt);
public:
    SwEscherEx( SvStream* pStrm, WW8Export& rWW8Wrt );
    virtual ~SwEscherEx();
    void FinishEscher();
    virtual void WritePictures();

    virtual void WriteFrmExtraData(const SwFrmFmt& rFmt);

    EscherExHostAppData* StartShape(const com::sun::star::uno::Reference<
        com::sun::star::drawing::XShape > &, const Rectangle*) {return &aHostData;}
private:
    //No copying
    SwEscherEx(const SwEscherEx&);
    SwEscherEx &operator=(const SwEscherEx&);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
