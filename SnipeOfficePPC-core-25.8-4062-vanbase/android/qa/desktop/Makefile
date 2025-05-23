# lo-main-library - parameter denoting which shlib to load and execute
# 'lo_main' from

include ../../../config_host.mk

BOOTSTRAPDIR=../../Bootstrap
include $(BOOTSTRAPDIR)/Makefile.shared

# The package of this app
APP_PACKAGE=org.libreoffice.android.libo

# The name of the Bootstrap activity class
BOOTSTRAP=org.libreoffice.android.Bootstrap

# The default target just builds.

all: build-ant

buildrcs:
# main sofficerc ini ...
	mkdir -p assets/program/
	echo "[Bootstrap]" > assets/program/lofficerc
	echo "Logo=1" >> assets/program/lofficerc
	echo "NativeProgress=1" >> assets/program/lofficerc
	echo "URE_BOOTSTRAP=file:///assets/program/fundamentalrc" >> assets/program/lofficerc
#	echo "RTL_LOGFILE=file:///dev/log/main" >> assets/program/lofficerc
	echo "HOME=$(APP_DATA_PATH)/files" >> assets/program/lofficerc
	echo "OSL_SOCKET_PATH=$(APP_DATA_PATH)/files" >> assets/program/lofficerc
# - this looks useful but breaks more than it fixes ...
#	echo "DISABLE_EXTENSION_SYNCHRONIZATION=1" >> assets/program/lofficerc

# fundamentalrc ini ...
	echo "[Bootstrap]" > assets/program/fundamentalrc
	echo "LO_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/fundamentalrc
	echo "URE_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/fundamentalrc # checkme - is this used to find configs ?
	echo "BRAND_BASE_DIR=file:///assets" >> assets/program/fundamentalrc
	echo 'CONFIGURATION_LAYERS=xcsxcu:$${BRAND_BASE_DIR}/share/registry module:$${BRAND_BASE_DIR}/share/registry/modules res:$${BRAND_BASE_DIR}/share/registry' >> assets/program/fundamentalrc
	echo 'URE_BIN_DIR=file:///assets/ure/bin/dir/not-here/can-we/exec-anyway' >> assets/program/fundamentalrc
	echo 'URE_MORE_TYPES=file:///assets/ure/share/misc/types.rdb file:///assets/program/types/types.rdb' >> assets/program/fundamentalrc
	echo 'URE_MORE_SERVICES= <$$BRAND_BASE_DIR/program/services>*' >> assets/program/fundamentalrc
	echo 'BUNDLED_EXTENSIONS=$$BRAND_BASE_DIR/share/extensions' >> assets/program/fundamentalrc
	echo 'BUNDLED_EXTENSIONS_USER=$${$$BRAND_BASE_DIR/program/bootstraprc:UserInstallation}/user/extensions/bundled' >> assets/program/fundamentalrc
	echo 'TMP_EXTENSIONS=$${$$BRAND_BASE_DIR/program/bootstraprc:UserInstallation}/user/extensions/tmp' >> assets/program/fundamentalrc
	echo 'SHARED_EXTENSIONS_USER=$${$$BRAND_BASE_DIR/program/bootstraprc:UserInstallation}/user/extensions/shared' >> assets/program/fundamentalrc
	echo 'UNO_SHARED_PACKAGES=$$BRAND_BASE_DIR/share/uno_packages' >> assets/program/fundamentalrc
	echo 'UNO_SHARED_PACKAGES_CACHE=$$UNO_SHARED_PACKAGES/cache' >> assets/program/fundamentalrc
	echo 'UNO_USER_PACKAGES=$${$$BRAND_BASE_DIR/program/bootstraprc:UserInstallation}/user/uno_packages' >> assets/program/fundamentalrc
	echo 'UNO_USER_PACKAGES_CACHE=$$UNO_USER_PACKAGES/cache' >> assets/program/fundamentalrc
	echo 'PKG_BundledUnoFile=$$BUNDLED_EXTENSIONS_USER/registry/com.sun.star.comp.deployment.component.PackageRegistryBackend/unorc' >> assets/program/fundamentalrc
	echo 'PKG_SharedUnoFile=$$SHARED_EXTENSIONS_USER/registry/com.sun.star.comp.deployment.component.PackageRegistryBackend/unorc' >> assets/program/fundamentalrc
	echo 'PKG_UserUnoFile=$$UNO_USER_PACKAGES_CACHE/registry/com.sun.star.comp.deployment.component.PackageRegistryBackend/unorc' >> assets/program/fundamentalrc

# unorc ini ...
	echo "[Bootstrap]" > assets/program/unorc
	echo "URE_INTERNAL_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/unorc
	echo 'UNO_TYPES=file:///assets/ure/share/misc/types.rdb file:///assets/program/types/types.rdb $${URE_MORE_TYPES}' >> assets/program/unorc
	echo 'UNO_SERVICES=file:///assets/ure/share/misc/services.rdb $${URE_MORE_SERVICES}' >> assets/program/unorc
# bootstraprc ini ...
	echo "[Bootstrap]" > assets/program/bootstraprc
	echo "InstallMode=<installmode>" >> assets/program/bootstraprc
	echo "ProductKey=LibreOffice 3.5" >> assets/program/bootstraprc
	echo "UserInstallation=file://$(APP_DATA_PATH)/files/.libreoffice" >> assets/program/bootstraprc

# versionrc ini ...
	echo "[Version]" > assets/program/versionrc
	echo "AllLanguages=en-US" >> assets/program/versionrc
	echo "BuildVersion=" >> assets/program/versionrc
	echo "buildid=dead-beef" >> assets/program/versionrc
	echo "ExtensionUpdateURL=http://updateexte.libreoffice.org/ExtensionUpdateService/check.Update" >> assets/program/versionrc
	echo "ProductBuildid=3" >> assets/program/versionrc
	echo "ProductMajor=360" >> assets/program/versionrc
	echo "ProductMinor=1" >> assets/program/versionrc
	echo "ProductSource=OOO350" >> assets/program/versionrc
	echo "ReferenceOOoMajorMinor=3.6" >> assets/program/versionrc
	echo "UpdateID=LibreOffice_3_en-US" >> assets/program/versionrc
	echo "UpdateURL=" >> assets/program/versionrc
	echo "UpdateUserAgent=<PRODUCT> (${buildid}; ${_OS}; ${_ARCH}; BundledLanguages=${AllLanguages})" >> assets/program/versionrc
	echo "Vendor=SelfCompiledGit" >> assets/program/versionrc

copy-stuff: buildrcs
#
# Copy shared libraries we need to libs/$(ANDROID_APP_ABI) so that ant will
# include them in the .apk.
#
# Copy them to obj/local/$(ANDROID_APP_ABI), too, where gdb will look for
# them.
#
	mkdir -p $(SODEST) $(OBJLOCAL)
#
# First the most important one, liblo-bootstrap.so
	$(call COPY,$(OUTDIR)/lib/liblo-bootstrap.so)
#
# Libs and UNO components that we need.
#
	for F in $(strip \
		  analysislo \
		  basegfxlo \
		  bootstrap.uno \
		  comphelpgcc3 \
		  cppcanvaslo \
		  embobj \
		  expwrap.uno \
		  gcc3_uno \
		  i18nisolang1gcc3 \
		  icudatalo \
		  icui18nlo \
		  iculelo \
		  icuuclo \
		  jvmaccessgcc3 \
		  jvmfwk \
		  libotouchlo \
		  introspection.uno \
		  localedata_en \
		  localedata_es \
		  localedata_euro \
		  localedata_others \
		  mergedlo \
		  msfilterlo \
		  ooxlo \
		  reflection.uno \
		  reg \
		  stocservices.uno \
		  store \
		  test \
		  tllo \
		  ucbhelper4gcc3 \
		  uno_cppu \
		  uno_cppuhelpergcc3 \
		  uno_sal \
		  uno_salhelpergcc3 \
		  unordflo \
		  unotest \
		  unoxmllo \
		  vbahelperlo \
		  xml2 \
		  xmlreader \
		  xmlsecurity \
		  xslt \
		  \
		  swlo \
		  swuilo \
		  swdlo \
		  \
		  sclo \
		  scdlo \
		  scnlo \
		  scuilo \
		  scfiltlo \
		  \
		  chartcontrollerlo \
		  chartcorelo \
		  ); do \
	    $(call COPYSO,$(OUTDIR)/lib/lib$${F}.so); \
	done

#
# Then the shared GNU C++ library
	$(call COPYSO,$(ANDROID_NDK_HOME)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_NDK_TOOLCHAIN_VERSION_SUBDIR)libs/$(ANDROID_APP_ABI)/libgnustl_shared.so)
#
# Then other "assets" that can be left in the .apk. Let the directory
# structure under assets mimic the install set - except for the fact
# that program/ is renamed to lib/ and we put the ure/lib .so's into
# the assets/lib directory too.
	mkdir -p assets/program/types assets/program/services
	mkdir -p assets/program/resource
	mkdir -p assets/ure/share/misc assets/share/registry/res
	mkdir -p assets/share/config/soffice.cfg
# uno types & services
	cp $(OUTDIR)/bin/types.rdb assets/program/types/
	cp $(OUTDIR)/bin/ure/types.rdb assets/ure/share/misc/
	cp $(OUTDIR)/xml/services.rdb assets/program/services
	cp $(OUTDIR)/xml/ure/services.rdb assets/ure/share/misc/
# config
	cp -R $(OUTDIR)/xml/*.xcd assets/share/registry/
	mv assets/share/registry/fcfg_langpack_en-US.xcd assets/share/registry/res/
	cp -R $(OUTDIR)/xml/uiconfig/* assets/share/config/soffice.cfg
	cp -R $(OUTDIR)/bin/images_tango.zip assets/share/config/images.zip
# .res files
	cp $(OUTDIR)/bin/*en-US.res assets/program/resource/
# presets - becomes the users home directory, .stamp forces .zip dir creation
	for D in $(strip autocorr autotext backup basic config gallery \
	         psprint/driver psprint/fontmetric template \
		 uno_packages/cache wordbook); do \
		mkdir -p assets/presets/$$D ; \
		echo "content" > assets/presets/$$D/stamp; \
	done
# shell / splash images
	cp -r $(SRC_ROOT)/icon-themes/galaxy/brand/* assets/program
# Then assets that are unpacked at run-time into the app's data directory.
	mkdir -p assets/unpack/etc/fonts
	cp fonts.conf assets/unpack/etc/fonts
# A sample file for fun ...
	mkdir -p assets/demo
	if test -f /opt/libreoffice/tmp/writer.odt; then cp /opt/libreoffice/tmp/writer.odt assets/demo; fi
#
# Then gdbserver and gdb.setup so that we can debug with ndk-gdb.
#
	cp $(ANDROID_NDK_GDBSERVER) $(SODEST)
	echo set solib-search-path ./obj/local/$(ANDROID_APP_ABI) >$(SODEST)/gdb.setup


build-ant: copy-stuff properties
	unset JAVA_HOME && $(ANT) debug

run:
	echo "-env:INIFILENAME=file:///assets/program/sofficerc" > cmdline
#	echo "file:///assets/demo/writer.odt" > cmdline
	$(ANDROID_SDK_HOME)/platform-tools/adb push cmdline $(APP_DATA_PATH)/cmdline
	$(ANDROID_SDK_HOME)/platform-tools/adb shell am start -n $(APP_PACKAGE)/$(BOOTSTRAP) -e lo-main-delay 20 -e lo-main-indirect-cmdline "$(APP_DATA_PATH)/cmdline"
# add -e lo-strace yes # if you want that
