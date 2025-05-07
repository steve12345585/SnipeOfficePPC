$(eval $(call gb_CppunitTest_CppunitTest,basic_enable))

$(eval $(call gb_CppunitTest_add_exception_objects,basic_enable, \
    basic/qa/cppunit/test_append \
))

# add a list of all needed libraries here
$(eval $(call gb_CppunitTest_use_libraries,basic_enable, \
    comphelper \
    cppu \
    cppuhelper \
    sal \
    salhelper \
    sb \
    sot \
    svl \
    svt \
    tl \
    utl \
    vcl \
    xcr \
    test \
    $(gb_STDLIBS) \
))

ifeq ($(GUI),WNT)
$(eval $(call gb_CppunitTest_use_libraries,basic_enable, \
	oleaut32 \
))
endif

$(eval $(call gb_CppunitTest_set_include,basic_enable,\
-I$(SRCDIR)/basic/source/inc \
-I$(SRCDIR)/basic/inc \
$$(INCLUDE) \
))

$(eval $(call gb_CppunitTest_use_api,basic_enable,\
    offapi \
    udkapi \
))

$(eval $(call gb_CppunitTest_use_ure,basic_enable))

$(eval $(call gb_CppunitTest_use_components,basic_enable,\
    configmgr/source/configmgr \
    ucb/source/core/ucb1 \
))
$(eval $(call gb_CppunitTest_use_configuration,basic_enable))
