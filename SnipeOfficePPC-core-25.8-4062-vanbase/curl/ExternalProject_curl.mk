# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalProject_ExternalProject,curl))

$(eval $(call gb_ExternalProject_use_unpacked,curl,curl))

$(eval $(call gb_ExternalProject_register_targets,curl,\
	build \
))

ifeq ($(GUI),UNX)

curl_CPPFLAGS :=
curl_LDFLAGS :=

ifneq ($(SYSBASE),)
curl_CPPFLAGS += -I$(SYSBASE)/usr/include
curl_LDFLAGS += -L$(SYSBASE)/usr/lib
endif

# there are 2 include paths, what is passed to --with-nss below is for libs...
ifeq ($(SYSTEM_NSS),NO)
curl_CPPFLAGS += -I$(OUTDIR)/inc/mozilla/nspr -I$(OUTDIR)/inc/mozilla/nss
endif

$(call gb_ExternalProject_get_state_target,curl,build):
	cd $(EXTERNAL_WORKDIR) \
	&&  CPPFLAGS="$(curl_CPPFLAGS)" \
		LDFLAGS="$(curl_LDFLAGS)" \
	./configure \
			--with-nss$(if $(filter NO,$(SYSTEM_NSS)),="$(OUTDIR)/") \
			--without-ssl \
	--without-libidn --enable-ftp --enable-ipv6 --enable-http --disable-gopher \
	--disable-file --disable-ldap --disable-telnet --disable-dict --without-libssh2 \
	$(if $(filter YES,$(CROSS_COMPILING)),--build=$(BUILD_PLATFORM) --host=$(HOST_PLATFORM)) \
	$(if $(filter TRUE,$(DISABLE_DYNLOADING)),--disable-shared,--disable-static) \
	$(if $(filter TRUE,$(ENABLE_DEBUG)),--enable-debug) \
	&& cd lib \
	&& $(MAKE) \
	&& touch $@

else ifeq ($(OS)$(COM),WNTGCC)

$(call gb_ExternalProject_get_state_target,curl,build):
	cd $(EXTERNAL_WORKDIR) \
	&& PATH=$(OUTDIR)/bin:$$PATH ./configure --with-nss --without-ssl --enable-ftp --enable-ipv6 --disable-http --disable-gopher \
	--disable-file --disable-ldap --disable-telnet --disable-dict --build=i586-pc-mingw32 --host=i586-pc-mingw32 \
	$(if $(filter TRUE,$(ENABLE_DEBUG)),--enable-debug) \
	CC="$(CC) -mthreads $(if $(filter YES,$(MINGW_SHARED_GCCLIB)),-shared-libgcc)" \
	LIBS="-lws2_32 -lwinmm $(if $(filter YES,$(MINGW_SHARED_GXXLIB)),$(MINGW_SHARED_LIBSTDCPP))" \
	LDFLAGS="$(patsubst ;, -L,$(ILIB))" \
	CPPFLAGS="$(INCLUDE)" OBJDUMP="objdump" \
	&& cd lib \
	&& $(MAKE) \
	&& touch $@

else ifeq ($(COM),MSC)

$(call gb_ExternalProject_get_state_target,curl,build):
	cd $(EXTERNAL_WORKDIR)/lib \
	&& MAKEFLAGS= LIB="$(ILIB)" nmake -f Makefile.vc9 cfg=release-dll \
		EXCFLAGS="/EHa /Zc:wchar_t- /D_CRT_SECURE_NO_DEPRECATE /DUSE_WINDOWS_SSPI $(SOLARINC)" $(if $(filter X86_64,$(CPUNAME)),MACHINE=X64) \
	&& touch $@

endif

# vim: set noet sw=4 ts=4:
