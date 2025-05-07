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


#include "regkey.hxx"

#include    <registry/registry.hxx>
#include    <rtl/alloc.h>
#include    "regimpl.hxx"
#include    "keyimpl.hxx"

using rtl::OUString;

//*********************************************************************
//  acquireKey
//
void REGISTRY_CALLTYPE acquireKey(RegKeyHandle hKey)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (pKey != 0)
    {
        ORegistry* pReg = pKey->getRegistry();
        (void) pReg->acquireKey(pKey);
    }
}


//*********************************************************************
//  releaseKey
//
void REGISTRY_CALLTYPE releaseKey(RegKeyHandle hKey)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (pKey != 0)
    {
        ORegistry* pReg = pKey->getRegistry();
        (void) pReg->releaseKey(pKey);
    }
}


//*********************************************************************
//  isKeyReadOnly
//
sal_Bool REGISTRY_CALLTYPE isKeyReadOnly(RegKeyHandle hKey)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    return (pKey != 0) ? pKey->isReadOnly() : sal_False;
}


//*********************************************************************
//  getKeyName
//
RegError REGISTRY_CALLTYPE getKeyName(RegKeyHandle hKey, rtl_uString** pKeyName)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (pKey)
    {
        rtl_uString_assign( pKeyName, pKey->getName().pData );
        return REG_NO_ERROR;
    } else
    {
        rtl_uString_new(pKeyName);
        return REG_INVALID_KEY;
    }
}


//*********************************************************************
//  createKey
//
RegError REGISTRY_CALLTYPE createKey(RegKeyHandle hKey,
                                     rtl_uString* keyName,
                                     RegKeyHandle* phNewKey)
{
    *phNewKey = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    if (pKey->isReadOnly())
        return REG_REGISTRY_READONLY;

    return pKey->createKey(keyName, phNewKey);
}

//*********************************************************************
//  openKey
//
RegError REGISTRY_CALLTYPE openKey(RegKeyHandle hKey,
                                   rtl_uString* keyName,
                                   RegKeyHandle* phOpenKey)
{
    *phOpenKey = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    return pKey->openKey(keyName, phOpenKey);
}

//*********************************************************************
//  openSubKeys
//
RegError REGISTRY_CALLTYPE openSubKeys(RegKeyHandle hKey,
                                       rtl_uString* keyName,
                                       RegKeyHandle** pphSubKeys,
                                       sal_uInt32* pnSubKeys)
{
    *pphSubKeys = NULL;
    *pnSubKeys = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    return pKey->openSubKeys(keyName, pphSubKeys, pnSubKeys);
}

//*********************************************************************
//  closeSubKeys
//
RegError REGISTRY_CALLTYPE closeSubKeys(RegKeyHandle* phSubKeys,
                                        sal_uInt32 nSubKeys)
{
    if (phSubKeys == 0 || nSubKeys == 0)
        return REG_INVALID_KEY;

    ORegistry* pReg = ((ORegKey*)(phSubKeys[0]))->getRegistry();
    for (sal_uInt32 i = 0; i < nSubKeys; i++)
    {
        (void) pReg->closeKey(phSubKeys[i]);
    }
    rtl_freeMemory(phSubKeys);

    return REG_NO_ERROR;
}

//*********************************************************************
//  deleteKey
//
RegError REGISTRY_CALLTYPE deleteKey(RegKeyHandle hKey,
                                     rtl_uString* keyName)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    if (pKey->isReadOnly())
        return REG_REGISTRY_READONLY;

    return pKey->deleteKey(keyName);
}

//*********************************************************************
//  closeKey
//
RegError REGISTRY_CALLTYPE closeKey(RegKeyHandle hKey)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    return pKey->closeKey(hKey);
}

//*********************************************************************
//  setValue
//
RegError REGISTRY_CALLTYPE setValue(RegKeyHandle hKey,
                                       rtl_uString* keyName,
                                       RegValueType valueType,
                                       RegValue pData,
                                       sal_uInt32 valueSize)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    if (pKey->isReadOnly())
        return REG_REGISTRY_READONLY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->setValue(valueName, valueType, pData, valueSize);
        if (_ret1 != REG_NO_ERROR)
        {
            RegError _ret2 = pKey->closeKey(pSubKey);
            if (_ret2)
                return _ret2;
            else
                return _ret1;
        }

        return pKey->closeKey(pSubKey);
    }

    return pKey->setValue(valueName, valueType, pData, valueSize);
}

//*********************************************************************
//  setLongValueList
//
RegError REGISTRY_CALLTYPE setLongListValue(RegKeyHandle hKey,
                                                  rtl_uString* keyName,
                                                  sal_Int32* pValueList,
                                                  sal_uInt32 len)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    if (pKey->isReadOnly())
        return REG_REGISTRY_READONLY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->setLongListValue(valueName, pValueList, len);
        if (_ret1 != REG_NO_ERROR)
        {
            RegError _ret2 = pKey->closeKey(pSubKey);
            if (_ret2 != REG_NO_ERROR)
                return _ret2;
            else
                return _ret1;
        }

        return pKey->closeKey(pSubKey);
    }

    return pKey->setLongListValue(valueName, pValueList, len);
}

//*********************************************************************
//  setStringValueList
//
RegError REGISTRY_CALLTYPE setStringListValue(RegKeyHandle hKey,
                                                   rtl_uString* keyName,
                                                   sal_Char** pValueList,
                                                   sal_uInt32 len)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    if (pKey->isReadOnly())
        return REG_REGISTRY_READONLY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->setStringListValue(valueName, pValueList, len);
        if (_ret1 != REG_NO_ERROR)
        {
            RegError _ret2 = pKey->closeKey(pSubKey);
            if (_ret2 != REG_NO_ERROR)
                return _ret2;
            else
                return _ret1;
        }

        return pKey->closeKey(pSubKey);
    }

    return pKey->setStringListValue(valueName, pValueList, len);
}

//*********************************************************************
//  setUnicodeValueList
//
RegError REGISTRY_CALLTYPE setUnicodeListValue(RegKeyHandle hKey,
                                                     rtl_uString* keyName,
                                                     sal_Unicode** pValueList,
                                                     sal_uInt32 len)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    if (pKey->isReadOnly())
        return REG_REGISTRY_READONLY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->setUnicodeListValue(valueName, pValueList, len);
        if (_ret1 != REG_NO_ERROR)
        {
            RegError _ret2 = pKey->closeKey(pSubKey);
            if (_ret2 != REG_NO_ERROR)
                return _ret2;
            else
                return _ret1;
        }

        return pKey->closeKey(pSubKey);
    }

    return pKey->setUnicodeListValue(valueName, pValueList, len);
}

//*********************************************************************
//  getValueInfo
//
RegError REGISTRY_CALLTYPE getValueInfo(RegKeyHandle hKey,
                                        rtl_uString* keyName,
                                        RegValueType* pValueType,
                                        sal_uInt32* pValueSize)
{
    *pValueType = RG_VALUETYPE_NOT_DEFINED;
    *pValueSize = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    RegValueType valueType;
    sal_uInt32   valueSize;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret != REG_NO_ERROR)
            return _ret;

        if (pSubKey->getValueInfo(valueName, &valueType, &valueSize) != REG_NO_ERROR)
        {
            (void) pKey->releaseKey(pSubKey);
            return REG_INVALID_VALUE;
        }

        *pValueType = valueType;
        *pValueSize = valueSize;

        return pKey->releaseKey(pSubKey);
    }


    if (pKey->getValueInfo(valueName, &valueType, &valueSize) != REG_NO_ERROR)
    {
        return REG_INVALID_VALUE;
    }

    *pValueType = valueType;
    *pValueSize = valueSize;

    return REG_NO_ERROR;
}

//*********************************************************************
//  getValueInfo
//
RegError REGISTRY_CALLTYPE getValue(RegKeyHandle hKey,
                                    rtl_uString* keyName,
                                    RegValue pValue)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->getValue(valueName, pValue);
        if (_ret1 != REG_NO_ERROR)
        {
            (void) pKey->releaseKey(pSubKey);
            return _ret1;
        }

        return pKey->releaseKey(pSubKey);
    }

    return pKey->getValue(valueName, pValue);
}

//*********************************************************************
//  getLongValueList
//
RegError REGISTRY_CALLTYPE getLongListValue(RegKeyHandle hKey,
                                            rtl_uString* keyName,
                                            sal_Int32** pValueList,
                                            sal_uInt32* pLen)
{
    OSL_PRECOND((pValueList != 0) && (pLen != 0), "registry::getLongListValue(): invalid parameter");
    *pValueList = 0, *pLen = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->getLongListValue(valueName, pValueList, pLen);
        if (_ret1 != REG_NO_ERROR)
        {
            (void) pKey->releaseKey(pSubKey);
            return _ret1;
        }

        return pKey->releaseKey(pSubKey);
    }

    return pKey->getLongListValue(valueName, pValueList, pLen);
}

//*********************************************************************
//  getStringValueList
//
RegError REGISTRY_CALLTYPE getStringListValue(RegKeyHandle hKey,
                                              rtl_uString* keyName,
                                              sal_Char*** pValueList,
                                              sal_uInt32* pLen)
{
    OSL_PRECOND((pValueList != 0) && (pLen != 0), "registry::getStringListValue(): invalid parameter");
    *pValueList = 0, *pLen = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->getStringListValue(valueName, pValueList, pLen);
        if (_ret1 != REG_NO_ERROR)
        {
            (void) pKey->releaseKey(pSubKey);
            return _ret1;
        }

        return pKey->releaseKey(pSubKey);
    }

    return pKey->getStringListValue(valueName, pValueList, pLen);
}

//*********************************************************************
//  getUnicodeListValue
//
RegError REGISTRY_CALLTYPE getUnicodeListValue(RegKeyHandle hKey,
                                               rtl_uString* keyName,
                                               sal_Unicode*** pValueList,
                                               sal_uInt32* pLen)
{
    OSL_PRECOND((pValueList != 0) && (pLen != 0), "registry::getUnicodeListValue(): invalid parameter");
    *pValueList = 0, *pLen = 0;

    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    OUString valueName( RTL_CONSTASCII_USTRINGPARAM("value") );
    if (keyName->length)
    {
        ORegKey* pSubKey = 0;
        RegError _ret1 = pKey->openKey(keyName, (RegKeyHandle*)&pSubKey);
        if (_ret1 != REG_NO_ERROR)
            return _ret1;

        _ret1 = pSubKey->getUnicodeListValue(valueName, pValueList, pLen);
        if (_ret1 != REG_NO_ERROR)
        {
            (void) pKey->releaseKey(pSubKey);
            return _ret1;
        }

        return pKey->releaseKey(pSubKey);
    }

    return pKey->getUnicodeListValue(valueName, pValueList, pLen);
}

//*********************************************************************
//  freeValueList
//
RegError REGISTRY_CALLTYPE freeValueList(RegValueType valueType,
                                              RegValue pValueList,
                                              sal_uInt32 len)
{
    switch (valueType)
    {
        case 5:
            {
                rtl_freeMemory(pValueList);
            }
            break;
        case 6:
            {
                sal_Char** pVList = (sal_Char**)pValueList;
                for (sal_uInt32 i=0; i < len; i++)
                {
                    rtl_freeMemory(pVList[i]);
                }

                rtl_freeMemory(pVList);
            }
            break;
        case 7:
            {
                sal_Unicode** pVList = (sal_Unicode**)pValueList;
                for (sal_uInt32 i=0; i < len; i++)
                {
                    rtl_freeMemory(pVList[i]);
                }

                rtl_freeMemory(pVList);
            }
            break;
        default:
            return REG_INVALID_VALUE;
    }

    pValueList = NULL;
    return REG_NO_ERROR;
}

//*********************************************************************
//  createLink
//
RegError REGISTRY_CALLTYPE createLink(
    SAL_UNUSED_PARAMETER RegKeyHandle, SAL_UNUSED_PARAMETER rtl_uString*,
    SAL_UNUSED_PARAMETER rtl_uString*)
{
    return REG_INVALID_LINK; // links are no longer supported
}

//*********************************************************************
//  deleteLink
//
RegError REGISTRY_CALLTYPE deleteLink(
    SAL_UNUSED_PARAMETER RegKeyHandle, SAL_UNUSED_PARAMETER rtl_uString*)
{
    return REG_INVALID_LINK; // links are no longer supported
}

//*********************************************************************
//  getKeyType
//
RegError REGISTRY_CALLTYPE getKeyType(RegKeyHandle hKey,
                                      rtl_uString* keyName,
                                         RegKeyType* pKeyType)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    return pKey->getKeyType(keyName, pKeyType);
}

//*********************************************************************
//  getLinkTarget
//
RegError REGISTRY_CALLTYPE getLinkTarget(
    SAL_UNUSED_PARAMETER RegKeyHandle, SAL_UNUSED_PARAMETER rtl_uString*,
    SAL_UNUSED_PARAMETER rtl_uString**)
{
    return REG_INVALID_LINK; // links are no longer supported
}

//*********************************************************************
//  getName
//
RegError REGISTRY_CALLTYPE getResolvedKeyName(RegKeyHandle hKey,
                                              rtl_uString* keyName,
                                              SAL_UNUSED_PARAMETER sal_Bool,
                                                rtl_uString** pResolvedName)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    OUString resolvedName;
    RegError _ret = pKey->getResolvedKeyName(keyName, resolvedName);
    if (_ret == REG_NO_ERROR)
        rtl_uString_assign(pResolvedName, resolvedName.pData);
    return _ret;
}

//*********************************************************************
//  getKeyNames
//
RegError REGISTRY_CALLTYPE getKeyNames(RegKeyHandle hKey,
                                       rtl_uString* keyName,
                                         rtl_uString*** pSubKeyNames,
                                         sal_uInt32* pnSubKeys)
{
    ORegKey* pKey = static_cast< ORegKey* >(hKey);
    if (!pKey)
        return REG_INVALID_KEY;

    if (pKey->isDeleted())
        return REG_INVALID_KEY;

    return pKey->getKeyNames(keyName, pSubKeyNames, pnSubKeys);
}

//*********************************************************************
//  freeKeyNames
//
RegError REGISTRY_CALLTYPE freeKeyNames(rtl_uString** pKeyNames,
                                          sal_uInt32 nKeys)
{
    for (sal_uInt32 i=0; i < nKeys; i++)
    {
        rtl_uString_release(pKeyNames[i]);
    }

    rtl_freeMemory(pKeyNames);

    return REG_NO_ERROR;
}

//*********************************************************************
//  C API
//

//*********************************************************************
//  reg_createKey
//
RegError REGISTRY_CALLTYPE reg_createKey(RegKeyHandle hKey,
                                         rtl_uString* keyName,
                                         RegKeyHandle* phNewKey)
{
    if (!hKey)
         return REG_INVALID_KEY;

    return createKey(hKey, keyName, phNewKey);
}

//*********************************************************************
//  reg_openKey
//
RegError REGISTRY_CALLTYPE reg_openKey(RegKeyHandle hKey,
                                       rtl_uString* keyName,
                                       RegKeyHandle* phOpenKey)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return openKey(hKey, keyName, phOpenKey);
}

//*********************************************************************
//  reg_openSubKeys
//
RegError REGISTRY_CALLTYPE reg_openSubKeys(RegKeyHandle hKey,
                                           rtl_uString* keyName,
                                           RegKeyHandle** pphSubKeys,
                                           sal_uInt32* pnSubKeys)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return openSubKeys(hKey, keyName, pphSubKeys, pnSubKeys);
}

//*********************************************************************
//  reg_closeSubKeys
//
RegError REGISTRY_CALLTYPE reg_closeSubKeys(RegKeyHandle* pphSubKeys,
                                            sal_uInt32 nSubKeys)
{
    if (!pphSubKeys)
        return REG_INVALID_KEY;

    return closeSubKeys(pphSubKeys, nSubKeys);
}

//*********************************************************************
//  reg_deleteKey
//
RegError REGISTRY_CALLTYPE reg_deleteKey(RegKeyHandle hKey,
                                         rtl_uString* keyName)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return deleteKey(hKey, keyName);
}

//*********************************************************************
//  reg_closeKey
//
RegError REGISTRY_CALLTYPE reg_closeKey(RegKeyHandle hKey)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return closeKey(hKey);
}


//*********************************************************************
//  reg_getKeyName
//
RegError REGISTRY_CALLTYPE reg_getKeyName(RegKeyHandle hKey, rtl_uString** pKeyName)
{
    if (hKey)
    {
        rtl_uString_assign( pKeyName, ((ORegKey*)hKey)->getName().pData );
        return REG_NO_ERROR;
    } else
    {
        rtl_uString_new( pKeyName );
        return REG_INVALID_KEY;
    }
}

//*********************************************************************
//  reg_setValue
//
RegError REGISTRY_CALLTYPE reg_setValue(RegKeyHandle hKey,
                                        rtl_uString* keyName,
                                        RegValueType valueType,
                                        RegValue pData,
                                        sal_uInt32 valueSize)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return setValue(hKey, keyName, valueType, pData, valueSize);
}

//*********************************************************************
//  reg_setLongListValue
//
RegError REGISTRY_CALLTYPE reg_setLongListValue(RegKeyHandle hKey,
                                                      rtl_uString* keyName,
                                                      sal_Int32* pValueList,
                                                      sal_uInt32 len)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return setLongListValue(hKey, keyName, pValueList, len);
}

//*********************************************************************
//  reg_setStringListValue
//
RegError REGISTRY_CALLTYPE reg_setStringListValue(RegKeyHandle hKey,
                                                          rtl_uString* keyName,
                                                          sal_Char** pValueList,
                                                          sal_uInt32 len)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return setStringListValue(hKey, keyName, pValueList, len);
}

//*********************************************************************
//  reg_setUnicodeListValue
//
RegError REGISTRY_CALLTYPE reg_setUnicodeListValue(RegKeyHandle hKey,
                                                            rtl_uString* keyName,
                                                            sal_Unicode** pValueList,
                                                            sal_uInt32 len)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return setUnicodeListValue(hKey, keyName, pValueList, len);
}

//*********************************************************************
//  reg_getValueInfo
//
RegError REGISTRY_CALLTYPE reg_getValueInfo(RegKeyHandle hKey,
                                            rtl_uString* keyName,
                                            RegValueType* pValueType,
                                            sal_uInt32* pValueSize)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getValueInfo(hKey, keyName, pValueType, pValueSize);
}

//*********************************************************************
//  reg_getValueInfo
//
RegError REGISTRY_CALLTYPE reg_getValue(RegKeyHandle hKey,
                                        rtl_uString* keyName,
                                        RegValue pData)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getValue(hKey, keyName, pData);
}

//*********************************************************************
//  reg_getLongListValue
//
RegError REGISTRY_CALLTYPE reg_getLongListValue(RegKeyHandle hKey,
                                                      rtl_uString* keyName,
                                                      sal_Int32** pValueList,
                                                      sal_uInt32* pLen)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getLongListValue(hKey, keyName, pValueList, pLen);
}

//*********************************************************************
//  reg_getStringListValue
//
RegError REGISTRY_CALLTYPE reg_getStringListValue(RegKeyHandle hKey,
                                                       rtl_uString* keyName,
                                                       sal_Char*** pValueList,
                                                       sal_uInt32* pLen)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getStringListValue(hKey, keyName, pValueList, pLen);
}

//*********************************************************************
//  reg_getUnicodeListValue
//
RegError REGISTRY_CALLTYPE reg_getUnicodeListValue(RegKeyHandle hKey,
                                                         rtl_uString* keyName,
                                                         sal_Unicode*** pValueList,
                                                         sal_uInt32* pLen)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getUnicodeListValue(hKey, keyName, pValueList, pLen);
}

//*********************************************************************
//  reg_freeValueList
//
RegError REGISTRY_CALLTYPE reg_freeValueList(RegValueType valueType,
                                                  RegValue pValueList,
                                                  sal_uInt32 len)
{
    if (pValueList)
        return freeValueList(valueType, pValueList, len);
    else
        return REG_INVALID_VALUE;
}

//*********************************************************************
//  reg_getKeyType
//
RegError REGISTRY_CALLTYPE reg_getKeyType(RegKeyHandle hKey,
                                          rtl_uString* keyName,
                                             RegKeyType* pKeyType)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getKeyType(hKey, keyName, pKeyType);
}

//*********************************************************************
//  reg_getResolvedKeyName
//
RegError REGISTRY_CALLTYPE reg_getResolvedKeyName(RegKeyHandle hKey,
                                                    rtl_uString* keyName,
                                                    sal_Bool firstLinkOnly,
                                                      rtl_uString** pResolvedName)
{
    if (!hKey)
        return REG_INVALID_KEY;

    return getResolvedKeyName(hKey, keyName, firstLinkOnly, pResolvedName);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
