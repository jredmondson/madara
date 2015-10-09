

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Sep 19 14:01:34 2011
 */
/* Compiler settings for ..\..\..\..\..\..\..\Program Files\GME\SDK\BON\Common\ComponentLib.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __ComponentLib_h__
#define __ComponentLib_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __MgaComponent_FWD_DEFINED__
#define __MgaComponent_FWD_DEFINED__

#ifdef __cplusplus
typedef class MgaComponent MgaComponent;
#else
typedef struct MgaComponent MgaComponent;
#endif /* __cplusplus */

#endif 	/* __MgaComponent_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "core.h"
#include "mga.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_ComponentLib_0000_0000 */
/* [local] */ 

#ifndef INTERFACECOLL_INCLUDED
#define INTERFACECOLL_INCLUDED
extern "C++"											
{														
	template<class COLL> struct TypeName_MgaColl2Elem		
	{														
		typedef IUnknown element_type;						
		typedef IUnknown collection_type;					
	};														
	template<class ELEM> struct TypeName_MgaElem2Coll		
	{														
		typedef IUnknown element_type;						
		typedef IUnknown collection_type;					
	};														
}														
#define TYPENAME_COLL2ELEM(COLL) typename ::TypeName_MgaColl2Elem<COLL>::element_type
#define TYPENAME_ELEM2COLL(ELEM) typename ::TypeName_MgaElem2Coll<ELEM>::collection_type
#endif


extern RPC_IF_HANDLE __MIDL_itf_ComponentLib_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ComponentLib_0000_0000_v0_0_s_ifspec;


#ifndef __MgaComponentLib_LIBRARY_DEFINED__
#define __MgaComponentLib_LIBRARY_DEFINED__

/* library MgaComponentLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBIDMgaComponentLib;

EXTERN_C const CLSID CLSIDMgaComponent;

#ifdef __cplusplus

class DECLSPEC_UUID("B47E411A-4357-44B7-93B2-22BDF3931D36")
MgaComponent;
#endif
#endif /* __MgaComponentLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


