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

$(eval $(call gb_Library_Library,xsltfilter))

$(eval $(call gb_Library_set_componentfile,xsltfilter,filter/source/xsltfilter/xsltfilter))

$(eval $(call gb_Library_use_api,xsltfilter,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_set_include,xsltfilter,\
	$$(INCLUDE) \
))

$(eval $(call gb_Library_use_libraries,xsltfilter,\
	xo \
	tl \
	package2 \
	sax \
	comphelper \
	ucbhelper \
	cppuhelper \
	cppu \
	sal \
        salhelper \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_use_externals,xsltfilter,\
	libxml2 \
	libxslt \
))

$(eval $(call gb_Library_add_exception_objects,xsltfilter,\
	filter/source/xsltfilter/LibXSLTTransformer \
	filter/source/xsltfilter/OleHandler \
	filter/source/xsltfilter/XSLTFilter \
))

# vim: set noet sw=4 ts=4:
