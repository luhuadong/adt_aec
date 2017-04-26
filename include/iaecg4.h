#if (defined (WIN32) && defined(_WINDOWS)) || defined(_USRDLL) 
//#include "forcelib.h"
#endif
/*
//============================================================================
//
//    FILE NAME : IAECG4.h
//
//    ALGORITHM : AECG4
//
//    VENDOR    : ADT
//
//    TARGET DSP: C64x
//
//    PURPOSE   : IAECG4 Interface Header
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

#ifndef IAECG4_
#define IAECG4_
#ifdef _TMS320C6X
#include <common/xdm_packages/ti/xdais/std.h>
#include <common/xdm_packages/ti/xdais/xdas.h>
#include <common/xdm_packages/ti/xdais/ialg.h>
#else
#include <common/xdm_packages/ti/xdais/std.h>
#include <common/xdm_packages/ti/xdais/xdas.h>
#include <common/xdm_packages/ti/xdais/ialg.h>
#endif
#include <common/include/adt_typedef_user.h>
#ifdef ECHO_ANALYSIS
#include <../adt_projects/echoanalysis/echoanalysis/ve_param.h>
#endif

#ifdef DUMP_ENABLE
#include <aecg4/private/source/aecg4_dump.h>
#endif

#define SAVE_RESTORE_ACTION_GET_LENGTH 0
#define SAVE_RESTORE_ACTION_SAVE 1
#define SAVE_RESTORE_ACTION_RESTORE 2


// No longer needed
//#define NOISE_REDUCTION_OFF 0
//#define NOISE_REDUCTION_LC 1	/* low complexity noise reduction */
//#define MIN_NOISE_REDUCTION_HC 2	/* high complexity noise reduction 2..31 */

#define CNG_OFF 0
#define CNG_ENABLE_SUBBAND 1
#define CNG_ENABLE_FULLBAND 2

#define MTAB_NRECS 7
/*
// ===========================================================================
// IAECG4_Handle
//
// This handle is used to reference all AECG4 instance objects
*/
typedef struct IAECG4_Obj *IAECG4_Handle;

/*
// ===========================================================================
// IAECG4_Obj
//
// This structure must be the first field of all AECG4 instance objects
*/
typedef struct IAECG4_Obj {
    struct IAECG4_Fxns *fxns;
} IAECG4_Obj;

/*
// ===========================================================================
// IAECG4_Status
//
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
#define MAX_BINS 64

#define AECG4_STATUS_FIELDS \
	Int             size;  /* must be first field of all status structures */ \
    /* Get Time Domain Echo Model Items */\
	XDAS_Int16		*pReturnedModel;		/*Points to location where echo model (time domain coefficients) will be stored */\
	XDAS_Int16		nCoefToReturn;			/*Number of coefficients to return (input to control function) */\
	/* Get Status items */\
	XDAS_Int16      txInPowerdBm10;\
    XDAS_Int16      txOutPowerdBm10;\
    XDAS_Int16      rxInPowerdBm10;\
    XDAS_Int16      rxOutPowerdBm10;\
	XDAS_Int16		residualPowerdBm10;\
    XDAS_Int16      erlDirectdB10;\
    XDAS_Int16      erlIndirectdB10;\
	XDAS_Int16		erldB10BestEstimate;\
	XDAS_Int16		worstPerBinERLdB10BestEstimate;\
	XDAS_Int16		worstPerBinERLdB10BestEstimateConfidence;\
	XDAS_Int16		erledB10;\
	XDAS_Int16		shortTermERLEdB10;\
	XDAS_Int16		instantaneousERLEdB100;\
	XDAS_Int16		dynamicNLPAggressivenessAdjustdB10;\
	XDAS_Int16		shadowERLEdB10;\
	XDAS_Int16		rxVADState;\
	XDAS_Int16		txVADState;\
	XDAS_Int16		rxVADStateLatched;\
	XDAS_Int16		currentBulkDelaySamples;\
	XDAS_Int16		txAttenuationdB10;\
	XDAS_Int16		rxAttenuationdB10;\
	XDAS_Int16		rxOutAttenuationdB10;\
	XDAS_Int16		nlpThresholddB10;\
	XDAS_Int16		nlpSaturateFlag;\
	XDAS_Int16		aecState;\
	XDAS_Int16		sbcngResidualPowerdBm10;\
	XDAS_Int16		sbcngCNGPowerdBm10;\
	XDAS_Int16		rxOutAttendB10;\
	XDAS_Int16		sbMaxAttendB10;\
	XDAS_Int16		sbMaxClipLeveldBm10;\
	XDAS_UInt32		sbInitFlags;\
	XDAS_Int16		txFreqOffsetHz;\
	XDAS_Int16		rxFreqOffsetHz;\
	XDAS_Int16		sbTxVADTotalExceeddBm10;\
	XDAS_Int16		EPLMSResidualSample; \
	XDAS_Int16		EPShadowResidualSample; \
	XDAS_Int16		EPReconvergenceConfidenceFractionQ6; \
	XDAS_Int16		instantaneousERLDirectdB10;\
	XDAS_Int16		estimatedBulkDelaySamples;\
	/* Set Status Items */\
	XDAS_Int16		speakerLevelChangeDeltadB;

typedef struct IAECG4_Status {
	AECG4_STATUS_FIELDS
} IAECG4_Status;

/*
// ===========================================================================
// IAECG4_Cmd
//
// The Cmd enumeration defines the control commands for the AECG4
// control method.
*/
typedef enum IAECG4_Cmd {
  IAECG4_GETSTATUS,
  IAECG4_SETSTATUS,
  IAECG4_PAUSE,
  IAECG4_RESUME,
  IAECG4_GET_TIME_DOMAIN_ECHO_MODEL,
  IAECG4_FREEZE_BG_TO_FG,			/* Reserved */
  IAECG4_UNFREEZE_BG_TO_FG,			/* Reserved */
  IAECG4_FREEZE_FG_TO_SHADOW,		/* Reserved */
  IAECG4_UNFREEZE_FG_TO_SHADOW,		/* Reserved */
  IAECG4_FORCE_USE_SHADOW,			/* Reserved */
  IAECG4_FORCE_USE_FOREGROUND,		/* Reserved */
  IAECG4_RESUME_NORMAL_FILTER_USE,	/* Reserved */
  IAECG4_NOTUSE_TAILSEARCH,
  IAECG4_USE_TAILSEARCH,
  IAECG4_ENABLE_TXAGC_RUN,
  IAECG4_DISABLE_TXAGC_RUN,
  IAECG4_ENABLE_RXAGC_RUN,
  IAECG4_DISABLE_RXAGC_RUN
} IAECG4_Cmd;


/* 
  ==============================================
  Lock Callback Definition - used by AEC to obtain, use, and delete a lock
*/
typedef enum
{
	CREATE_LOCK,
	LOCK,
	UNLOCK,
	DELETE_LOCK
}		LockAction_e;

typedef ADT_UInt32 (LockCallback_t)(	//returns 0 if OK, 1 otherwise
		void *LockHandle,		//If LOCK, UNLOCK, DELETE_LOCK: Handle to lock. Null otherwise
		char *Name,				//If CREATE_LOCK, name of lock. If LOCK or UNLOCK, name of calling function
		LockAction_e Action,	
		void **CreatedLock		//Used only if action is CREATE_LOCK
		);

/* Multi-Mic Definitions */
#define MAX_MICS 6	//USERS MUST NOT CHANGE THIS
#define MAX_MIC_GROUPS MAX_MICS //USERS MUST NOT CHANGE THIS

typedef struct {
	
	XDAS_Int8 TxFlag;
	XDAS_Int8 RxFlag;
	IAECG4_Handle hRx;
	IAECG4_Handle hTx[MAX_MICS];
	ADT_Int16 MMICFrameSize;
}		IMMICAECG4_Obj;
typedef IMMICAECG4_Obj *IMMICAECG4_Handle; 


/* txrxMode definitions */
#define MODE_RX_B 0
#define MODE_TX_B 1
#define MODE_DMNR_B 2
#define MODE_DMNR_PRIMARY_B 3
#define MODE_MMIC_B 4

#define BIT_TO_MASK(b) (1 << b)
#define TXRX_MODE_NORMAL				(BIT_TO_MASK(MODE_RX_B) |	BIT_TO_MASK(MODE_TX_B))
#define TXRX_MODE_RX					BIT_TO_MASK(MODE_RX_B)
#define TXRX_MODE_TX					BIT_TO_MASK(MODE_TX_B)
#define TXRX_MODE_RX_MMIC				(BIT_TO_MASK(MODE_RX_B)	|																	BIT_TO_MASK(MODE_MMIC_B))
#define TXRX_MODE_TX_MMIC				(BIT_TO_MASK(MODE_TX_B) |																	BIT_TO_MASK(MODE_MMIC_B))
#define TXRX_MODE_RX_DMNR				(BIT_TO_MASK(MODE_RX_B)	|																	BIT_TO_MASK(MODE_MMIC_B))
#define TXRX_MODE_DMICNR_TX_PRIMARY		(BIT_TO_MASK(MODE_TX_B) |	BIT_TO_MASK(MODE_DMNR_B) | BIT_TO_MASK(MODE_DMNR_PRIMARY_B)	|	BIT_TO_MASK(MODE_MMIC_B))
#define TXRX_MODE_DMICNR_TX_SECONDARY	(BIT_TO_MASK(MODE_TX_B) |	BIT_TO_MASK(MODE_DMNR_B)									|	BIT_TO_MASK(MODE_MMIC_B))

#define AECG4_ADT_NOT_SINGLE_MIC(mode) (mode & (BIT_TO_MASK(MODE_DMNR_B) | BIT_TO_MASK(MODE_MMIC_B)))
#define AECG4_ADT_IS_SINGLE_MIC(mode) (!(AECG4_ADT_NOT_SINGLE_MIC(mode)))

#define AECG4_ADT_IS_DMNR_MIC(mode) (mode & (BIT_TO_MASK(MODE_DMNR_B)))
#define AECG4_ADT_NOT_DMNR_MIC(mode) (!(AECG4_ADT_IS_DMNR_MIC(mode)))

/*
// ===========================================================================
// IAECG4_Params
//
// This structure defines the creation parameters for all AECG4 objects
*/
//TAG_PARAMS
#define __AECG4_PARAMS \
	LockCallback_t  *lockCallback;			/*0 */\
	XDAS_UInt16 	frameSize;				/*1 */\
	XDAS_UInt8 		antiHowlEnable;			/*2 */\
	XDAS_Int32	 	samplingRate;			/*3 */\
	XDAS_Int32		maxAudioFreq;			/*4*/\
	XDAS_Int16 		fixedBulkDelayMSec;		/*5*/\
	XDAS_Int16		variableBulkDelayMSec;	/*6*/\
	XDAS_Int16		initialBulkDelayMSec;	/*7*/\
    XDAS_Int16 		activeTailLengthMSec;	/*8*/\
    XDAS_Int16 		totalTailLengthMSec;	/*9*/\
	XDAS_Int16		txNLPAggressiveness;	/*10*/\
    XDAS_Int16      maxTxLossSTdB;			/*11 */\
	XDAS_Int16		maxTxLossDTdB;			/*12*/\
    XDAS_Int16      maxRxLossdB;			/*13*/\
	XDAS_Int16		initialRxOutAttendB;	/*14*/\
    XDAS_Int16      targetResidualLeveldBm;	/*15*/\
    XDAS_Int16      maxRxNoiseLeveldBm;		/*16*/\
	XDAS_Int16		worstExpectedERLdB;		/*17*/\
	XDAS_Int16		rxSaturateLeveldBm;		/*18*/\
	XDAS_Int16		noiseReduction1Setting;	/*19*/\
	XDAS_Int16		noiseReduction2Setting;	/*20*/\
	XDAS_Int16		cngEnable;				/*21*/\
	XDAS_Int8		fixedGaindB10;			/*22*/\
    XDAS_Int8       txAGCEnable;			/*23*/\
    XDAS_Int8       txAGCMaxGaindB;			/*24*/\
    XDAS_Int8       txAGCMaxLossdB;			/*25*/\
    XDAS_Int8       txAGCTargetLeveldBm;	/*26*/\
    XDAS_Int8       txAGCLowSigThreshdBm;	/*27*/\
    XDAS_Int8       rxAGCEnable;			/*28*/\
    XDAS_Int8       rxAGCMaxGaindB;			/*29*/\
    XDAS_Int8       rxAGCMaxLossdB;			/*30*/\
    XDAS_Int8       rxAGCTargetLeveldBm;	/*31*/\
    XDAS_Int8       rxAGCLowSigThreshdBm;	/*32*/\
	XDAS_Int8		rxBypassEnable;			/*33*/\
	XDAS_Int16		maxTrainingTimeMSec;	/*34*/\
	XDAS_Int16		trainingRxNoiseLeveldBm;/*35*/\
    XDAS_Int16 *    pTxEqualizerdB10;		/*36*/\
	XDAS_Int8		mipsMemReductionSetting;/*37*/\
	XDAS_Int8		mipsReductionSetting2;	/*38*/\
	XDAS_Int8		txrxMode; /*39*/	/* reserved - must be set to zero*/

#define N_AECG4_PARAMS 40

typedef struct IAECG4_Params {
    Int size;	  /* must be first field of all params structures */
	__AECG4_PARAMS

} IAECG4_Params;
typedef struct
{
	ADT_UInt8 EchoPath;
	ADT_UInt8 Update;
}		IAECG4_SoftResetParams_t;

/*
// ===========================================================================
// IAECG4_PARAMS
//
// Default parameter values for AECG4 instance objects
*/
extern const IAECG4_Params IAECG4_PARAMS;

/*
// ===========================================================================
// IAECG4_Fxns
//
// This structure defines all of the operations on AECG4 objects
*/
typedef struct IAECG4_Fxns {
    IALG_Fxns	ialg;    /* IAECG4 extends IALG */
    XDAS_Void (*apply)(IAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut, XDAS_Int16 * ptrTxIn, XDAS_Int16 * ptrTxOut);
    XDAS_Void (*applyTx)(IAECG4_Handle handle, XDAS_Int16 * ptrTxIn, XDAS_Int16 * ptrTxOut);
    XDAS_Void (*applyRx)(IAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut);
    XDAS_Void (*backgroundHandler)(IAECG4_Handle handle);
	XDAS_Int32 (*saveRestoreState) (IAECG4_Handle handle, XDAS_Int8 *pState, XDAS_Int32 Length, XDAS_Int8 Action);
	Int (*reset) (IAECG4_Handle handle, const IAECG4_Params *iAECG4Params);
	XDAS_Void (*softReset) (IAECG4_Handle handle, const IAECG4_SoftResetParams_t *pSoftResetParams);
/* Multi-Mic APIs */
    XDAS_Void (*applyMMIC)(IMMICAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut, XDAS_Int16 * ptrTxIn[], XDAS_Int16 * ptrTxOut[]);
    XDAS_Void (*backgroundHandlerMMIC)(IMMICAECG4_Handle handle);
	XDAS_Int32 (*saveRestoreStateMMIC) (IMMICAECG4_Handle handle, XDAS_Int8 *pState, XDAS_Int32 Length, XDAS_Int8 Action, XDAS_Int8 MicIndex);
	Int (*resetMMIC) (IMMICAECG4_Handle handle, const IAECG4_Params *iAECG4Params);
	XDAS_Void (*softResetMMIC) (IMMICAECG4_Handle handle, const IAECG4_SoftResetParams_t *pSoftResetParams);
	XDAS_Void (*AECG4_ADT_controlMMIC)(IMMICAECG4_Handle handle, IAECG4_Cmd cmd, XDAS_Int32 MicNumber, IAECG4_Status * status);

} IAECG4_Fxns;



/* Concrete interface to all AECG4 functions */
ADT_API IAECG4_Handle AECG4_ADT_create(const IAECG4_Fxns *fxns, const IAECG4_Params *prms);
ADT_API IMMICAECG4_Handle AECG4_ADT_createMMIC(const IAECG4_Fxns *fxns, const IAECG4_Params *prms, const unsigned char MaxMicrophones, XDAS_UInt8 *pMicGroups);
ADT_API IMMICAECG4_Handle AECG4_ADT_createDMICNR(const IAECG4_Fxns *fxns, const IAECG4_Params *prms);
ADT_API IAECG4_Handle AECG4_ADT_createStatic(IAECG4_Fxns *fxns, IAECG4_Params *params, IALG_MemRec *memTab);
ADT_API void AECG4_ADT_staticAllocHelper(const IAECG4_Params *prms);
ADT_API Int AECG4_ADT_alloc(const IAECG4_Params *prms, struct IAECG4_Fxns **, IALG_MemRec memTab[]);
ADT_API Int AECG4_ADT_control(IAECG4_Handle handle, IAECG4_Cmd cmd, IAECG4_Status *status);
ADT_API Void AECG4_ADT_delete(IAECG4_Handle handle);
ADT_API Void AECG4_ADT_deleteStatic(IAECG4_Handle handle);
ADT_API XDAS_Void AECG4_ADT_apply(IAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut, XDAS_Int16 * ptrTxIn, XDAS_Int16 * ptrTxOut);
ADT_API XDAS_Void AECG4_ADT_applyTx(IAECG4_Handle handle, XDAS_Int16 * ptrTxIn, XDAS_Int16 * ptrTxOut);
ADT_API XDAS_Void AECG4_ADT_applyRx(IAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut);
ADT_API XDAS_Void AECG4_ADT_backgroundHandler(IAECG4_Handle handle);
ADT_API XDAS_Int32 AECG4_ADT_saveRestoreState(IAECG4_Handle handle, XDAS_Int8 *pState, XDAS_Int32 Length, XDAS_Int8 Action);
ADT_API Int AECG4_ADT_reset(IAECG4_Handle handle, const IAECG4_Params *prms);
ADT_API XDAS_Void AECG4_ADT_softReset (IAECG4_Handle handle, const IAECG4_SoftResetParams_t *pSoftResetParams);
ADT_API XDAS_Int16 AECG4_ADT_getTxDelaySamples(IAECG4_Handle Handle);
//Multi-Mic APIs

ADT_API XDAS_Void AECG4_ADT_applyMMIC(IMMICAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut, XDAS_Int16 * ptrTxIn[], XDAS_Int16 * ptrTxOut[]);
ADT_API XDAS_Void AECG4_ADT_applyMMICTx(IMMICAECG4_Handle handle, XDAS_Int16 * ptrTxIn[], XDAS_Int16 * ptrTxOut[]);
ADT_API XDAS_Void AECG4_ADT_applyMMICRx(IMMICAECG4_Handle handle, XDAS_Int16 * ptrRxIn, XDAS_Int16 * ptrRxOut);

ADT_API XDAS_Void AECG4_ADT_backgroundHandlerMMIC(IMMICAECG4_Handle handle);
ADT_API XDAS_Int32 AECG4_ADT_saveRestoreStateMMIC(IMMICAECG4_Handle handle, XDAS_Int8 *pState, XDAS_Int32 Length, XDAS_Int8 Action, XDAS_Int8 MicIndex);
ADT_API Int AECG4_ADT_resetMMIC(IMMICAECG4_Handle handle, const IAECG4_Params *iAECG4Params);
ADT_API XDAS_Void AECG4_ADT_softResetMMIC(IMMICAECG4_Handle handle, const IAECG4_SoftResetParams_t *pSoftResetParams);
ADT_API XDAS_Int16 AECG4_ADT_getTxDelaySamplesMMIC(IMMICAECG4_Handle Handle);

ADT_API XDAS_Void AECG4_ADT_controlMMIC(IMMICAECG4_Handle handle, IAECG4_Cmd cmd, XDAS_Int32 MicNumber, IAECG4_Status * status);

ADT_API XDAS_Int32 AECG4_ADT_addMic(IAECG4_Handle hRxChannel, IAECG4_Handle hMicChannel, XDAS_Int8 MicIndex, XDAS_Int8 MicGroup);

ADT_API XDAS_Int32 AECG4_ADT_enableMic(IMMICAECG4_Handle mmhandle, XDAS_Int8 MicIndex);
ADT_API XDAS_Int32 AECG4_ADT_disableMic(IMMICAECG4_Handle mmhandle, XDAS_Int8 MicIndex);
ADT_API Void AECG4_ADT_deleteMMIC(IMMICAECG4_Handle handle);
ADT_API Void AECG4_ADT_updateMaxAudioFreqMMIC(IMMICAECG4_Handle handle, XDAS_Int32 maxAudioFreq);
ADT_API Void AECG4_ADT_updateMaxAudioFreq (IAECG4_Handle handle, XDAS_Int32 maxAudioFreq);

ADT_API Void AECG4_ADT_updateTxAGC (IAECG4_Handle handle, XDAS_Int8 TxAGCSetting);
ADT_API Void AECG4_ADT_updateRxAGC (IAECG4_Handle handle, XDAS_Int8 RxAGCSetting);
ADT_API Void AECG4_ADT_updateTxAGCMMIC (IMMICAECG4_Handle mmhandle, XDAS_Int8 MicIndex, XDAS_Int8 TxAGCRuntimeEnable);
ADT_API Void AECG4_ADT_updateRxAGCMMIC (IMMICAECG4_Handle mmhandle, XDAS_Int8 RxAGCRuntimeEnable);

// These are not per-instance functions
typedef struct 
{
	ADT_UInt8 NR2;
	ADT_UInt8 Mobile;
	ADT_UInt8 MultiMic;
	ADT_UInt8 DualMicNoiseReduction;
}		AECG4_BuildInfo_t;
ADT_API char *AECG4_ADT_getBuildInfoString();
ADT_API AECG4_BuildInfo_t *AECG4_ADT_getBuildInfo();

ADT_API XDAS_Int16 AECG4_ADT_getParamCount();
ADT_API XDAS_Int16 AECG4_ADT_getParamNames(char *pParamNameTable[], ADT_Int16 TableSize);
#ifdef DUMP_ENABLE
/*
// ===========================================================================
// AECG4_dumpInit
*/

ADT_API XDAS_Void AECG4_ADT_dumpInit(IAECG4_Handle handle, const DumpParams_t *pDumpParams);
ADT_API XDAS_Void AECG4_ADT_dumpInitMMIC(IMMICAECG4_Handle handle, const DumpParams_t *pDumpParams);

/*
// ===========================================================================
// AECG4_dump
*/
ADT_API XDAS_Void AECG4_ADT_dump(IAECG4_Handle handle);
ADT_API XDAS_Void AECG4_ADT_dumpMMIC(IMMICAECG4_Handle mmhandle);

#endif

#endif	/* IAECG4_ */
