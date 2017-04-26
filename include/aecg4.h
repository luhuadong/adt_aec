/*
//============================================================================
//
//    FILE NAME : AECG4.h
//
//    ALGORITHM : AECG4
//
//    VENDOR    : ADT
//
//    TARGET DSP: C64x
//
//    PURPOSE   : This header defines the interface used by clients of the
//                AECG4 module
//
//    Component Wizard for eXpressDSP Version 1.33.00 Auto-Generated Component
//
//    Number of Inputs : 1
//    Number of Outputs: 1
//
//    Creation Date: Thu - 23 October 2008
//    Creation Time: 01:55 PM
//
//============================================================================
*/

#ifndef AECG4_
#define AECG4_
//#include "ti/xdais/alg.h"
//#include "ti/xdais/xdas.h"
#ifdef DUMP_ENABLE
#include <AECG4/private/source/aecg4_dump.h>
#endif

#include "iaecg4.h"
#include <common/include/adt_typedef_user.h>
/*
// ===========================================================================
// AECG4_Handle
//
// This pointer is used to reference all AECG4 instance objects
*/
typedef struct IAECG4_Obj *AECG4_Handle;

typedef IMMICAECG4_Obj *MMICAECG4_Handle;

/*
// ===========================================================================
// AECG4_Params
//
// This structure defines the creation parameters for all AECG4 objects
*/
typedef IAECG4_Params AECG4_Params;

/*
// ===========================================================================
// AECG4_PARAMS
//
// This structure defines the default creation parameters for AECG4 objects
*/
#define AECG4_PARAMS   IAECG4_PARAMS

/*
// ===========================================================================
// AECG4_Status
//
// This structure defines the real-time parameters for AECG4 objects
*/
typedef struct IAECG4_Status   AECG4_Status;

/*
// ===========================================================================
// AECG4_Cmd
//
// This typedef defines the control commands AECG4 objects
*/
typedef IAECG4_Cmd   AECG4_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define AECG4_GETSTATUS    IAECG4_GETSTATUS
#define AECG4_SETSTATUS    IAECG4_SETSTATUS

/*
// ===========================================================================
// AECG4_create
//
// Create an AECG4 instance object (using parameters specified by prms)
*/

ADT_API AECG4_Handle AECG4_create(const IAECG4_Fxns *fxns, const AECG4_Params *prms);
ADT_API IMMICAECG4_Handle AECG4_createMMIC(const IAECG4_Fxns *fxns, const AECG4_Params *prms, const unsigned char NMicrophones);
ADT_API IMMICAECG4_Handle AECG4_createDMICNR(const IAECG4_Fxns *fxns, const AECG4_Params *prms);

ADT_API AECG4_Handle AECG4_createStatic(IALG_Fxns *fxns, AECG4_Params *params, IALG_MemRec *memTab);
ADT_API void AECG4_staticAllocHelper(const AECG4_Params *prms);
ADT_API void AECG4_alloc(const AECG4_Params *prms, IALG_MemRec memTab[]);
/*
// ===========================================================================
// AECG4_control
//
// Get, set, and change the parameters of the AECG4 function (using parameters specified by status).
*/
ADT_API Int AECG4_control(AECG4_Handle handle, AECG4_Cmd cmd, AECG4_Status *status);

/*
// ===========================================================================
// AECG4_delete
// Delete the AECG4 instance object specified by handle
*/
ADT_API Void AECG4_delete(AECG4_Handle handle);
ADT_API Void AECG4_deleteMMIC(IMMICAECG4_Handle handle);

/*
// ===========================================================================
// AECG4_apply
*/
ADT_API XDAS_Void AECG4_apply(AECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut, XDAS_Int16 * ptrTxIn, XDAS_Int16 * ptrTxOut);
ADT_API XDAS_Void AECG4_applyTx(AECG4_Handle handle, XDAS_Int16 * ptrTxIn, XDAS_Int16 * ptrTxOut);
ADT_API XDAS_Void AECG4_applyRx(AECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut);

/*
// ===========================================================================
// AECG4_backgroundHandler
*/
ADT_API XDAS_Void AECG4_backgroundHandler(AECG4_Handle handle);

/*
// ===========================================================================
// AECG4_saveRestoreState
*/
ADT_API XDAS_Int32 AECG4_saveRestoreState(AECG4_Handle handle, XDAS_Int8 *pState, XDAS_Int32 Length, XDAS_Int8 Action);

/*
// ===========================================================================
// AECG4_reset
*/
ADT_API XDAS_Void AECG4_reset(AECG4_Handle handle, const AECG4_Params *prms);

#ifdef DUMP_ENABLE
/*
// ===========================================================================
// AECG4_dumpInit
*/

ADT_API XDAS_Void AECG4_dumpInit(AECG4_Handle handle, const DumpParams_t *pDumpParams);

/*
// ===========================================================================
// AECG4_dump
*/
ADT_API XDAS_Void AECG4_dump(AECG4_Handle handle);

#endif

#endif	/* AECG4_ */
