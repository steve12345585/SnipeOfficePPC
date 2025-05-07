#***************************************************************
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#***************************************************************

$(eval $(call gb_Library_Library,msfilter))

$(eval $(call gb_Library_use_package,msfilter,filter_inc))

$(eval $(call gb_Library_set_componentfile,msfilter,filter/source/msfilter/msfilter))

$(eval $(call gb_Library_use_api,msfilter,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_set_include,msfilter,\
	$$(INCLUDE) \
))

$(eval $(call gb_Library_add_defs,msfilter,\
	-DMSFILTER_DLLIMPLEMENTATION \
))

$(eval $(call gb_Library_use_libraries,msfilter,\
	svxcore \
	editeng \
	sfx \
	xo \
	sb \
	svt \
	tk \
	vcl \
	svl \
	sot \
	xcr \
	utl \
	tl \
	comphelper \
	basegfx \
	cppuhelper \
	cppu \
	sal \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,msfilter,\
	filter/source/msfilter/countryid \
	filter/source/msfilter/escherex \
	filter/source/msfilter/eschesdo \
	filter/source/msfilter/mscodec \
	filter/source/msfilter/msdffimp \
	filter/source/msfilter/msocximex \
	filter/source/msfilter/msoleexp \
	filter/source/msfilter/mstoolbar \
	filter/source/msfilter/msvbahelper \
	filter/source/msfilter/services \
	filter/source/msfilter/svdfppt \
	filter/source/msfilter/svxmsbas2 \
	filter/source/msfilter/util \
))

# vim: set noet sw=4 ts=4:
