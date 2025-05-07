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
#ifndef _RSCDEF_HXX
#define _RSCDEF_HXX

#include <tools/unqidx.hxx>
#include <rsctree.hxx>
#include <rtl/strbuf.hxx>
#include <vector>

/****************** C L A S S E S ****************************************/
class RscExpression;
class RscFileTab;
class RscDefine;

/*********** R s c E x p r e s s i o n ***********************************/
#define RSCEXP_LONG     0
#define RSCEXP_EXP      1
#define RSCEXP_DEF      2
#define RSCEXP_NOTHING  3

class RscExpType
{
public:
    union {
        RscExpression * pExp;
        RscDefine     * pDef;
        struct {
             short           nHi;
             unsigned short  nLo;
        } aLong;
    } aExp;
    char cType;
    char cUnused;
    sal_Bool IsNumber()    const { return( RSCEXP_LONG     == cType ); }
    sal_Bool IsExpression()const { return( RSCEXP_EXP      == cType ); }
    sal_Bool IsDefinition()const { return( RSCEXP_DEF      == cType ); }
    sal_Bool IsNothing()   const { return( RSCEXP_NOTHING  == cType ); }
    void SetLong( sal_Int32 lValue ){
            aExp.aLong.nHi = (short)(lValue >> 16);
            aExp.aLong.nLo = (unsigned short)lValue;
            cType = RSCEXP_LONG;
         }
    sal_Int32 GetLong() const{
             return aExp.aLong.nLo |
                    ((sal_Int32)aExp.aLong.nHi << 16);
         }
    sal_Bool Evaluate( sal_Int32 * pValue ) const;
    void AppendMacro( rtl::OStringBuffer & ) const;
};

/*********** R s c I d ***************************************************/
class RscId
{
    static      sal_Bool bNames;// sal_False, bei den Namenoperation nur Zahlen
public:
    RscExpType  aExp;       // Zahl, Define oder Ausdruck
    sal_Int32   GetNumber() const;
    void    Create( const RscExpType & rExpType );
    void    Create(){ aExp.cType = RSCEXP_NOTHING; }

            RscId() { Create();                         }

            RscId( RscDefine * pEle );
            RscId( sal_Int32 lNumber )
                    { aExp.SetLong( lNumber );          }

            RscId( const RscExpType & rExpType )
                    { Create( rExpType );               }

            void Destroy();

            ~RscId(){
                Destroy();
            }

            RscId( const RscId& rRscId );

            RscId& operator = ( const RscId& rRscId );

    static void    SetNames( sal_Bool bSet = sal_True );
    operator         sal_Int32() const;   // Gibt Nummer zurueck
    rtl::OString GetName()  const;   // Gibt den Namen des Defines zurueck
    sal_Bool    operator <  ( const RscId& rRscId ) const;
    sal_Bool    operator >  ( const RscId& rRscId ) const;
    sal_Bool    operator == ( const RscId& rRscId ) const;
    sal_Bool    operator <=  ( const RscId& rRscId ) const
            { return !(operator > ( rRscId )); }
    sal_Bool    operator >= ( const RscId& rRscId ) const
            { return !(operator < ( rRscId )); }
    sal_Bool    IsId() const { return !aExp.IsNothing(); }
};

/*********** R s c D e f i n e *******************************************/
class RscDefine : public StringNode
{
friend class RscFileTab;
friend class RscDefineList;
friend class RscDefTree;
friend class RscExpression;
friend class RscId;
    sal_uLong           lFileKey;   // zu welcher Datei gehoert das Define
    sal_uInt32          nRefCount;  // Wieviele Referenzen auf dieses Objekt
    sal_Int32           lId;        // Identifier
    RscExpression * pExp;       // Ausdruck
protected:

            RscDefine( sal_uLong lFileKey, const rtl::OString& rDefName,
                       sal_Int32 lDefId );
            RscDefine( sal_uLong lFileKey, const rtl::OString& rDefName,
                       RscExpression * pExpression );
            ~RscDefine();
    void    IncRef(){ nRefCount++; }
    sal_uInt32  GetRefCount() const    { return nRefCount; }
    void    DecRef();
    void    DefineToNumber();
    void    SetName(const rtl::OString& rNewName) { m_aName = rNewName; }

    using StringNode::Search;
public:
    RscDefine * Search( const char * );
    sal_uLong       GetFileKey() const { return lFileKey; }
    sal_Bool        Evaluate();
    sal_Int32       GetNumber() const  { return lId;      }
    rtl::OString    GetMacro();
};

typedef ::std::vector< RscDefine* > RscSubDefList;

class RscDefineList {
friend class RscFile;
friend class RscFileTab;
private:
    RscSubDefList   maList;
                // pExpression wird auf jedenfall Eigentum der Liste
    RscDefine * New( sal_uLong lFileKey, const rtl::OString& rDefName,
                     sal_Int32 lDefId, size_t lPos );
    RscDefine * New( sal_uLong lFileKey, const rtl::OString& rDefName,
                     RscExpression * pExpression, size_t lPos );
    sal_Bool        Remove();
    size_t      GetPos( RscDefine* item ) {
                    for ( size_t i = 0, n = maList.size(); i < n; ++i )
                        if ( maList[ i ] == item )
                            return i;
                    return size_t(-1);
                }
public:
    void        WriteAll( FILE * fOutput );
};

/*********** R s c E x p r e s s i o n ***********************************/
class RscExpression {
friend class RscFileTab;
    char        cOperation;
    RscExpType  aLeftExp;
    RscExpType  aRightExp;
public:
                RscExpression( RscExpType aLE, char cOp,
                               RscExpType aRE );
                ~RscExpression();
    sal_Bool        Evaluate( sal_Int32 * pValue );
    rtl::OString GetMacro();
};

/********************** R S C F I L E ************************************/
class RscDepend {
    sal_uLong            lKey;
public:
            RscDepend( sal_uLong lIncKey ){ lKey = lIncKey; };
    sal_uLong   GetFileKey(){ return lKey; }
};

typedef ::std::vector< RscDepend* > RscDependList;

// Tabelle die alle Dateinamen enthaelt
class RscFile
{
friend class RscFileTab;
    sal_Bool            bIncFile;   // Ist es eine Include-Datei
public:
    sal_Bool            bLoaded;    // Ist die Datei geladen
    sal_Bool            bScanned;   // Wurde Datei nach Inclide abgesucht
    sal_Bool            bDirty;     // Dirty-Flag
    rtl::OString aFileName;  // Name der Datei
    rtl::OString aPathName;  // Pfad und Name der Datei
    RscDefineList   aDefLst;    // Liste der Defines
    RscDependList   aDepLst;    // List of Depend

                    RscFile();
                    ~RscFile();
    sal_Bool            InsertDependFile( sal_uLong lDepFile, size_t lPos );
    sal_Bool            Depend( sal_uLong lDepend, sal_uLong lFree );
    void            SetIncFlag(){ bIncFile = sal_True; };
    sal_Bool            IsIncFile(){  return bIncFile; };
};

DECLARE_UNIQUEINDEX( RscSubFileTab, RscFile * )
#define NOFILE_INDEX UNIQUEINDEX_ENTRY_NOTFOUND

class RscDefTree {
    RscDefine * pDefRoot;
public:
    static sal_Bool Evaluate( RscDefine * pDef );
                RscDefTree(){ pDefRoot = NULL; }
                ~RscDefTree();
    void        Remove();
    RscDefine * Search( const char * pName );
    void        Insert( RscDefine * pDef );
    void        Remove( RscDefine * pDef );
};

class RscFileTab : public RscSubFileTab
{
    RscDefTree aDefTree;
    sal_uLong       Find(const rtl::OString& rName);
public:
                RscFileTab();
                ~RscFileTab();

    RscDefine * FindDef( const char * );
    RscDefine * FindDef(const rtl::OString& rStr)
    {
        return FindDef(rStr.getStr());
    }

    sal_Bool        Depend( sal_uLong lDepend, sal_uLong lFree );
    sal_Bool        TestDef( sal_uLong lFileKey, size_t lPos,
                         const RscDefine * pDefDec );
    sal_Bool        TestDef( sal_uLong lFileKey, size_t lPos,
                         const RscExpression * pExpDec );

    RscDefine * NewDef( sal_uLong lKey, const rtl::OString& rDefName,
                        sal_Int32 lId, sal_uLong lPos );
    RscDefine * NewDef( sal_uLong lKey, const rtl::OString& rDefName,
                        RscExpression *, sal_uLong lPos );

           // Alle Defines die in dieser Datei Definiert sind loeschen
    void   DeleteFileContext( sal_uLong lKey );
    sal_uLong  NewCodeFile(const rtl::OString& rName);
    sal_uLong  NewIncFile(const rtl::OString& rName, const rtl::OString& rPath);
    RscFile * GetFile( sal_uLong lFileKey ){ return Get( lFileKey ); }
};

#endif // _RSCDEF_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
