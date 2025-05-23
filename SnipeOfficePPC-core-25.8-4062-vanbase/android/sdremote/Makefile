# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../../config_host.mk

all: properties translations.done
	mkdir -p ../abs-lib/libs
	cp $(ANDROID_SDK_HOME)/extras/android/support/v4/android-support-v4.jar ../abs-lib/libs
	$(ANT) debug

properties:
	echo sdk.dir=$(ANDROID_SDK_HOME) >local.properties
	echo sdk.dir=$(ANDROID_SDK_HOME) >../abs-lib/local.properties

clean: properties translations.clean
	$(ANT) clean

install: all
	$(ANDROID_SDK_HOME)/platform-tools/adb install -r ./bin/ImpressRemote-debug.apk

define android_get_podir
res/values-$(1)
endef

define android_get_po
$(call android_get_podir,$(1))/strings.xml
endef

define android_translation_rules
$(call android_get_po,$(1)):
	mkdir -p res/values-$(1)
	echo $(SRCDIR)/translations/source/$(1)/android/sdremote/res/values.po > translations-$(1).db
	$(OUTDIR_FOR_BUILD)/bin/stringex -i res/values/strings.xml -o res/values-$(1)/strings.xml -m translations-$(1).db -l $(1)

endef

define android_translations_rules
$(foreach lang,$(1),$(call android_translation_rules,$(lang)))
endef

define android_get_langlist
$(foreach lang,$(filter-out en-US sat sid mai ast mni brx dgo qtz nso kok,$(WITH_LANG_LIST)),$(subst -,-r,$(lang)))
endef

$(eval $(call android_translations_rules,$(call android_get_langlist)))

translations.done: $(foreach lang,$(call android_get_langlist),$(call android_get_po,$(lang)))
	touch translations.done

translations.clean:
	rm -rf $(foreach lang,$(call android_get_langlist),$(call android_get_podir,$(lang)))
	rm -rf $(foreach lang,$(call android_get_langlist),translations-$(lang).db)
	rm translations.done

# vim: set noet sw=4 ts=4:
