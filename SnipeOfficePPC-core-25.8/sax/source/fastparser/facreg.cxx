/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <cppuhelper/factory.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/implbase2.hxx>

#include "../tools/fastserializer.hxx"
#include "fastparser.hxx"

using namespace sax_fastparser;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::registry;
using ::rtl::OUString;
using namespace ::com::sun::star::lang;

namespace sax_fastparser
{

//--------------------------------------
// the extern interface
//---------------------------------------
Reference< XInterface > SAL_CALL FastSaxParser_CreateInstance(
    SAL_UNUSED_PARAMETER const Reference< XMultiServiceFactory > & )
    throw(Exception)
{
    FastSaxParser *p = new FastSaxParser;
    return Reference< XInterface > ( (OWeakObject * ) p );
}

Reference< XInterface > SAL_CALL FastSaxSerializer_CreateInstance(
    SAL_UNUSED_PARAMETER const Reference< XMultiServiceFactory > & )
    throw(Exception)
{
    FastSaxSerializer *p = new FastSaxSerializer;
    return Reference< XInterface > ( (OWeakObject * ) p );
}
}

extern "C"
{

SAL_DLLPUBLIC_EXPORT void * SAL_CALL fastsax_component_getFactory(
    const sal_Char * pImplName, void * pServiceManager,
    SAL_UNUSED_PARAMETER void * /*pRegistryKey*/ )
{
    void * pRet = 0;

    if (pServiceManager )
    {
        Reference< XSingleServiceFactory > xRet;
        Reference< XMultiServiceFactory > xSMgr( reinterpret_cast< XMultiServiceFactory * > ( pServiceManager ) );

        OUString aImplementationName( OUString::createFromAscii( pImplName ) );

        if ( aImplementationName == PARSER_IMPLEMENTATION_NAME  )
        {
            xRet = createSingleFactory( xSMgr, aImplementationName,
                                        FastSaxParser_CreateInstance,
                                        FastSaxParser::getSupportedServiceNames_Static() );
        }
        else if ( aImplementationName == SERIALIZER_IMPLEMENTATION_NAME  )
        {
            xRet = createSingleFactory( xSMgr, aImplementationName,
                                        FastSaxSerializer_CreateInstance,
                                        FastSaxSerializer::getSupportedServiceNames_Static() );
        }

        if (xRet.is())
        {
            xRet->acquire();
            pRet = xRet.get();
        }
    }

    return pRet;
}


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
