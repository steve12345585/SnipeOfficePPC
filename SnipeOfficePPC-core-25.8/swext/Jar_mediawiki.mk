#
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Initial Developer of the Original Code is
# 	Peter Foley <pefoley2@verizon.net>
# Portions created by the Initial Developer are Copyright (C) 2011 the
# Initial Developer. All Rights Reserved.
#
# Major Contributor(s):
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.
#

$(eval $(call gb_Jar_Jar,mediawiki))

$(eval $(call gb_Jar_set_packageroot,mediawiki,com))

$(eval $(call gb_Jar_set_manifest,mediawiki,$(SRCDIR)/swext/mediawiki/src/com/sun/star/wiki/MANIFEST.MF))

ifeq ($(SYSTEM_APACHE_COMMONS),YES)
# FIXME: use gb_Jar_use_externals?
# for the build
$(eval $(call gb_Jar_use_jars,mediawiki,\
	$(COMMONS_CODEC_JAR) \
	$(COMMONS_LANG_JAR) \
	$(COMMONS_HTTPCLIENT_JAR) \
	$(COMMONS_LOGGING_JAR) \
))
# Class-Path: in MANIFEST.MF
$(eval $(call gb_Jar_set_jarclasspath,mediawiki,\
	$(COMMONS_CODEC_JAR) \
	$(COMMONS_LANG_JAR) \
	$(COMMONS_HTTPCLIENT_JAR) \
	$(COMMONS_LOGGING_JAR) \
))
else
$(eval $(call gb_Jar_set_jarclasspath,mediawiki,\
	commons-codec-1.3.jar \
	commons-lang-2.3.jar \
	commons-httpclient-3.1.jar \
	commons-logging-1.1.1.jar \
))
endif

$(eval $(call gb_Jar_use_jars,mediawiki,\
	$(OUTDIR)/bin/juh.jar \
	$(OUTDIR)/bin/jurt.jar \
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/unoil.jar \
))

ifneq ($(SYSTEM_APACHE_COMMONS),YES)
$(eval $(call gb_Jar_use_jars,mediawiki,\
	$(OUTDIR)/bin/commons-codec-1.3.jar \
	$(OUTDIR)/bin/commons-lang-2.3.jar \
	$(OUTDIR)/bin/commons-httpclient-3.1.jar \
	$(OUTDIR)/bin/commons-logging-1.1.1.jar \
))
endif

$(eval $(call gb_Jar_add_sourcefiles,mediawiki,\
	swext/mediawiki/src/com/sun/star/wiki/EditPageParser \
	swext/mediawiki/src/com/sun/star/wiki/Helper \
	swext/mediawiki/src/com/sun/star/wiki/MainThreadDialogExecutor \
	swext/mediawiki/src/com/sun/star/wiki/Settings \
	swext/mediawiki/src/com/sun/star/wiki/WikiArticle \
	swext/mediawiki/src/com/sun/star/wiki/WikiCancelException \
	swext/mediawiki/src/com/sun/star/wiki/WikiDialog \
	swext/mediawiki/src/com/sun/star/wiki/WikiEditorImpl \
	swext/mediawiki/src/com/sun/star/wiki/WikiEditSettingDialog \
	swext/mediawiki/src/com/sun/star/wiki/WikiOptionsEventHandlerImpl \
	swext/mediawiki/src/com/sun/star/wiki/WikiPropDialog \
	swext/mediawiki/src/com/sun/star/wiki/WikiProtocolSocketFactory \
))
