/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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
#ifndef SC_VBA_TEXTBOX_HXX
#define SC_VBA_TEXTBOX_HXX
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/text/XTextRange.hpp>
#include <ooo/vba/msforms/XTextBoxShape.hpp>
#include <vbahelper/vbashape.hxx>
#include "excelvbahelper.hxx"
#include "vbacharacters.hxx"

typedef cppu::ImplInheritanceHelper1< ScVbaShape, ov::msforms::XTextBoxShape > TextBoxShapeImpl_BASE;

class ScVbaTextBoxShape : public TextBoxShapeImpl_BASE
{
    css::uno::Reference< css::text::XTextRange > m_xTextRange;
    css::uno::Reference< css::frame::XModel > m_xModel;
public:
    ScVbaTextBoxShape( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape >& xShape, const css::uno::Reference< css::drawing::XShapes >& xShapes, const css::uno::Reference< css::frame::XModel >& xModel );

   // Attributes
    virtual rtl::OUString SAL_CALL getText() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setText( const rtl::OUString& _text ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< ov::excel::XCharacters > SAL_CALL characters( const css::uno::Any& Start, const css::uno::Any& Length ) throw (css::uno::RuntimeException);
};
#endif //SC_VBA_TEXTBOX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
