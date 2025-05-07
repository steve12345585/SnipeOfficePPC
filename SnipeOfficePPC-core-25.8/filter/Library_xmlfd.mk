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

$(eval $(call gb_Library_Library,xmlfd))

$(eval $(call gb_Library_set_componentfile,xmlfd,filter/source/xmlfilterdetect/xmlfd))

$(eval $(call gb_Library_use_api,xmlfd,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_set_include,xmlfd,\
	$$(INCLUDE) \
))

$(eval $(call gb_Library_use_libraries,xmlfd,\
	ucbhelper \
	cppuhelper \
	cppu \
	sal \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,xmlfd,\
	filter/source/xmlfilterdetect/fdcomp \
	filter/source/xmlfilterdetect/filterdetect \
))

# vim: set noet sw=4 ts=4:
