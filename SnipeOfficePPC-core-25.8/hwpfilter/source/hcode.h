/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _HCODE_H_
#define _HCODE_H_

#include "hwplib.h"

/**
 * Codetype of Korean
 * KSSM - Johap, KS - Wansung
 */
enum { KSSM, KS, UNICODE };
/**
 *   Transfer combination-code for internal using of hwp to ascii
 */
DLLEXPORT int hcharconv(hchar ch, hchar *dest, int codeType) ;

DLLEXPORT int   kssm_hangul_to_ucs2(hchar ch, hchar *dest) ;
DLLEXPORT hchar ksc5601_han_to_ucs2 (hchar);
DLLEXPORT hchar ksc5601_sym_to_ucs2 (hchar);
DLLEXPORT hchar_string hstr2ucsstr(hchar const* hstr);
/**
 * ���Ľ�Ʈ���� �ϼ�����Ʈ������ ��ȯ�Ѵ�.
 */
DLLEXPORT ::std::string hstr2ksstr(hchar const* hstr);

/**
 * �ѱ��� ������ �� �ִ� char����Ʈ���� ���Ľ�Ʈ������ ��ȯ�Ѵ�.
 */
DLLEXPORT hchar_string kstr2hstr(uchar const* src);

/**
 * hwp�� ��θ� unix���·� �ٲ۴�.
 */
DLLEXPORT ::std::string urltounix(const char *src);

/**
 * hwp�� ��θ� windows���·� �ٲ۴�.
 */
#ifdef _WIN32
DLLEXPORT ::std::string urltowin(const char *src);
#endif
/**
 *  Transfer interger to string following format
 */
DLLEXPORT char* Int2Str(int value, const char *format, char *buf);

/**
 * color�ε��� ���� �������� �����Ͽ� ��Ÿ���ǽ��� color�� ��ȯ
 */
DLLEXPORT char *hcolor2str(uchar color, uchar shade, char *buf, bool bIsChar = false);

DLLEXPORT char *base64_encode_string( const uchar *buf, unsigned int len );
DLLEXPORT double calcAngle(int x1, int y1, int x2, int y2);


#endif                                            /* _HCODE_H_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
