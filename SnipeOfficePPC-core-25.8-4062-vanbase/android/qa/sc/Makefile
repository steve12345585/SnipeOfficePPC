include ../../../config_host.mk

# The package of this app
APP_PACKAGE=org.libreoffice.android.qa.sc

# The name of the Bootstrap activity class
BOOTSTRAP=org.libreoffice.android.Bootstrap

# The full path the the app's directory needs to be supplied in a
# couple of places, unfortunately.

APP_DATA_PATH=/data/data/$(APP_PACKAGE)

SODEST=libs/$(ANDROID_APP_ABI)
OBJLOCAL=obj/local/$(ANDROID_APP_ABI)

define COPY
cp $(1) $(SODEST)$(if $(2),/$(2)) && $(STRIP) --strip-debug $(SODEST)$(if $(2),/$(2),/$(notdir $(1))) && \
cp $(1) $(OBJLOCAL)$(if $(2),/$(2))
endef

# The default target just builds.

all: build-ant

properties:
	echo sdk.dir=$(ANDROID_SDK_HOME) >local.properties
	echo sdk.dir=$(ANDROID_SDK_HOME) >../../Bootstrap/local.properties 
	echo "# File needed by ndk-gdb" >jni/Application.mk
	echo "APP_ABI := $(ANDROID_APP_ABI)" >>jni/Application.mk
	echo "APP_PLATFORM := android-14" >>jni/Application.mk

copy-stuff:
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
# Then the cppunittester "program". As all "program"
# files we build for Android actually is a shared object.
	$(call COPY,$(OUTDIR)/bin/cppunit/cppunittester,libcppunittester.so)
#
# The cppunit library
#
	$(call COPY,$(OUTDIR)/lib/libcppunit-1.13.so)
#
# The tests we run here
#
	for F in sc_ucalc sc_filters_test; do \
	    $(call COPY,$(WORKDIR)/LinkTarget/CppunitTest/libtest_$${F}.so); \
	done
#
# Libs and UNO components that the tests need.
#
	for F in $(strip \
		  analysislo \
		  basegfxlo \
		  bootstrap.uno \
		  comphelpgcc3 \
		  datelo \
		  expwrap.uno \
		  fastsax.uno \
		  frmlo \
		  gcc3_uno \
		  i18nisolang1gcc3 \
		  icudatalo \
		  icui18nlo \
		  iculelo \
		  icuuclo \
		  introspection.uno \
		  jvmaccessgcc3 \
		  jvmfwk \
		  libotouchlo \
		  localedata_en \
		  localedata_es \
		  localedata_euro \
		  localedata_others \
		  mergedlo \
		  msfilterlo \
		  ooxlo \
		  reflection.uno \
		  reg \
		  sclo \
		  scfiltlo \
		  stocservices.uno \
		  store \
		  textinstream.uno \
		  test \
		  tllo \
		  ucbhelper4gcc3 \
		  uno_cppu \
		  uno_cppuhelpergcc3 \
		  uno_sal \
		  uno_salhelpergcc3 \
		  unobootstrapprotector \
		  unoexceptionprotector \
		  unotest \
		  unoxmllo \
		  vbahelperlo \
		  xml2 \
		  xmlreader \
		  ); do \
	    $(call COPY,$(OUTDIR)/lib/lib$${F}.so); \
	done
#
# Then the shared GNU C++ library
	$(call COPY,$(ANDROID_NDK_HOME)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_NDK_TOOLCHAIN_VERSION_SUBDIR)libs/$(ANDROID_APP_ABI)/libgnustl_shared.so)
#
# Then other "assets" that can be left in the .apk. Let the directory
# structure under assets mimic that under solver or workdir for now.
	mkdir -p assets/bin assets/bin/ure assets/lib assets/xml/ure
	cp $(OUTDIR)/bin/udkapi.rdb assets/bin
	cp $(OUTDIR)/bin/types.rdb assets/bin
	cp $(OUTDIR)/bin/ure/types.rdb assets/bin/ure
#
	for F in xml/services xml/ure/services; do \
		cp $(OUTDIR)/$$F.rdb assets/$$F.rdb; \
	done
# For some reason the vnd.sun.star.expand:$LO_LIB_DIR doesn't seem to work, it expands to empty!?
# So just hardcode the known APP_DATA_PATH for now...
	for F in framework/util/fwk i18npool/util/i18npool sfx2/util/sfx ucb/source/core/ucb1 ucb/source/ucp/file/ucpfile1 unoxml/source/service/unoxml configmgr/source/configmgr basic/util/sb chart2/source/controller/chartcontroller chart2/source/tools/charttools chart2/source/model/chartmodel comphelper/util/comphelp dbaccess/util/dba eventattacher/source/evtatt fileaccess/source/fileacc filter/source/config/cache/filterconfig1 forms/util/frm oox/util/oox package/source/xstor/xstor package/util/package2 sax/source/expatwrap/expwrap sax/source/fastparser/fastsax sc/util/sc sc/util/scfilt scaddins/source/analysis/analysis scaddins/source/datefunc/date sot/util/sot svl/util/svl toolkit/util/tk ucb/source/ucp/tdoc/ucptdoc1 unotools/util/utl unoxml/source/rdf/unordf; do \
		mkdir -p assets/ComponentTarget/`dirname $$F`; \
	        sed -e 's!uri="vnd.sun.star.expand:$$LO_LIB_DIR/!uri="$(APP_DATA_PATH)/lib/!g' <$(WORKDIR)/ComponentTarget/$$F.component >assets/ComponentTarget/$$F.component; \
	done
	cp -R $(OUTDIR)/xml/registry assets/xml
#
	mkdir -p assets/share/registry/res assets/share/config/soffice.cfg
	cp $(OUTDIR)/xml/*.xcd assets/share/registry
	mv assets/share/registry/fcfg_langpack_en-US.xcd assets/share/registry/res
	cp -R $(OUTDIR)/xml/uiconfig/* assets/share/config/soffice.cfg
	cp -R $(OUTDIR)/xml/registry/* assets/share/registry
	cp $(OUTDIR)/bin/images_tango.zip assets/share/config/images.zip
#
# .res files
	for F in $(OUTDIR)/bin/*.res; do \
		cp $$F assets/bin; \
	done
#
# rc is mandatory, the "inifile". See BootstrapMap::getBaseIni(). As this app
# doesn't use soffice_main() (at least I think it shouldn't), the
# rtl::Bootstrap::setIniFilename() call there that hardcodes
# /assets/program/lofficerc isn't executed. Instead the hardcoding of
# /assets/rc in BootstrapMap::getBaseIni() gets used.
	echo '[Bootstrap]' > assets/rc
	echo 'URE_BOOTSTRAP=file:///assets/program/fundamentalrc' >> assets/rc
#
# unorc is also mandatory. It seems that it *has* to contain the
# URE_INTERNAL_LIB_DIR, UNO_TYPES and UNO_SERVICES settings(?)
	mkdir -p assets/program
	echo "[Bootstrap]" > assets/program/unorc
	echo "URE_INTERNAL_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/unorc
	echo "UNO_TYPES=file:///assets/bin/udkapi.rdb file:///assets/bin/types.rdb"  >> assets/program/unorc
	echo "UNO_SERVICES=file:///assets/xml/ure/services.rdb file:///assets/ComponentTarget/basic/util/sb.component file:///assets/ComponentTarget/chart2/source/controller/chartcontroller.component file:///assets/ComponentTarget/chart2/source/tools/charttools.component file:///assets/ComponentTarget/chart2/source/model/chartmodel.component file:///assets/ComponentTarget/comphelper/util/comphelp.component file:///assets/ComponentTarget/dbaccess/util/dba.component file:///assets/ComponentTarget/eventattacher/source/evtatt.component file:///assets/ComponentTarget/fileaccess/source/fileacc.component file:///assets/ComponentTarget/filter/source/config/cache/filterconfig1.component file:///assets/ComponentTarget/forms/util/frm.component file:///assets/ComponentTarget/oox/util/oox.component file:///assets/ComponentTarget/package/source/xstor/xstor.component file:///assets/ComponentTarget/package/util/package2.component file:///assets/ComponentTarget/sax/source/expatwrap/expwrap.component file:///assets/ComponentTarget/sax/source/fastparser/fastsax.component file:///assets/ComponentTarget/sc/util/sc.component file:///assets/ComponentTarget/sc/util/scfilt.component file:///assets/ComponentTarget/scaddins/source/analysis/analysis.component file:///assets/ComponentTarget/scaddins/source/datefunc/date.component file:///assets/ComponentTarget/sot/util/sot.component file:///assets/ComponentTarget/svl/util/svl.component file:///assets/ComponentTarget/toolkit/util/tk.component file:///assets/ComponentTarget/ucb/source/ucp/tdoc/ucptdoc1.component file:///assets/ComponentTarget/unotools/util/utl.component file:///assets/ComponentTarget/unoxml/source/rdf/unordf.component file:///assets/ComponentTarget/framework/util/fwk.component file:///assets/ComponentTarget/i18npool/util/i18npool.component file:///assets/ComponentTarget/sfx2/util/sfx.component file:///assets/ComponentTarget/unoxml/source/service/unoxml.component file:///assets/ComponentTarget/configmgr/source/configmgr.component file:///assets/ComponentTarget/ucb/source/core/ucb1.component file:///assets/ComponentTarget/ucb/source/ucp/file/ucpfile1.component" >> assets/program/unorc
#
# Hmm, maybe fundamentalrc, too
	echo '[Bootstrap]' > assets/program/fundamentalrc
	echo "LO_LIB_DIR=file:$(APP_DATA_PATH)/lib/" >> assets/program/fundamentalrc
	echo "URE_LIB_DIR=file://$(APP_DATA_PATH)/lib/" >> assets/program/fundamentalrc # checkme - is this used to find configs ?
	echo 'BRAND_BASE_DIR=file:///assets' >> assets/program/fundamentalrc
	echo 'CONFIGURATION_LAYERS=xcsxcu:$${BRAND_BASE_DIR}/share/registry module:$${BRAND_BASE_DIR}/share/registry/modules res:$${BRAND_BASE_DIR}/share/registry' >> assets/program/fundamentalrc
	echo 'URE_BIN_DIR=file:///assets/ure/bin/dir/not-here/can-we/exec-anyway' >> assets/program/fundamentalrc
	echo 'URE_MORE_TYPES=file:///assets/bin/ure/types.rdb file:///assets/bin/types.rdb' >> assets/program/fundamentalrc
	echo 'URE_MORE_SERVICES=file:///assets/xml/services.rdb <$$BRAND_BASE_DIR/program/services>*' >> assets/program/fundamentalrc

#
# Test documents for filters_test
	(cd $(SRC_ROOT) && tar cf - sc/qa/unit/data) | (cd assets && tar xvf -)
#
# Then assets that are unpacked at run-time into the app's data directory.
	mkdir -p assets/unpack/etc/fonts
	cp fonts.conf assets/unpack/etc/fonts
# The indirect command lines...
	echo "SAL_LOG=+WARN+INFO STAR_RESOURCEPATH=/assets/bin $(APP_DATA_PATH)/lib/libtest_sc_ucalc.so --headless --protector libunoexceptionprotector.so unoexceptionprotector --protector libunobootstrapprotector.so unobootstrapprotector" >assets/unpack/ucalc.cmdline
	echo "SAL_LOG=+WARN+INFO STAR_RESOURCEPATH=/assets/bin $(APP_DATA_PATH)/lib/libtest_sc_filters_test.so --headless --protector libunoexceptionprotector.so unoexceptionprotector --protector libunobootstrapprotector.so unobootstrapprotector" >assets/unpack/filters_test.cmdline
#
# Then gdbserver and gdb.setup so that we can debug with ndk-gdb.
	cp $(ANDROID_NDK_GDBSERVER) $(SODEST)
	echo set solib-search-path ./obj/local/$(ANDROID_APP_ABI) >$(SODEST)/gdb.setup


build-ant: copy-stuff properties
	unset JAVA_HOME && $(ANT) debug

install: copy-stuff properties
	unset JAVA_HOME && $(ANT) debug install
	@echo
	@echo 'Run either "make run_ucalc" or "make run_filters_test"'
	@echo

uninstall:
	$(ANDROID_SDK_HOME)/platform-tools/adb uninstall $(APP_PACKAGE)

run_ucalc:
	$(ANDROID_SDK_HOME)/platform-tools/adb shell am start -n $(APP_PACKAGE)/$(BOOTSTRAP) -e lo-main-library libcppunittester -e lo-main-indirect-cmdline $(APP_DATA_PATH)/ucalc.cmdline

run_filters_test:
	$(ANDROID_SDK_HOME)/platform-tools/adb shell am start -n $(APP_PACKAGE)/$(BOOTSTRAP) -e lo-main-library libcppunittester -e lo-main-indirect-cmdline $(APP_DATA_PATH)/filters_test.cmdline

run:
	@echo Make either run_ucalc or run_filters_test but not both right after each other

# If you reinstall an app several times, even if you uninstall it
# between, disk space seems to leak that won't get recycled until you
# stop and start...
stop-start-cycle:
	$(ANDROID_SDK_HOME)/platform-tools/adb shell stop && $(ANDROID_SDK_HOME)/platform-tools/adb shell start && sleep 10

clean: properties
	$(ANT) clean
	rm -rf assets $(SODEST) $(OBJLOCAL)

