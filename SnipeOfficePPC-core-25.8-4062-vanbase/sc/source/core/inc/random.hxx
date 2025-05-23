/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SC_RANDOM_HXX
#define SC_RANDOM_HXX

namespace sc
{

namespace rng
{

void seed(int i);       // set initial seed (equivalent of libc srand())

double uniform();                   // uniform distribution in [0,1)

} // namespace

} // namespace

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
