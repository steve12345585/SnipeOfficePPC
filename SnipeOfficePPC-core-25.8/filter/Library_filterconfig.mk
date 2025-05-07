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

$(eval $(call gb_Library_Library,filterconfig))

$(eval $(call gb_Library_set_componentfile,filterconfig,filter/source/config/cache/filterconfig1))

$(eval $(call gb_Library_use_api,filterconfig,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_set_include,filterconfig,\
	$$(INCLUDE) \
	-I$(SRCDIR)/filter/inc \
))

$(eval $(call gb_Library_use_libraries,filterconfig,\
	fwe \
	utl \
	tl \
	comphelper \
	cppuhelper \
	cppu \
	sal \
	salhelper \
	$(gb_STDLIBS) \
))


$(eval $(call gb_Library_add_exception_objects,filterconfig,\
	filter/source/config/cache/basecontainer \
	filter/source/config/cache/cacheitem \
	filter/source/config/cache/cacheupdatelistener \
	filter/source/config/cache/configflush \
	filter/source/config/cache/contenthandlerfactory \
	filter/source/config/cache/filtercache \
	filter/source/config/cache/filterfactory \
	filter/source/config/cache/frameloaderfactory \
	filter/source/config/cache/lateinitlistener \
	filter/source/config/cache/lateinitthread \
	filter/source/config/cache/querytokenizer \
	filter/source/config/cache/registration \
	filter/source/config/cache/typedetection \
))

# vim: set noet sw=4 ts=4:
