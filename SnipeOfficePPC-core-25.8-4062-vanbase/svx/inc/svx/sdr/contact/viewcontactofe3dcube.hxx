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

#ifndef _SDR_CONTACT_VIEWCONTACTOFE3DCUBE_HXX
#define _SDR_CONTACT_VIEWCONTACTOFE3DCUBE_HXX

#include <svx/sdr/contact/viewcontactofe3d.hxx>

//////////////////////////////////////////////////////////////////////////////
// predeclarations

class E3dCubeObj;

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        class ViewContactOfE3dCube : public ViewContactOfE3d
        {
        public:
            // basic constructor, used from SdrObject.
            ViewContactOfE3dCube(E3dCubeObj& rCubeObj);
            virtual ~ViewContactOfE3dCube();

            // access to SdrObject
            E3dCubeObj& GetE3dCubeObj() const
            {
                return (E3dCubeObj&)GetE3dObject();
            }

        protected:
            // This method is responsible for creating the graphical visualisation data which is
            // stored in the local primitive list. Default creates a yellow replacement rectangle.
            virtual drawinglayer::primitive3d::Primitive3DSequence createViewIndependentPrimitive3DSequence() const;
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_CONTACT_VIEWCONTACTOFE3DCUBE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
