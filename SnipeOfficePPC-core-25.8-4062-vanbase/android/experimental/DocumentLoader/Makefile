include ../../../config_host.mk

BOOTSTRAPDIR=../../Bootstrap
include $(BOOTSTRAPDIR)/Makefile.shared

# The package of this app
APP_PACKAGE=org.libreoffice.android.examples

# We can't keep assuming APP_DATA_PATH like this, surely this can vary with
# Android versions and whatnot, this is temporary and works at least with the
# SDK 16 emulator...

# Probably would be best to just stop fooling around with the possibilities to
# set various stuff with the -env command line parameters (and environment
# variables?) and in a plethora of rc files, and hardcode construction of
# *all* required pathnames based on the app installation location for Android
# (and iOS), etc. We don't really win anything by having so many layers of
# configurability on platforms like Android and iOS where apps based on LO
# code are very much self-contained pre-packaged thingies.
APP_DATA_PATH=/data/data/$(APP_PACKAGE)

SODEST=libs/$(ANDROID_APP_ABI)
OBJLOCAL=obj/local/$(ANDROID_APP_ABI)

define COPYJAR
cp $(1) libs
endef

LIBS = \
  -Wl,--start-group \
  $(wildcard $(OUTDIR)/lib/lib*.a) \
  -Wl,--end-group

WHOLELIBS = \
  -Wl,--whole-archive \
  $(addprefix -l,$(strip \
	juh \
  )) \
  -Wl,--no-whole-archive

# The default target just builds.

all: build-ant

link-so:
# Build the single .so for this app
	mkdir -p $(OBJLOCAL)
	$(CXX) -Wl,-Map,liblo-native-code.map -Wl,--gc-sections -Wl,--version-script=../../Bootstrap/version.map -Wl,--no-keep-files-mapped -Wl,--no-undefined -DANDROID -DDISABLE_DYNLOADING -shared -o $(OBJLOCAL)/liblo-native-code.so -I$(OUTDIR)/inc native-code.cxx -L$(OUTDIR)/lib $(WHOLELIBS) $(LIBS) -lgnustl_static -landroid -llog -lz
	mkdir -p $(SODEST)
	$(STRIP) -o $(SODEST)/liblo-native-code.so $(OBJLOCAL)/liblo-native-code.so

copy-stuff:
# Then "assets". Let the directory structure under assets mimic
# that under solver for now.
#
# Please note that I have no idea what all of this is really necessary and for
# much of this stuff being copied, no idea whether it makes any sense at all.
# Much of this is copy-pasted from android/qa/sc/Makefile (where a couple of
# unit tests for sc are built, and those do seem to mostly work) and
# android/qa/desktop/Makefile (mmeeks's desktop demo, also works to some
# extent).
#
	mkdir -p assets/gz.unpack/program/ure assets/lib assets/program assets/xml/ure assets/ComponentTarget/i18npool/util
	gzip -9 <$(OUTDIR)/bin/offapi.rdb >assets/gz.unpack/program/offapi.rdb
	gzip -9 <$(OUTDIR)/bin/oovbaapi.rdb >assets/gz.unpack/program/oovbaapi.rdb
	gzip -9 <$(OUTDIR)/bin/types.rdb >assets/gz.unpack/program/types.rdb
	gzip -9 <$(OUTDIR)/bin/ure/types.rdb >assets/gz.unpack/program/ure/types.rdb
# For some reason the vnd.sun.star.expand:$LO_LIB_DIR doesn't seem to work, it expands to empty!?
# So just hardcode the known APP_DATA_PATH for now...
	for F in xml/services xml/ure/services; do \
		sed -e 's!uri="vnd.sun.star.expand:$$LO_LIB_DIR/!uri="file://$(APP_DATA_PATH)/lib/!g' <$(OUTDIR)/$$F.rdb >assets/$$F.rdb; \
	done
	cp $(SRC_ROOT)/odk/examples/java/DocumentHandling/test/test1.odt \
	   $(SRC_ROOT)/sc/qa/unit/data/xls/border.xls \
	   $(SRC_ROOT)/sw/qa/core/data/odt/test.odt \
	   $(SRC_ROOT)/sw/qa/core/data/doc/testVba.doc \
		assets
	cp $(WORKDIR)/ComponentTarget/i18npool/util/i18npool.component assets/ComponentTarget/i18npool/util
#
	mkdir -p assets/ure/share/misc assets/share/registry/res assets/share/config/soffice.cfg
	cp -R $(OUTDIR)/xml/*.xcd assets/share/registry
	mv assets/share/registry/fcfg_langpack_en-US.xcd assets/share/registry/res
	cp -R $(OUTDIR)/xml/uiconfig/* assets/share/config/soffice.cfg
	cp -R $(OUTDIR)/xml/registry/* assets/share/registry
#
# Set up rc, the "inifile". See BootstrapMap::getBaseIni(). As this app
# doesn't use soffice_main() (at least I think it shouldn't), the
# rtl::Bootstrap::setIniFilename() call there that hardcodes
# /assets/program/lofficerc isn't executed. Instead the hardcoding of
# /assets/rc in BootstrapMap::getBaseIni() gets used.
	echo '[Bootstrap]' > assets/rc
	echo 'Logo=1' >> assets/rc
	echo 'NativeProgress=1' >> assets/rc
	echo 'URE_BOOTSTRAP=file:///assets/program/fundamentalrc' >> assets/rc
#	echo 'RTL_LOGFILE=file:///dev/log/main' >> assets/rc
	echo "HOME=$(APP_DATA_PATH)/cache" >> assets/rc
	echo "OSL_SOCKET_PATH=$(APP_DATA_PATH)/cache" >> assets/rc
#
# Set up fundamentalrc
	echo '[Bootstrap]' > assets/program/fundamentalrc
	echo "LO_LIB_DIR=file:$(APP_DATA_PATH)/lib/" >> assets/program/fundamentalrc
	echo "URE_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/fundamentalrc # checkme - is this used to find configs ?
	echo 'BRAND_BASE_DIR=file:///assets' >> assets/program/fundamentalrc
	echo 'CONFIGURATION_LAYERS=xcsxcu:$${BRAND_BASE_DIR}/share/registry module:$${BRAND_BASE_DIR}/share/registry/modules res:$${BRAND_BASE_DIR}/share/registry' >> assets/program/fundamentalrc
	echo 'URE_BIN_DIR=file:///assets/ure/bin/dir/not-here/can-we/exec-anyway' >> assets/program/fundamentalrc
	echo 'URE_MORE_TYPES=$(APP_DATA_PATH)/program/offapi.rdb $(APP_DATA_PATH)/program/oovbaapi.rdb' >> assets/program/fundamentalrc
	echo 'URE_MORE_SERVICES=file:///assets/xml/services.rdb <$$BRAND_BASE_DIR/program/services>*' >> assets/program/fundamentalrc
#
# Set up unorc
	echo '[Bootstrap]' > assets/program/unorc
	echo "URE_INTERNAL_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/unorc
	echo 'UNO_TYPES=$(APP_DATA_PATH)/program/ure/types.rdb $(APP_DATA_PATH)/program/types.rdb $${URE_MORE_TYPES}' >> assets/program/unorc
	echo 'UNO_SERVICES=file:///assets/xml/ure/services.rdb $${URE_MORE_SERVICES}' >> assets/program/unorc
#
# Set up bootstraprc
	echo '[Bootstrap]' > assets/program/bootstraprc
	echo 'InstallMode=<installmode>' >> assets/program/bootstraprc
	echo 'ProductKey=LibreOffice 3.6' >> assets/program/bootstraprc
	echo "UserInstallation=file://$(APP_DATA_PATH)" >> assets/program/bootstraprc
#
# Set up versionrc
	echo '[Version]' > assets/program/versionrc
	echo 'AllLanguages=en-US' >> assets/program/versionrc
	echo 'BuildVersion=' >> assets/program/versionrc
	echo 'buildid=dead-beef' >> assets/program/versionrc
	echo 'ProductBuildid=3' >> assets/program/versionrc
	echo 'ProductMajor=360' >> assets/program/versionrc
	echo 'ProductMinor=1' >> assets/program/versionrc
	echo 'ProductSource=OOO350' >> assets/program/versionrc
	echo 'ReferenceOOoMajorMinor=3.6' >> assets/program/versionrc
#
# .res files
	mkdir -p assets/program/resource
	cp $(OUTDIR)/bin/*en-US.res assets/program/resource
#
# Assets that are unpacked at run-time into the app's data directory. These
# are files read by non-LO code, fontconfig and freetype for now, that doesn't
# understand "/assets" paths.
	mkdir -p assets/unpack/etc/fonts
	cp fonts.conf assets/unpack/etc/fonts
# $UserInstallation/user/fonts is added to the fontconfig path in
# vcl/generic/fontmanager/helper.cxx: psp::getFontPath(). UserInstallation is
# set to the app's data dir above.
	mkdir -p assets/gz.unpack/user/fonts
	for F in $(OUTDIR)/pck/Liberation*.ttf $(OUTDIR)/pck/Gen*.ttf $(OUTDIR)/pck/opens___.ttf; do \
		gzip -9 <$$F >assets/gz.unpack/user/fonts/`basename $$F`; \
	done
#
# Then gdbserver and gdb.setup so that we can debug with ndk-gdb.
#
	mkdir -p $(SODEST)
	cp $(ANDROID_NDK_GDBSERVER) $(SODEST)
	echo set solib-search-path ./obj/local/$(ANDROID_APP_ABI) >$(SODEST)/gdb.setup

build-ant: android_version_setup copy-stuff link-so properties
#
# Copy jar files we need
#
	for F in $(strip \
		   java_uno \
		   juh \
		   jurt \
		   ridl \
		   unoil \
		   unoloader \
		  ); do \
	    $(call COPYJAR,$(OUTDIR)/bin/$${F}.jar); \
	done
#
	unset JAVA_HOME && $(ANT) debug

run:
	adb shell am start -n org.libreoffice.android.examples/.DocumentLoader -e input /assets/test1.odt
