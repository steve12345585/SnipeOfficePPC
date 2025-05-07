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

#ifndef __FRAMEWORK_JOBS_HELPONSTARTUP_HXX_
#define __FRAMEWORK_JOBS_HELPONSTARTUP_HXX_

//_______________________________________________
// my own includes

#include <threadhelp/threadhelpbase.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>

//_______________________________________________
// other includes
#include <cppuhelper/implbase3.hxx>

//_______________________________________________
// uno includes
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/task/XJob.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>

//_______________________________________________
// namespace

namespace framework{

//_______________________________________________
// declarations

//_______________________________________________
/** @short  implements a job component, which handle the special
            feature to show a suitable help page for every (visible!)
            loaded document.

    @author as96863
 */
class HelpOnStartup : private ThreadHelpBase
                      ,public ::cppu::WeakImplHelper3< ::com::sun::star::lang::XServiceInfo,::com::sun::star::lang::XEventListener,::com::sun::star::task::XJob >
{
    //-------------------------------------------
    // member
    private:

        //.......................................
        /** @short  reference to an uno service manager. */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        //.......................................
        /** @short  such module manager is used to classify new opened documents. */
        css::uno::Reference< css::frame::XModuleManager > m_xModuleManager;

        //.......................................
        /** @short  is needed to locate a might open help frame. */
        css::uno::Reference< css::frame::XFrame > m_xDesktop;

        //.......................................
        /** @short  provides read access to the underlying configuration. */
        css::uno::Reference< css::container::XNameAccess > m_xConfig;

        //.......................................
        /** @short  knows the current locale of this office session,
                    which is needed to build complete help URLs.
         */
        ::rtl::OUString m_sLocale;

        //.......................................
        /** @short  knows the current operating system of this office session,
                    which is needed to build complete help URLs.
         */
        ::rtl::OUString m_sSystem;

    //-------------------------------------------
    // native interface
    public:

        //---------------------------------------
        /** @short  create new instance of this class.

            @param  xSMGR
                    reference to the uno service manager, which created this instance.
                    Can be used later to create own needed uno resources on demand.
         */
        HelpOnStartup(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR);

        //---------------------------------------
        /** @short  does nothing real ...

            @descr  But it should exists as virtual function,
                    so this class cant make trouble
                    related to inline/symbols etcpp.!
         */
        virtual ~HelpOnStartup();

    //-------------------------------------------
    // uno interface
    public:

        //---------------------------------------
        // css.lang.XServiceInfo
        DECLARE_XSERVICEINFO

        // css.task.XJob
        virtual css::uno::Any SAL_CALL execute(const css::uno::Sequence< css::beans::NamedValue >& lArguments)
            throw(css::lang::IllegalArgumentException,
                  css::uno::Exception                ,
                  css::uno::RuntimeException         );

        // css.lang.XEventListener
        virtual void SAL_CALL disposing(const css::lang::EventObject& aEvent)
            throw(css::uno::RuntimeException);

    //-------------------------------------------
    // helper
    private:

        //---------------------------------------
        /** @short  analyze the given job arguments, try to locate a model reference
                    and try to classify this model.

            @descr  As a result of this operation a module identifier will be returned.
                    It can be used against the module configuration then to retrieve further informations.

            @param  lArguments
                    the list of job arguments which is given on our interface method execute().

            @return [string]
                    a module identifier ... or an empty value if no model could be located ...
                    or if it could not be classified successfully.
         */
        ::rtl::OUString its_getModuleIdFromEnv(const css::uno::Sequence< css::beans::NamedValue >& lArguments);

        //---------------------------------------
        /** @short  tries to locate the open help module and return
                    the url of the currently shown help content.

            @descr  It returns an empty string, if the help isnt still
                    open at calling time.

            @return The URL of the current shown help content;
                    or an empty value if the help isnt still open.
         */
        ::rtl::OUString its_getCurrentHelpURL();

        //---------------------------------------
        /** @short  checks if the given help url match to a default help url
                    of any office module.

            @param  sHelpURL
                    the help url for checking.

            @return [bool]
                    sal_True if the given URL is any default one ...
                    sal_False otherwise.
         */
        ::sal_Bool its_isHelpUrlADefaultOne(const ::rtl::OUString& sHelpURL);

        //---------------------------------------
        /** @short  checks, if the help module should be shown automaticly for the
                    currently opened office module.

            @descr  This value is readed from the module configuration.
                    In case the help should be shown, this method returns
                    a help URL, which can be used to show the right help content.

            @param  sModule
                    identifies the used office module.

            @return [string]
                    A valid help URL in case the help content should be shown;
                    an empty value if such automatism was disabled for the specified office module.
         */
        ::rtl::OUString its_checkIfHelpEnabledAndGetURL(const ::rtl::OUString& sModule);

        //---------------------------------------
        /** @short  create a help URL for the given parameters.

            @param  sBaseURL
                    must be the base URL for a requested help content
                    e.g. "vnd.sun.star.help://swriter/"
                    or   "vnd.sun.star.help://swriter/67351"

            @param  sLocale
                    the current office locale
                    e.g. "en-US"

            @param  sSystem
                    the current operating system
                    e.g. "WIN"

            @return The URL which was generated.
                    e.g.
                    e.g. "vnd.sun.star.help://swriter/?Language=en-US&System=WIN"
                    or   "vnd.sun.star.help://swriter/67351?Language=en-US&System=WIN"
         */
        static ::rtl::OUString ist_createHelpURL(const ::rtl::OUString& sBaseURL,
                                                 const ::rtl::OUString& sLocale ,
                                                 const ::rtl::OUString& sSystem );
};

} // namespace framework

#endif // __FRAMEWORK_JOBS_HELPONSTARTUP_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
