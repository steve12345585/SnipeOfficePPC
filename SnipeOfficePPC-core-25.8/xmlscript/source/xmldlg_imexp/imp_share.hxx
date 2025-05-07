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

#include "common.hxx"
#include "misc.hxx"
#include <xmlscript/xmldlg_imexp.hxx>
#include <xmlscript/xmllib_imexp.hxx>
#include <xmlscript/xmlmod_imexp.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/FontDescriptor.hpp>
#include <com/sun/star/awt/FontEmphasisMark.hpp>
#include <com/sun/star/awt/FontRelief.hpp>
#include <com/sun/star/xml/input/XRoot.hpp>
#include <com/sun/star/script/XLibraryContainer.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace css = ::com::sun::star;

namespace xmlscript
{

inline sal_Int32 toInt32( ::rtl::OUString const & rStr ) SAL_THROW(())
{
    sal_Int32 nVal;
    if (rStr.getLength() > 2 && rStr[ 0 ] == '0' && rStr[ 1 ] == 'x')
        nVal = rStr.copy( 2 ).toInt32( 16 );
    else
        nVal = rStr.toInt32();
    return nVal;
}

inline bool getBoolAttr(
    sal_Bool * pRet, ::rtl::OUString const & rAttrName,
    css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
    sal_Int32 nUid )
{
    ::rtl::OUString aValue( xAttributes->getValueByUidName( nUid, rAttrName ) );
    if (!aValue.isEmpty())
    {
        if ( aValue == "true" )
        {
            *pRet = sal_True;
            return true;
        }
        else if ( aValue == "false" )
        {
            *pRet = sal_False;
            return true;
        }
        else
        {
            throw css::xml::sax::SAXException(
                rAttrName + OUSTR(": no boolean value (true|false)!"),
                css::uno::Reference<css::uno::XInterface>(), css::uno::Any() );
        }
    }
    return false;
}

inline bool getStringAttr(
    ::rtl::OUString * pRet, ::rtl::OUString const & rAttrName,
    css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
    sal_Int32 nUid )
{
    *pRet = xAttributes->getValueByUidName( nUid, rAttrName );
    return (!pRet->isEmpty());
}

inline bool getLongAttr(
    sal_Int32 * pRet, ::rtl::OUString const & rAttrName,
    css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
    sal_Int32 nUid )
{
    ::rtl::OUString aValue( xAttributes->getValueByUidName( nUid, rAttrName ) );
    if (!aValue.isEmpty())
    {
        *pRet = toInt32( aValue );
        return true;
    }
    return false;
}

class ImportContext;

//==============================================================================
typedef ::cppu::WeakImplHelper1< css::xml::input::XRoot >   DialogImport_Base;
struct DialogImport
    : DialogImport_Base
{
    friend class ImportContext;

    css::uno::Reference< css::uno::XComponentContext > _xContext;
    css::uno::Reference< css::util::XNumberFormatsSupplier > _xSupplier;

    ::boost::shared_ptr< ::std::vector< ::rtl::OUString > > _pStyleNames;
    ::boost::shared_ptr< ::std::vector< css::uno::Reference< css::xml::input::XElement > > > _pStyles;

    css::uno::Reference< css::container::XNameContainer > _xDialogModel;
    css::uno::Reference< css::lang::XMultiServiceFactory > _xDialogModelFactory;
    css::uno::Reference< css::frame::XModel > _xDoc;
    css::uno::Reference< css::script::XLibraryContainer > _xScriptLibraryContainer;

    sal_Int32 XMLNS_DIALOGS_UID, XMLNS_SCRIPT_UID;

public:
    inline bool isEventElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName )
    {
        return ((XMLNS_SCRIPT_UID == nUid && (rLocalName == "event" || rLocalName == "listener-event" )) ||
                (XMLNS_DIALOGS_UID == nUid && rLocalName == "event" ));
    }

    void addStyle(
        ::rtl::OUString const & rStyleId,
        css::uno::Reference< css::xml::input::XElement > const & xStyle )
        SAL_THROW(());
    css::uno::Reference< css::xml::input::XElement > getStyle(
        ::rtl::OUString const & rStyleId ) const
        SAL_THROW(());

    inline css::uno::Reference< css::uno::XComponentContext >
    const & getComponentContext() SAL_THROW(()) { return _xContext; }
    css::uno::Reference< css::util::XNumberFormatsSupplier >
    const & getNumberFormatsSupplier();

    inline DialogImport(
        css::uno::Reference<css::uno::XComponentContext> const & xContext,
        css::uno::Reference<css::container::XNameContainer>
        const & xDialogModel,
        ::boost::shared_ptr< ::std::vector< ::rtl::OUString > >& pStyleNames,
        ::boost::shared_ptr< ::std::vector< css::uno::Reference< css::xml::input::XElement > > >& pStyles,
        css::uno::Reference<css::frame::XModel> const & xDoc )
        SAL_THROW(())
        : _xContext( xContext )
        , _pStyleNames( pStyleNames )
        , _pStyles( pStyles )
        , _xDialogModel( xDialogModel )
        , _xDialogModelFactory( xDialogModel, css::uno::UNO_QUERY_THROW ), _xDoc( xDoc )
        { OSL_ASSERT( _xDialogModel.is() && _xDialogModelFactory.is() &&
                      _xContext.is() ); }
    inline DialogImport( const DialogImport& rOther ) : DialogImport_Base()
        , _xContext( rOther._xContext )
        , _xSupplier( rOther._xSupplier )
        , _pStyleNames( rOther._pStyleNames )
        , _pStyles( rOther._pStyles )
        , _xDialogModel( rOther._xDialogModel )
        , _xDialogModelFactory( rOther._xDialogModelFactory )
        , _xDoc( rOther._xDoc )
        , XMLNS_DIALOGS_UID( rOther.XMLNS_DIALOGS_UID )
        , XMLNS_SCRIPT_UID( rOther.XMLNS_SCRIPT_UID ) {}

    virtual ~DialogImport()
        SAL_THROW(());

    inline css::uno::Reference< css::frame::XModel > getDocOwner() { return _xDoc; }

    // XRoot
    virtual void SAL_CALL startDocument(
        css::uno::Reference< css::xml::input::XNamespaceMapping >
        const & xNamespaceMapping )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endDocument()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL processingInstruction(
        ::rtl::OUString const & rTarget, ::rtl::OUString const & rData )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL setDocumentLocator(
        css::uno::Reference< css::xml::sax::XLocator > const & xLocator )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startRootElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
};

//==============================================================================
class ElementBase
    : public ::cppu::WeakImplHelper1< css::xml::input::XElement >
{
protected:
    DialogImport * _pImport;
    ElementBase * _pParent;

    sal_Int32 _nUid;
    ::rtl::OUString _aLocalName;
    css::uno::Reference< css::xml::input::XAttributes > _xAttributes;

public:
    ElementBase(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(());
    virtual ~ElementBase()
        SAL_THROW(());

    // XElement
    virtual css::uno::Reference<css::xml::input::XElement> SAL_CALL getParent()
        throw (css::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getLocalName()
        throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getUid()
        throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::xml::input::XAttributes >
    SAL_CALL getAttributes() throw (css::uno::RuntimeException);
    virtual void SAL_CALL ignorableWhitespace(
        ::rtl::OUString const & rWhitespaces )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL characters( ::rtl::OUString const & rChars )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL processingInstruction(
        ::rtl::OUString const & Target, ::rtl::OUString const & Data )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
};

//==============================================================================
class StylesElement
    : public ElementBase
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline StylesElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ElementBase( pImport->XMLNS_DIALOGS_UID,
                       rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class StyleElement
    : public ElementBase
{
    sal_Int32 _backgroundColor;
    sal_Int32 _textColor;
    sal_Int32 _textLineColor;
    sal_Int16 _border;
    sal_Int32 _borderColor;
    css::awt::FontDescriptor _descr;
    sal_Int16 _fontRelief;
    sal_Int16 _fontEmphasisMark;
    sal_Int32 _fillColor;
    sal_Int16 _visualEffect;

    // current highest mask: 0x40
    short _inited, _hasValue;

    void setFontProperties(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );

public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    bool importTextColorStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );
    bool importTextLineColorStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );
    bool importFillColorStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );
    bool importBackgroundColorStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );
    bool importFontStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );
    bool importBorderStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );
    bool importVisualEffectStyle(
        css::uno::Reference< css::beans::XPropertySet > const & xProps );

    inline StyleElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ElementBase( pImport->XMLNS_DIALOGS_UID,
                       rLocalName, xAttributes, pParent, pImport )
        , _fontRelief( css::awt::FontRelief::NONE )
        , _fontEmphasisMark( css::awt::FontEmphasisMark::NONE )
        , _inited( 0 )
        , _hasValue( 0 )
        {}
};

//==============================================================================
class MenuPopupElement
    : public ElementBase
{
    ::std::vector< ::rtl::OUString > _itemValues;
    ::std::vector< sal_Int16 > _itemSelected;
public:
    css::uno::Sequence< ::rtl::OUString > getItemValues();
    css::uno::Sequence< sal_Int16 > getSelectedItems();

    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline MenuPopupElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ElementBase( pImport->XMLNS_DIALOGS_UID,
                       rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class ControlElement
    : public ElementBase
{
    friend class EventElement;

protected:
    sal_Int32 _nBasePosX, _nBasePosY;

    ::std::vector< css::uno::Reference< css::xml::input::XElement > > _events;

    ::rtl::OUString getControlId(
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    ::rtl::OUString getControlModelName(
        rtl::OUString const& rDefaultModel,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    css::uno::Reference< css::xml::input::XElement > getStyle(
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
public:
    ::std::vector<css::uno::Reference< css::xml::input::XElement> > *getEvents()
        SAL_THROW(()) { return &_events; }

    ControlElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(());
};

//==============================================================================
class ImportContext
{
protected:
    DialogImport * _pImport;
    css::uno::Reference< css::beans::XPropertySet > _xControlModel;
    ::rtl::OUString _aId;

public:
    inline ImportContext(
        DialogImport * pImport,
        css::uno::Reference< css::beans::XPropertySet > const & xControlModel_,
        ::rtl::OUString const & id )
        : _pImport( pImport ),
          _xControlModel( xControlModel_ ),
          _aId( id )
        { OSL_ASSERT( _xControlModel.is() ); }

    inline css::uno::Reference< css::beans::XPropertySet > getControlModel() const
        { return _xControlModel; }

    void importDefaults(
        sal_Int32 nBaseX, sal_Int32 nBaseY,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        bool supportPrintable = true );
    void importEvents(
        ::std::vector< css::uno::Reference< css::xml::input::XElement > >
        const & rEvents );

    bool importStringProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importDoubleProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importBooleanProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importShortProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importLongProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importLongProperty(
        sal_Int32 nOffset,
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importHexLongProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importAlignProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importVerticalAlignProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importImageURLProperty( rtl::OUString const & rPropName, rtl::OUString const & rAttrName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes );
    bool importImageAlignProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importImagePositionProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importDateFormatProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importTimeFormatProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importOrientationProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importButtonTypeProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importLineEndFormatProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importSelectionTypeProperty(
        ::rtl::OUString const & rPropName, ::rtl::OUString const & rAttrName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
    bool importDataAwareProperty(
        ::rtl::OUString const & rPropName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes );
};

//==============================================================================
class ControlImportContext : public ImportContext
{
public:
    inline ControlImportContext(
        DialogImport * pImport,
        ::rtl::OUString const & rId, ::rtl::OUString const & rControlName )
        : ImportContext(
            pImport,
            css::uno::Reference< css::beans::XPropertySet >(
                pImport->_xDialogModelFactory->createInstance( rControlName ),
                css::uno::UNO_QUERY_THROW ), rId )
        {}
    inline ControlImportContext(
        DialogImport * pImport,
        const css::uno::Reference< css::beans::XPropertySet >& xProps, ::rtl::OUString const & rControlName )
        : ImportContext(
            pImport,
                xProps,
                rControlName )
        {}
    inline ~ControlImportContext()
    {
        _pImport->_xDialogModel->insertByName(
            _aId, css::uno::makeAny(
                css::uno::Reference<css::awt::XControlModel>::query(
                    _xControlModel ) ) );
    }
};

//==============================================================================
class WindowElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline WindowElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class EventElement
    : public ElementBase
{
public:
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline EventElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ElementBase( nUid, rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class BulletinBoardElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    BulletinBoardElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(());
};

//==============================================================================
class ButtonElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline ButtonElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class CheckBoxElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline CheckBoxElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class ComboBoxElement
    : public ControlElement
{
    css::uno::Reference< css::xml::input::XElement > _popup;
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline ComboBoxElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class MenuListElement
    : public ControlElement
{
    css::uno::Reference< css::xml::input::XElement > _popup;
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline MenuListElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class RadioElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline RadioElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class RadioGroupElement
    : public ControlElement
{
    ::std::vector< css::uno::Reference< css::xml::input::XElement > > _radios;
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline RadioGroupElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class TitledBoxElement
    : public BulletinBoardElement
{
    ::rtl::OUString _label;
    ::std::vector< css::uno::Reference< css::xml::input::XElement > > _radios;
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline TitledBoxElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : BulletinBoardElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class TextElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline TextElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};
//==============================================================================
class FixedHyperLinkElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline FixedHyperLinkElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};
//==============================================================================
class TextFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline TextFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class ImageControlElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline ImageControlElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class FileControlElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline FileControlElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class TreeControlElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline TreeControlElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class CurrencyFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline CurrencyFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class DateFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline DateFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class NumericFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline NumericFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class TimeFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline TimeFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class PatternFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline PatternFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class FormattedFieldElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline FormattedFieldElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class FixedLineElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline FixedLineElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class ScrollBarElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline ScrollBarElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================

class SpinButtonElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline SpinButtonElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

//==============================================================================
class MultiPage
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline MultiPage(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {
            m_xContainer.set( _pImport->_xDialogModelFactory->createInstance( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.UnoMultiPageModel") ) ), css::uno::UNO_QUERY );
        }
private:
    css::uno::Reference< css::container::XNameContainer > m_xContainer;
};

//==============================================================================
class Frame
    : public ControlElement
{
    ::rtl::OUString _label;
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline Frame(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
private:
    css::uno::Reference< css::container::XNameContainer > m_xContainer;
};

//==============================================================================
class Page
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline Page(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {
            m_xContainer.set( _pImport->_xDialogModelFactory->createInstance( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.UnoPageModel") ) ), css::uno::UNO_QUERY );
        }
private:
    css::uno::Reference< css::container::XNameContainer > m_xContainer;
};


class ProgressBarElement
    : public ControlElement
{
public:
    virtual css::uno::Reference< css::xml::input::XElement >
    SAL_CALL startChildElement(
        sal_Int32 nUid, ::rtl::OUString const & rLocalName,
        css::uno::Reference<css::xml::input::XAttributes> const & xAttributes )
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);
    virtual void SAL_CALL endElement()
        throw (css::xml::sax::SAXException, css::uno::RuntimeException);

    inline ProgressBarElement(
        ::rtl::OUString const & rLocalName,
        css::uno::Reference< css::xml::input::XAttributes > const & xAttributes,
        ElementBase * pParent, DialogImport * pImport )
        SAL_THROW(())
        : ControlElement( rLocalName, xAttributes, pParent, pImport )
        {}
};

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
