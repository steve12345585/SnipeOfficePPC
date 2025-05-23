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

#ifndef INCLUDED_CANVAS_IRENDERMODULE_HXX
#define INCLUDED_CANVAS_IRENDERMODULE_HXX

#include <sal/types.h>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>


namespace basegfx
{
    class B2DRange;
    class B2IRange;
    class B2IVector;
    class B2IPoint;
}

namespace canvas
{
    struct ISurface;

    struct Vertex
    {
        float r,g,b,a;
        float u,v;
        float x,y,z;
    };

    /** Output module interface for backend render implementations.

        Implement this interface for each operating system- or
        library-specific rendering backend, which needs coupling with
        the canvas rendering framework (which can be shared between
        all backend implementations).
     */
    struct IRenderModule
    {
        /** Type of primitive passed to the render module via
            pushVertex()
         */
        enum PrimitiveType
        {
            PRIMITIVE_TYPE_UNKNOWN,
            PRIMITIVE_TYPE_TRIANGLE,
            PRIMITIVE_TYPE_QUAD
        };

        virtual ~IRenderModule() {}

        /// Lock rendermodule against concurrent access
        virtual void lock() const = 0;

        /// Unlock rendermodule for concurrent access
        virtual void unlock() const = 0;

        /** Maximal size of VRAM pages available

            This is typically the maximum texture size of the
            hardware, or some arbitrary limit if the backend is
            software.
         */
        virtual ::basegfx::B2IVector getPageSize() = 0;

        /** Create a (possibly hardware-accelerated) surface

            @return a pointer to a surface, which is an abstraction of
            a piece of (possibly hardware-accelerated) texture memory.
         */
        virtual ::boost::shared_ptr<ISurface> createSurface( const ::basegfx::B2IVector& surfaceSize ) = 0;

        /** Begin rendering the given primitive.

            Each beginPrimitive() call must be matched with an
            endPrimitive() call.
         */
        virtual void      beginPrimitive( PrimitiveType eType ) = 0;

        /** Finish rendering a primitive.

            Each beginPrimitive() call must be matched with an
            endPrimitive() call.
         */
        virtual void      endPrimitive() = 0;

        /** Add given vertex to current primitive

            After issuing a beginPrimitive(), each pushVertex() adds a
            vertex to the active primitive.
         */
        virtual void      pushVertex( const Vertex& vertex ) = 0;

        /** Query error status

            @returns true, if an error occurred during primitive
            construction.
         */
        virtual bool      isError() = 0;
    };

    typedef ::boost::shared_ptr< IRenderModule > IRenderModuleSharedPtr;

    /// Little RAII wrapper for guarding access to IRenderModule interface
    class RenderModuleGuard : private ::boost::noncopyable
    {
    public:
        explicit RenderModuleGuard( const IRenderModuleSharedPtr& rRenderModule ) :
            mpRenderModule( rRenderModule )
        {
            mpRenderModule->lock();
        }

        ~RenderModuleGuard()
        {
            mpRenderModule->unlock();
        }

    private:
        const IRenderModuleSharedPtr mpRenderModule;
    };
}

#endif /* INCLUDED_CANVAS_IRENDERMODULE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
