/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  DRM Crypto functionality
*
*/



/*

            DRMCrypto
            --------------------------

            SW module - ANSI C

Location:           -

Filename:           drmcrypto.c

Document code:      -

/* ------------------------------------------------------------------------- */

/*  1    ABSTRACT
    1.1    Module type
    1.2    Functional description
    1.3    Notes

    2    CONTENTS

    3    GLOSSARY

    4    REFERENCES

    5    EXTERNAL RESOURCES
    5.1    Mandatory include files
    5.2    Library include files
    5.3    Interface include files

    6    LOCAL DEFINITIONS
    6.1    Local include files
    6.2    Local constants
    6.3    Local macros
    6.3.1    dummy_message_type
    6.3.2    generate_connection_address
    6.4    Local data types
    6.5    Local data structures
    6.6    Local function prototypes

    7    MODULE CODE
    7.1     DRMCrypto_Encrypt
	7.2	    DRMCrypto_Decrypt
	7.3		DRMCrypto_EncryptRightsDb
	7.4		DRMCrypto_DecryptRightsVDb
	7.5		DRMCrypto_AddPadding
	7.6		DRMCrypto_RemovePadding
	7.7		DRMCrypto_GenerateKey
	7.8		DRMCrypto_GenerateIV





*/


/*  3    GLOSSARY
		 -
*/

/*  4    REFERENCES

    Specification reference

    DRM Engine Crypto Interface Specification


    Design reference

    -

    Module test specification reference

    -
*/
#ifndef C_DRMCRYPTO_CFILE
#define C_DRMCRYPTO_CFILE

/*  5    EXTERNAL RESOURCES */

/*  5.1    Mandatory include files */

/*  5.2    Library include files */

/*  5.3    Interface include files */
#include "drmcrypto.h"
#include "drmenv.h" /* for DRMEnv_GetRandom */

/*  6    LOCAL DEFINITIONS */

/*  6.1    Local include files */

#include "aes_if.h"

/*  6.2    Local constants */
#define	KEYSEED_NUMBER_OF_INT32		4
#define KEYSEED_LENGTH				16
/*  6.3    Local macros */

/*  6.4    Local data types */

/*  6.5    Local data structures */

/*  6.6    Local function prototypes */

/*  7    MODULE CODE */


/* ========================================================================= */

/*  7.1 */

/* Functional description
 *
 * Encrypt data using specified algorithm
 *
 *
 * Parameters
 *
 * Cipher type
 *
 cType
 *
 * Pointer to encryption key
 *
 pszKey
 *
 * Encryption key length in bytes
 *
 iKeyLen
 *
 * Pointer to initializer vector of encryption
 *
 pszIV
 *
 * Pointer to data to be encrypted
 *
 pszIn
 *
 * Pointer to encrypted data.
 * It can be same pointer as pszOut.
 *
 pszOut
 *
 * Length in bytes of content to be encrypted.
 *
 iInLen
 *
 * Cipher type(AES encryption modes).
 *		AES_MODE_CBC ... AES_MODE_ECB
 *
 uiParameters
 *
 * Return values
 *
 * If encryption is OK, return DRM_ENG_OK, 
 * otherwize DRM_ENG_ERROR.
 */

/* ---------------------------------------------------------------------- */

#ifdef	ENCRYPT_USED
uint8 DRMCrypto_Encrypt( 
		  CipherType				cType, 
		  uint8*					pszKey, 
		  uint16					iKeyLen,
		  uint8*					pszIV,
		  uint8*					pszIn,
		  uint8*					pszOut,
		  uint32					iInLen, 
		  CipherParamType			uiParameters	)
	{
	/* Data structures */

	/* return code
	 */
	uint8 ret = 0;

	/* AES encryption mode
	 */
	uint8 iMode = 0;

	/*  Code  */

	/* check parameter */
	if( !pszKey || !pszIV || !pszIn || !pszOut )
		{
		return DRM_ENG_INVALID_PARAM;
		}

	/* Convert uiParameters to inner interface type
	 */
	if( uiParameters ==  AES_MODE_CBC)
		{
		iMode = AES_CBC;
		}
	else if( uiParameters ==  AES_MODE_ECB)
		{
		iMode = AES_ECB;
		}
	else 
		{
		DEBUG("Crypto Error: invalid uiParameters!")
		return DRM_ENG_ERROR;
		}

	if( cType == CIPHER_AES )
		{
		ret = AESEncrypt( (uint32*)pszKey, (uint16)(iKeyLen*8), (uint32*)pszIV, 
						  (uint32*)pszIn, (uint32*)pszOut, iInLen, iMode );
		if( ret==AES_CRYPTO_OK ) 
			{
			return DRM_ENG_OK;
			}
		else if( ret == AES_CRYPTO_ERR_MEMORY )
			{
			return DRM_ENG_MEM_ERROR;
			}
		else 
			{
			DEBUGD("Crypto Error: AES Encryption Error ", ret)
			return DRM_ENG_ERROR;
			}
		}
	else
		{
		return DRM_ENG_ERROR;
		}
	}
#endif /* #ifdef	ENCRYPT_USED */


/*  7.2 */

/* Functional description
 *
 * Decrypt data using specified algorithm.
 *
 *
 * Parameters
 *
 * Cipher type
 *
 cType
 *
 * Pointer to encryption key
 *
 pszKey
 *
 * Encryption key length in bytes
 *
 iKeyLen
 *
 * Pointer to initializer vector of encryption
 *
 pszIV
 *
 * Pointer to encrypted data which is to be decrypted
 *
 pszIn
 *
 * Pointer to decrypted content.
 * It can be same pointer as pszOut.
 *
 pszOut
 *
 * Length in bytes of content to be decrypted.
 *
 iInLen
 *
 * Cipher type(AES encryption modes).
 *		AES_MODE_CBC ... AES_MODE_ECB
 *
 uiParameters
 *
 * Return values
 *
 * If decryption is OK, return DRM_ENG_OK, 
 * otherwize DRM_ENG_ERROR.
 */

/* ---------------------------------------------------------------------- */


uint8 DRMCrypto_Decrypt( 
		  CipherType				cType,
		  uint8*					pszKey, 
          uint16					iKeyLen,
		  uint8*					pszIV,
		  uint8*					pszIn,
		  uint8*					pszOut,
		  uint32					iInLen,
		  CipherParamType			uiParameters	)
	{
	/* Data structures */

	/* return code
	 */
	uint8 ret = 0;

	/* AES encryption mode
	 */
	uint8 iMode = 0;

	/* Aligned buffer for Key
	 */
	/* uint32*			pKeyAligned=NULL; */

	/* Aligned buffer for IV
	 */
	/* uint32*			pIVAligned=NULL; */

	/* Aligned buffer for Input data
	 */
	/* uint32*			pInAligned=NULL; */

	/* Byte stream pointer
	 */
	/* uint8* pBytes = NULL; */

	/*  Code  */

	/* check parameter */
	if( !pszKey || !pszIV || !pszIn || !pszOut )
		{
		return DRM_ENG_INVALID_PARAM;
		}

	/* Convert uiParameters to inner interface type
	 */
	if( uiParameters ==  AES_MODE_CBC)
		{
		iMode = AES_CBC;
		}
	else if( uiParameters ==  AES_MODE_ECB)
		{
		iMode = AES_ECB;
		}
	else 
		{
		DEBUG("Crypto Error: invalid uiParameters !")
		return DRM_ENG_ERROR;
		}

	if( cType == CIPHER_AES )
		{
			
			ret = AESDecrypt( (uint32*)pszKey, (uint16)(iKeyLen*8), (uint32*)pszIV, 
							  (uint32*)pszIn,  (uint32*)pszOut, iInLen, iMode );

		if( ret==AES_CRYPTO_OK ) 
			{
			ret = DRM_ENG_OK;
			}
		else if( ret == AES_CRYPTO_ERR_MEMORY )
			{
			ret = DRM_ENG_MEM_ERROR;
			}
		else 
			{
			DEBUGD("Crypto Error: AES Decryption Error ", ret)
			ret = DRM_ENG_ERROR;
			}
		}
	else
		{
		ret = DRM_ENG_ERROR;
		}

	return ret;
	}


/*  7.5 */

/* Functional description
 *
 * Adds padding bytes at the end of data.
 *
 *
 * Parameters
 *
 * Pointer to pointer to data.
 * IN: points to data before adding padding bytes.
 * OUT: points to data with added padding bytes.
 * Memory used by input data will be freed inside this function.
 * New memory will be allocated for output data.
 *
 ppData
 *
 * Pointer to data length in bytes
 * IN:  pointer to length of data with padding.
 * OUT: points to length of data without padding.
 *
 pDataLen
 *
 * Cipher block size. 
 * Max 256
 *
 CipBlockSize
 *
 * Specifies used padding method.
 *      PADDING_PKCS7
 *
 uiPaddingMethod
 *
 * Return values
 *
 * If operation is OK, return DRM_ENG_OK, 
 * otherwize DRM_ENG_ERROR.
 */

/* ---------------------------------------------------------------------- */


uint8 DRMCrypto_AddPadding(
		  uint8**				ppData, 
		  uint32*				pDataLen,
		  uint8					CipBlockSize,
		  PaddingMethodType		uiPaddingMethod	)
	{
	/* Data structures */

	/* Number of bytes to add to data
	 */
	uint8	padSize=0;

	/* Address of input data
	 */
	uint8	*pDataIn;

	/* Iterator
	 */
	uint16	i;

	/* return code */
	uint8 ret = DRM_ENG_OK;


	/*  Code  */

	/* check parameter */
	if( !ppData || !pDataLen )
		{
		return DRM_ENG_INVALID_PARAM;
		}

	if( uiPaddingMethod == PADDING_PKCS7)
		{
		/* calculate padding size
		 */
		padSize = (uint8)( CipBlockSize-( *pDataLen % CipBlockSize ) );
	
		/* record input data address
		 */
		pDataIn = *ppData;

		/* allocate memory
		 */
		*ppData = (uint8*)DRM_BLOCK_ALLOC( *pDataLen+padSize ) ;
		if( !(*ppData) )
			{
			return DRM_ENG_MEM_ERROR;
			}

		/* copy data
		 */
		DRM_BLOCK_COPY( *ppData, pDataIn, *pDataLen  );

		/* free memory for input data
		 */
		DRM_BLOCK_DEALLOC( pDataIn);

		/* add padding
		 */
		for( i=0; i<padSize; i++)
			*( (uint8*)(*ppData) + *pDataLen +i) = padSize;

		/* calculate new data length:
		 */
		*pDataLen = *pDataLen+padSize;

		return	DRM_ENG_OK;
		}
	else
		{
		DEBUG( "Cypto Error: invalid uiPaddingMethod !" )
		ret = DRM_ENG_ERROR;
		}

	return ret;
	}



/*  7.6 */

/* Functional description
 *
 * Remove padding bytes from data.
 *
 *
 * Parameters
 *
 * Pointer to pointer to data(with padding bytes).
 * IN: points to data with padding bytes.
 * OUT: points to data without padding bytes.
 *
 ppData
 *
 * Pointer to data length in bytes
 * IN:  points to length of data with padding.
 * OUT: points to length of data without padding.
 *
 pDataLen
 *
 * Specifies used padding method.
 *      PADDING_PKCS7
 *
 uiPaddingMethod
 *
 * Return values
 *
 * If operation is OK, return DRM_ENG_OK, 
 * otherwize DRM_ENG_ERROR.
 */

/* ---------------------------------------------------------------------- */


uint8 DRMCrypto_RemovePadding(
		  uint8**				ppData, 
		  uint32*				pDataLen,
		  PaddingMethodType		uiPaddingMethod	)
	{
	/* Data structures */

	/* Number of bytes of padding data
	 */
	uint8	padSize=0;

		/* return code */
	uint8 ret = DRM_ENG_OK;

	/*  Code  */

	/* check parameter */
	if( !ppData || !pDataLen )
		{
		return DRM_ENG_INVALID_PARAM;
		}

	if( uiPaddingMethod == PADDING_PKCS7)
		{
		/* calculate padding size
		 * padding size is equal to last byte value of data
		 */
		padSize = *( (*ppData) + (*pDataLen) -1);

		if( padSize<1 || padSize>CRYPTO_BLOCK_SIZE )
			{
			DEBUG("Padding Size wrong!")
			return DRM_ENG_ERROR;
			}

		/* calculate new data length:
		 */
		*pDataLen = *pDataLen-padSize;

		return	DRM_ENG_OK;
		}
	else
		{
		DEBUG( "Cypto Error: invalid uiPaddingMethod !" )
		ret = DRM_ENG_ERROR;
		}

	return ret;
	}


/*  7.8 */

/* Functional description
 *
 * Generates an initialization vector for cipher CBC mode. 
 *
 *
 * Parameters
 *
 * Lenght of the IV to be generated in bits. Must be a value between 1-16.
 *
 ivLen
 *
 *
 * Pointer to pointer to the generated IV.
 *
 ppIV
 *
 *
 * Return values
 *
 * If operation is OK, return DRM_ENG_OK, 
 * otherwize DRM_ENG_ERROR.
 */

/* ---------------------------------------------------------------------- */

uint8 DRMCrypto_GenerateIV(uint32 ivLen, uint8 **ppIV)
	{
	/* Data structures */

	/* return code	 */
	uint8	ret=DRM_ENG_OK;

	/* seed */
	uint8* pSeed=NULL;

	/*  Code  */
	
	/* check parameter */
	if( ppIV == NULL || ivLen == 0 || ivLen > 16)
		{
		return DRM_ENG_INVALID_PARAM;
		}

	/* allocate memory for IV */
	*ppIV = DRM_BLOCK_ALLOC( ivLen );
	
	if( !(*ppIV) )
		{
		return DRM_ENG_MEM_ERROR;
		}

	/* generate random number as seed */
	pSeed = DRM_BLOCK_ALLOC( KEYSEED_LENGTH );
	if( !pSeed )
		{
		ret = DRM_ENG_MEM_ERROR;
		}
	else
		{
		ret = DRMEnv_GetRandom( (uint32*)pSeed, KEYSEED_NUMBER_OF_INT32 );
		TRANSLATE_ERROR_CODE( ret );
		}

	/* generate IV by seed */
	if( ret == DRM_ENG_OK )
		{
		/* Just copy the seed as IV. */
		DRM_BLOCK_COPY(*ppIV, pSeed, ivLen<KEYSEED_LENGTH?ivLen:KEYSEED_LENGTH );
		}
	else /* free memory */
		{
		DRM_BLOCK_DEALLOC( *ppIV );
		}

	/* free memory */
	DRM_BLOCK_DEALLOC( pSeed );

	return ret;
	}

#endif /* #ifndef C_DRMCRYPTO_CFILE */
/* End of File */

