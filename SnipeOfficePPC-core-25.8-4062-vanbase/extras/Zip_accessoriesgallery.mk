# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Zip_Zip,accessoriesgallery,$(call gb_UnpackedTarball_get_dir,gallery-pack)/accessories))

$(eval $(call gb_Zip_add_dependencies,accessoriesgallery,\
	$(call gb_UnpackedTarball_get_target,gallery-pack) \
))

$(eval $(call gb_Zip_add_files,accessoriesgallery,\
	sg1010.sdg \
	sg1010.sdv \
	sg1010.thm \
	sg1011.sdg \
	sg1011.sdv \
	sg1011.thm \
	sg101.sdg \
	sg101.sdv \
	sg101.thm \
	sg102.sdg \
	sg102.sdv \
	sg102.thm \
	sg103.sdg \
	sg103.sdv \
	sg103.thm \
	sg105.sdg \
	sg105.sdv \
	sg105.thm \
	sg1067.sdg \
	sg1067.sdv \
	sg1067.thm \
	sg106.sdg \
	sg106.sdv \
	sg106.thm \
	sg107.sdg \
	sg107.sdv \
	sg107.thm \
	sg108.sdg \
	sg108.sdv \
	sg108.thm \
	sg109.sdg \
	sg109.sdv \
	sg109.thm \
	sg110.sdg \
	sg110.sdv \
	sg110.thm \
	sg111.sdg \
	sg111.sdv \
	sg111.thm \
	sg1208.sdg \
	sg1208.sdv \
	sg1208.thm \
	sg121.sdg \
	sg121.sdv \
	sg121.thm \
	sg127.sdg \
	sg127.sdv \
	sg127.thm \
	sg133.sdg \
	sg133.sdv \
	sg133.thm \
	sg200.sdg \
	sg200.sdv \
	sg200.thm \
	sg201.sdg \
	sg201.sdv \
	sg201.thm \
	sg203.sdg \
	sg203.sdv \
	sg203.thm \
	sg204.sdg \
	sg204.sdv \
	sg204.thm \
	sg205.sdg \
	sg205.sdv \
	sg205.thm \
	sg206.sdg \
	sg206.sdv \
	sg206.thm \
	sg207.sdg \
	sg207.sdv \
	sg207.thm \
	sg2101.sdg \
	sg2101.sdv \
	sg2101.thm \
	sg210.sdg \
	sg210.sdv \
	sg210.thm \
	sg212.sdg \
	sg212.sdv \
	sg212.thm \
	sg2245.sdg \
	sg2245.sdv \
	sg2245.thm \
	sg2246.sdg \
	sg2246.sdv \
	sg2246.thm \
	sg2247.sdg \
	sg2247.sdv \
	sg2247.thm \
	sg2248.sdg \
	sg2248.sdv \
	sg2248.thm \
	sg2249.sdg \
	sg2249.sdv \
	sg2249.thm \
	sg2250.sdg \
	sg2250.sdv \
	sg2250.thm \
	sg2251.sdg \
	sg2251.sdv \
	sg2251.thm \
	sg2253.sdg \
	sg2253.sdv \
	sg2253.thm \
	sg2254.sdg \
	sg2254.sdv \
	sg2254.thm \
	sg2255.sdg \
	sg2255.sdv \
	sg2255.thm \
	sg2256.sdg \
	sg2256.sdv \
	sg2256.thm \
	sg2257.sdg \
	sg2257.sdv \
	sg2257.thm \
	sg2258.sdg \
	sg2258.sdv \
	sg2258.thm \
	sg2259.sdg \
	sg2259.sdv \
	sg2259.thm \
	sg2261.sdg \
	sg2261.sdv \
	sg2261.thm \
	sg2262.sdg \
	sg2262.sdv \
	sg2262.thm \
	sg2264.sdg \
	sg2264.sdv \
	sg2264.thm \
	sg2265.sdg \
	sg2265.sdv \
	sg2265.thm \
	sg2266.sdg \
	sg2266.sdv \
	sg2266.thm \
	sg2267.sdg \
	sg2267.sdv \
	sg2267.thm \
	sg2268.sdg \
	sg2268.sdv \
	sg2268.thm \
	sg2269.sdg \
	sg2269.sdv \
	sg2269.thm \
	sg2270.sdg \
	sg2270.sdv \
	sg2270.thm \
	sg2272.sdg \
	sg2272.sdv \
	sg2272.thm \
	sg2273.sdg \
	sg2273.sdv \
	sg2273.thm \
	sg2274.sdg \
	sg2274.sdv \
	sg2274.thm \
	sg2275.sdg \
	sg2275.sdv \
	sg2275.thm \
	sg2276.sdg \
	sg2276.sdv \
	sg2276.thm \
	sg2277.sdg \
	sg2277.sdv \
	sg2277.thm \
	sg2278.sdg \
	sg2278.sdv \
	sg2278.thm \
	sg2279.sdg \
	sg2279.sdv \
	sg2279.thm \
	sg2280.sdg \
	sg2280.sdv \
	sg2280.thm \
	sg2281.sdg \
	sg2281.sdv \
	sg2281.thm \
	sg2282.sdg \
	sg2282.sdv \
	sg2282.thm \
	sg2283.sdg \
	sg2283.sdv \
	sg2283.thm \
	sg2284.sdg \
	sg2284.sdv \
	sg2284.thm \
	sg2285.sdg \
	sg2285.sdv \
	sg2285.thm \
	sg2303.sdg \
	sg2303.sdv \
	sg2303.thm \
	sg2304.sdg \
	sg2304.sdv \
	sg2304.thm \
	sg2305.sdg \
	sg2305.sdv \
	sg2305.thm \
	sg2306.sdg \
	sg2306.sdv \
	sg2306.thm \
	sg2307.sdg \
	sg2307.sdv \
	sg2307.thm \
	sg2308.sdg \
	sg2308.sdv \
	sg2308.thm \
	sg2309.sdg \
	sg2309.sdv \
	sg2309.thm \
	sg2310.sdg \
	sg2310.sdv \
	sg2310.thm \
	sg2311.sdg \
	sg2311.sdv \
	sg2311.thm \
	sg2312.sdg \
	sg2312.sdv \
	sg2312.thm \
	sg231.sdg \
	sg231.sdv \
	sg231.thm \
	sg301.sdg \
	sg301.sdv \
	sg301.thm \
	sg302.sdg \
	sg302.sdv \
	sg302.thm \
	sg303.sdg \
	sg303.sdv \
	sg303.thm \
	sg304.sdg \
	sg304.sdv \
	sg304.thm \
	sg305.sdg \
	sg305.sdv \
	sg305.thm \
	sg307.sdg \
	sg307.sdv \
	sg307.thm \
	sg308.sdg \
	sg308.sdv \
	sg308.thm \
	sg309.sdg \
	sg309.sdv \
	sg309.thm \
	sg310.sdg \
	sg310.sdv \
	sg310.thm \
	sg311.sdg \
	sg311.sdv \
	sg311.thm \
	sg312.sdg \
	sg312.sdv \
	sg312.thm \
	sg313.sdg \
	sg313.sdv \
	sg313.thm \
	sg314.sdg \
	sg314.sdv \
	sg314.thm \
	sg315.sdg \
	sg315.sdv \
	sg315.thm \
	sg316.sdg \
	sg316.sdv \
	sg316.thm \
	sg317.sdg \
	sg317.sdv \
	sg317.thm \
	sg318.sdg \
	sg318.sdv \
	sg318.thm \
	sg319.sdg \
	sg319.sdv \
	sg319.thm \
	sg320.sdg \
	sg320.sdv \
	sg320.thm \
	sg402.sdg \
	sg402.sdv \
	sg402.thm \
	sg403.sdg \
	sg403.sdv \
	sg403.thm \
	sg404.sdg \
	sg404.sdv \
	sg404.thm \
	sg405.sdg \
	sg405.sdv \
	sg405.thm \
	sg412.sdg \
	sg412.sdv \
	sg412.thm \
	sg555.sdg \
	sg555.sdv \
	sg555.thm \
))

# vim: set noet sw=4 ts=4:
