/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "osl/detail/android-bootstrap.h"

extern "C"
{
    extern void * animcore_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * avmedia_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * dba_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * dbaxml_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * evtatt_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * fileacc_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * frm_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * fsstorage_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * fwk_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * fwl_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * fwm_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * hwp_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * hyphen_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * lng_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * lnth_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * lotuswordpro_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * oox_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * sb_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * sc_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * scd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * scfilt_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * sd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * sdd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * sm_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * smd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * spell_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * svgfilter_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * sw_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * swd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * t602filter_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * textfd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * unoxml_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * unordf_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * wpftdraw_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * wpftwriter_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * xmlfd_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * xmlsecurity_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * xo_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
    extern void * xof_component_getFactory( const char * pImplName, void * pServiceManager, void * pRegistryKey );
}

extern "C"
__attribute__ ((visibility("default")))
const lib_to_component_mapping *
lo_get_libmap(void)
{
    static lib_to_component_mapping map[] = {
        { "libanimcorelo.a", animcore_component_getFactory },
        { "libavmedialo.a", avmedia_component_getFactory },
        { "libdbalo.a", dba_component_getFactory },
        { "libdbaxmllo.a", dbaxml_component_getFactory },
        { "libevtattlo.a", evtatt_component_getFactory },
        { "libfileacc.a", fileacc_component_getFactory },
        { "libfrmlo.a", frm_component_getFactory },
        { "libfsstorage.uno.a", fsstorage_component_getFactory },
        { "libfwklo.a", fwk_component_getFactory },
        { "libfwllo.a", fwl_component_getFactory },
        { "libfwmlo.a", fwm_component_getFactory },
        { "libhwplo.a", hwp_component_getFactory },
        { "libhyphenlo.a", hyphen_component_getFactory },
        { "liblnglo.a", lng_component_getFactory },
        { "liblnthlo.a", lnth_component_getFactory },
        { "liblwpftlo.a", lotuswordpro_component_getFactory },
        { "libooxlo.a", oox_component_getFactory },
        { "libscdlo.a", scd_component_getFactory },
        { "libscfiltlo.a", scfilt_component_getFactory },
        { "libsblo.a", sb_component_getFactory },
        { "libsclo.a", sc_component_getFactory },
        { "libsddlo.a", sdd_component_getFactory },
        { "libsdlo.a", sd_component_getFactory },
        { "libsmdlo.a", smd_component_getFactory },
        { "libsmlo.a", sm_component_getFactory },
        { "libsvgfilterlo.a", svgfilter_component_getFactory },
        { "libswdlo.a", swd_component_getFactory },
        { "libswlo.a", sw_component_getFactory },
        { "libt602filterlo.a", t602filter_component_getFactory },
        { "libtextfdlo.a", textfd_component_getFactory },
        { "libunordflo.a", unordf_component_getFactory },
        { "libunoxmllo.a", unoxml_component_getFactory },
        { "libwpftdrawlo.a", wpftdraw_component_getFactory },
        { "libwpftwriterlo.a", wpftwriter_component_getFactory },
        { "libxmlfdlo.a", xmlfd_component_getFactory },
        { "libxmlsecurity.a", xmlsecurity_component_getFactory },
        { "libxoflo.a", xof_component_getFactory },
        { "libxolo.a", xo_component_getFactory },
        { NULL, NULL }
    };

    return map;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
