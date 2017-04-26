/*___________________________________________________________________________
 |
 |   File: adt_typedef_user.h
 |
 |   This file contains the Adaptive Digital Technologies, Inc. 
 |   word length assignments based on processor type. This file
 |   is intended to be used in conjunction with Adaptive Digital's algorithms
 |   and is included by the algorithm public (user) header files.
 |   
 |   This software is the property of Adaptive Digital Technologies, Inc.
 |   Unauthorized use is prohibited.
 |
 |   Copyright (c) 2000-2011, Adaptive Digital Technologies, Inc.
 |
 |   www.adaptivedigital.com
 |   610-825-0182
 |
 |______________________________________________________________________________
*/
//.S
#ifndef _ADT_TYPEDEF_USER
#define _ADT_TYPEDEF_USER
#ifdef __cplusplus
   #define ADT_FT extern "C"
#else
   #define ADT_FT extern
#endif
#ifdef _WIN32
   #ifndef WIN32
      #define WIN32
   #endif
#endif

// ADT_API is used on Windows platforms to make symbols available to the DLL user
#ifdef WIN32
# ifdef _USRDLL_ADT
#  ifdef __cplusplus
#   define ADT_API extern "C" __declspec(dllexport)
#  else
#   define ADT_API __declspec(dllexport)
#  endif
# elif defined(_USING_ADT_DLL)
#  ifdef __cplusplus
#   define ADT_API extern "C" __declspec(dllimport)
#  else
#   define ADT_API __declspec(dllimport)
#  endif
# else
#  ifdef __cplusplus
#   define ADT_API extern "C"
#  else
#   define ADT_API
#  endif
# endif
#else	// Not WIN32
#  ifdef __cplusplus
#   define ADT_API extern "C"
#  else
#   define ADT_API
# endif
#endif


#ifdef __TMS470__
     #define __arm
#endif 


   #if defined (_TMS320C5XX)
      typedef short int           ADT_Bool;    //16b
      typedef signed char         ADT_Int8;    //16b
      typedef unsigned int        ADT_UInt8;   //16b
      typedef   signed short int  ADT_Int16;   //16b
      typedef unsigned short int  ADT_UInt16;  //16b
      typedef   signed long int   ADT_Int32;   //32b
      typedef unsigned long int   ADT_UInt32;  //32b
      typedef float               ADT_Float32; //32b
      typedef   signed long int   ADT_Int40;   //40b
      typedef unsigned long int   ADT_UInt40;  //40b
      typedef   signed long int   ADT_Int64;   //40b
      typedef unsigned long int   ADT_UInt64;  //40b
      #define DSP_TYPE 54
      #define LowBitFirst 0


   #elif defined (__TMS320C55XX__)
      typedef short int           ADT_Bool;    //16b
      typedef signed char         ADT_Int8;    //16b
      typedef unsigned char       ADT_UInt8;   //16b
      typedef   signed short int  ADT_Int16;   //16b
      typedef unsigned short int  ADT_UInt16;  //16b
      typedef   signed long int   ADT_Int32;   //32b
      typedef unsigned long int   ADT_UInt32;  //32b
      typedef float               ADT_Float32; //32b
      typedef   signed long long  ADT_Int40;   //40b
      typedef unsigned long long  ADT_UInt40;  //40b
      typedef   signed long long int  ADT_Int64;   //64b
      typedef unsigned long long  ADT_UInt64;  //unsigned 64 b

   #elif defined (_TMS320C6X)

      typedef int                 ADT_Bool;    //32b
      typedef signed char         ADT_Int8;    // 8b
      typedef unsigned char       ADT_UInt8;   // 8b
      typedef   signed short int  ADT_Int16;   //16b
      typedef unsigned short int  ADT_UInt16;  //16b
      typedef   signed int        ADT_Int32;   //32b
      typedef unsigned int        ADT_UInt32;  //32b
      typedef float               ADT_Float32; //32b

#ifdef __TI_INT40_T__
      typedef __int40_t           ADT_Int40;   //40b
      typedef unsigned __int40_t  ADT_UInt40;  //40b
#else      
      typedef   signed long int   ADT_Int40;   //40b
      typedef unsigned long int   ADT_UInt40;   //40b
#endif
      
#if (__COMPILER_VERSION__ <= 500)
      typedef double              ADT_Int64;   //64b
#else
      typedef   signed long long int   ADT_Int64;   //64b
      typedef unsigned long long int   ADT_UInt64;   //64b
#endif

   
   #elif defined (WIN32)
      typedef int                 ADT_Bool;    //32b
      typedef   signed char       ADT_Int8;    // 8b
      typedef unsigned char       ADT_UInt8;   // 8b
      typedef   signed short int  ADT_Int16;   //16b
      typedef unsigned short int  ADT_UInt16;  //16b
      typedef   signed long int   ADT_Int32;   //32b
      typedef unsigned long int   ADT_UInt32;  //32b
      typedef float               ADT_Float32; //32b
      typedef __int64             ADT_Int40;   //64b
      typedef __int64             ADT_Int64;    // 64b
      typedef unsigned __int64    ADT_UInt64;    // 64b

   #elif defined (LINUX) || defined (LINUX32) || defined(__arm)  || defined(__i386) ||  defined(__arm__) || defined (__APPLE_CC__) || defined(__GNUC__)
      typedef int                 ADT_Bool;    //32b
      typedef   signed char       ADT_Int8;    // 8b
      typedef unsigned char       ADT_UInt8;   // 8b
      typedef   signed short int  ADT_Int16;   //16b
      typedef unsigned short int  ADT_UInt16;  //16b
   #if defined(__LP64__)
      typedef   signed int   ADT_Int32;        //32b
      typedef unsigned int        ADT_UInt32;  //32b
   #else
      typedef   signed long int   ADT_Int32;   //32b
      typedef unsigned long int   ADT_UInt32;  //32b
   #endif
      typedef float               ADT_Float32; //32b
      //typedef   signed long long int  __int64;     //64b
      typedef   signed long long int ADT_Int40;   //64b
      typedef   signed long long int ADT_Int64;   //64b
      typedef unsigned long long int ADT_UInt64;   //64b

   #endif 



#define ADT_TRUE           1
#define ADT_FALSE          0


// Definitions relating to the Memory Record data type
// Memory Attributes


typedef enum  ADT_MemAttributes_e{
    ADT_SCRATCH,           /**< Scratch memory. */
    ADT_PERSIST,           /**< Persistent memory. */
    ADT_WRITEONCE          /**< Write-once persistent memory. */
} ADT_MemAttributes_e;

#define ADT_MPROG  0x0008  /**< Program memory space bit. */
#define ADT_MXTRN  0x0010  /**< External memory space bit. */

typedef enum  ADT_MemSpace_e {
    ADT_EPROG =            /**< External program memory */
        ADT_MPROG | ADT_MXTRN,

    ADT_IPROG =            /**< Internal program memory */
        ADT_MPROG,

    ADT_ESDATA =           /**< Off-chip data memory (accessed sequentially) */
        ADT_MXTRN + 0,

    ADT_EXTERNAL =         /**< Off-chip data memory (accessed randomly) */
        ADT_MXTRN + 1,

    ADT_DARAM0 = 0,        /**< Dual access on-chip data memory */
    ADT_DARAM1 = 1,        /**< Block 1, if independant blocks required */

    ADT_SARAM  = 2,        /**< Single access on-chip data memory */
    ADT_SARAM0 = 2,        /**< Block 0, equivalent to ADT_SARAM */
    ADT_SARAM1 = 3,        /**< Block 1, if independant blocks required */

    ADT_DARAM2 = 4,        /**< Block 2, if a 3rd independent block required */
    ADT_SARAM2 = 5         /**< Block 2, if a 3rd independent block required */
} ADT_MemSpace_e;

typedef struct
{
	ADT_UInt32			Size;
	ADT_UInt8			Alignment;
	ADT_MemSpace_e		MemSpaceType;
	ADT_MemAttributes_e	MemAttributes;
	void				*base;
}	ADT_MemRec_t;

#define todo(a) 
#endif //_ADT_TYPEDEF
//.E
