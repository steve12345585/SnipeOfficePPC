/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * [ Copyright (C) 2011 Noel Power <noel.power@suse.com> (initial developer) ]
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */
#include "oox/ole/axbinarywriter.hxx"

#include "oox/ole/olehelper.hxx"

namespace oox {
namespace ole {

// ============================================================================

using ::rtl::OUString;

// ============================================================================

namespace {

const sal_uInt32 AX_STRING_SIZEMASK         = 0x7FFFFFFF;
const sal_uInt32 AX_STRING_COMPRESSED       = 0x80000000;

} // namespace

// ============================================================================

AxAlignedOutputStream::AxAlignedOutputStream( BinaryOutputStream& rOutStrm ) :
    BinaryStreamBase( false ),
    mpOutStrm( &rOutStrm ),
    mnStrmPos( 0 ),
    mnStrmSize( rOutStrm.getRemaining() ),
    mnWrappedBeginPos( rOutStrm.tell() )
{
    mbEof = mbEof || rOutStrm.isEof();
}

sal_Int64 AxAlignedOutputStream::size() const
{
    return mpOutStrm ? mnStrmSize : -1;
}

sal_Int64 AxAlignedOutputStream::tell() const
{
    return mpOutStrm ? mnStrmPos : -1;
}

void AxAlignedOutputStream::seek( sal_Int64 nPos )
{
    mbEof = (nPos < 0);
    if( !mbEof )
    {
        mpOutStrm->seek( static_cast< sal_Int32 >( mnWrappedBeginPos + nPos  ) );
        mnStrmPos = mpOutStrm->tell() - mnWrappedBeginPos;
    }
}

void AxAlignedOutputStream::close()
{
    mpOutStrm = 0;
    mbEof = true;
}

void AxAlignedOutputStream::writeData( const StreamDataSequence& orData, size_t nAtomSize )
{
    mpOutStrm->writeData( orData, nAtomSize );
    mnStrmPos = mpOutStrm->tell() - mnWrappedBeginPos;
}

void AxAlignedOutputStream::writeMemory( const void* opMem, sal_Int32 nBytes, size_t nAtomSize )
{
    mpOutStrm->writeMemory( opMem, nBytes, nAtomSize );
    mnStrmPos = mpOutStrm->tell() - mnWrappedBeginPos;
}

void AxAlignedOutputStream::pad( sal_Int32 nBytes, size_t nAtomSize )
{
   //PRESUMABELY we need to pad with 0's here as appropriate
   com::sun::star::uno::Sequence< sal_Int8 > aData( nBytes );
   // ok we could be padding with rubbish here, but really that shouldn't matter
   // set to 0(s), easier to not get fooled by 0's when looking at
   // binary content......
   memset( static_cast<void*>( aData.getArray() ), 0, nBytes );
   mpOutStrm->writeData( aData, nAtomSize );
   mnStrmPos = mpOutStrm->tell() - mnWrappedBeginPos;
}

void AxAlignedOutputStream::align( size_t nSize )
{
    pad( static_cast< sal_Int32 >( (nSize - (mnStrmPos % nSize)) % nSize ) );
}

// ============================================================================

namespace {

void lclWriteString( AxAlignedOutputStream& rOutStrm, OUString& rValue, sal_uInt32 nSize, bool bArrayString )
{
    bool bCompressed = getFlag( nSize, AX_STRING_COMPRESSED );
    rOutStrm.writeCompressedUnicodeArray( rValue, bCompressed || bArrayString );
}

} // namespace

// ----------------------------------------------------------------------------

AxBinaryPropertyWriter::ComplexProperty::~ComplexProperty()
{
}

bool AxBinaryPropertyWriter::PairProperty::writeProperty( AxAlignedOutputStream& rOutStrm )
{
    rOutStrm << mrPairData.first << mrPairData.second;
    return true;
}

bool AxBinaryPropertyWriter::StringProperty::writeProperty( AxAlignedOutputStream& rOutStrm )
{
    lclWriteString( rOutStrm, mrValue, mnSize, false );
    return true;
}

// ----------------------------------------------------------------------------

AxBinaryPropertyWriter::AxBinaryPropertyWriter( BinaryOutputStream& rOutStrm, bool b64BitPropFlags ) :
    maOutStrm( rOutStrm ),
    mnPropFlags( 0x0 ),
    mbValid( true ),
    mb64BitPropFlags( b64BitPropFlags )
{
    sal_uInt16 nId( 0x0200 );
    maOutStrm << nId;
    mnBlockSize = 0; // will be filled in the finalize method

    maOutStrm << nId;
    mnPropFlagsStart = maOutStrm.tell();

    if( mb64BitPropFlags )
        maOutStrm << mnPropFlags;
    else
        maOutStrm << sal_uInt32( mnPropFlags );
    mnNextProp = 1;
}

void AxBinaryPropertyWriter::writeBoolProperty( bool orbValue, bool bReverse )
{
    // orbValue ^ bReverse true then we want to set the bit, e.g. don't skip
    startNextProperty(  !( ( orbValue ^ bReverse ) >= 1 ) );
}

void AxBinaryPropertyWriter::writePairProperty( AxPairData& orPairData )
{
    if( startNextProperty() )
        maLargeProps.push_back( ComplexPropVector::value_type( new PairProperty( orPairData ) ) );
}

void AxBinaryPropertyWriter::writeStringProperty( OUString& orValue, bool bCompressed )
{
    sal_uInt32 nSize = orValue.getLength();
    if ( bCompressed )
        setFlag(  nSize, AX_STRING_COMPRESSED );
    else
        nSize *= 2;
    maOutStrm.writeAligned< sal_uInt32 >( nSize );
    maLargeProps.push_back( ComplexPropVector::value_type( new StringProperty( orValue, nSize ) ) );
    startNextProperty();
}

bool AxBinaryPropertyWriter::finalizeExport()
{
    // write large properties
    maOutStrm.align( 4 );
    if( !maLargeProps.empty() )
    {
        for( ComplexPropVector::iterator aIt = maLargeProps.begin(), aEnd = maLargeProps.end(); ensureValid() && (aIt != aEnd); ++aIt )
        {
            (*aIt)->writeProperty( maOutStrm );
            maOutStrm.align( 4 );
        }
    }

    mnBlockSize = maOutStrm.tell() - mnPropFlagsStart;

    // write stream properties (no stream alignment between properties!)
    if( !maStreamProps.empty() )
        for( ComplexPropVector::iterator aIt = maStreamProps.begin(), aEnd = maStreamProps.end(); ensureValid() && (aIt != aEnd); ++aIt )
           (*aIt)->writeProperty( maOutStrm );

    sal_Int64 nPos = maOutStrm.tell();
    maOutStrm.seek( mnPropFlagsStart - sizeof( mnBlockSize ) );

    maOutStrm << mnBlockSize;

    if( mb64BitPropFlags )
        maOutStrm << mnPropFlags;
    else
        maOutStrm << sal_uInt32( mnPropFlags );

    maOutStrm.seek( nPos );
    return true;
}

bool AxBinaryPropertyWriter::ensureValid( bool bCondition )
{
    mbValid = mbValid && bCondition && !maOutStrm.isEof();
    return mbValid;
}

bool AxBinaryPropertyWriter::startNextProperty( bool bSkip )
{
    // if we are skipping then we clear the flag
    setFlag( mnPropFlags, mnNextProp, !bSkip );
    mnNextProp <<= 1;
    return true;
}

// ============================================================================

} // namespace exp
} // namespace ole

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
