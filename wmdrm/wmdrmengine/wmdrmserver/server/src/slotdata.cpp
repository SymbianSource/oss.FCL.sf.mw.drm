/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WMDRM Server implementation
*
*/


#include <ezlib.h>
#include <e32math.h>
#include <symmetric.h>
#include <bacntf.h>
#include <random.h>

#include "wmdrmkeystorage.h"
#include "slotdata.h"
#include "wmdrmserver.h"
#include "slotdatacache.h"
#include "slotenumeratorcache.h"
#include "wmdrmdb.h"

#define _LOGGING_FILE L"wmdrmserver.txt"

#include "flogger.h"
#include "logfn.h"

#ifdef __WINSCW__
_LIT8( KDummyKey, "0123456789012345" );
#endif

CSlotData* CSlotData::NewL( 
    CWmDrmServer* aServer, 
    const TDesC8& aStore,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey )
    {
    LOGFN( "CSlotData::NewL" );
    CSlotData* self = new (ELeave) CSlotData( aServer,
                                              aStore,
                                              aNamespace,
                                              aHashKey,
                                              aUniqueKey );
    return self;
    }

CSlotData::CSlotData( 
    CWmDrmServer* aServer, 
    const TDesC8& aStore,
    const TDesC8& aNamespace,
    const TDesC8& aHashKey,
    const TDesC8& aUniqueKey ) : iServer( aServer ),
                                 iDirty( EFalse ),
                                 iExists( ETrue ),
                                 iReferences( 0 )
    {
    LOGFN( "CSlotData::CSlotData" );
    iStore.Copy( aStore );
    iNamespace.Copy( aNamespace );
    LOG( iNamespace );
    iHashKey.Copy( aHashKey );
    LOG( iHashKey );
    iUniqueKey.Copy( aUniqueKey );
    LOG( iUniqueKey );
    }

CSlotData::~CSlotData()
    {
    LOGFN( "CSlotData::~CSlotData" );
    //FlushL();
    iData.Close();
    }

void CSlotData::CreateL( TInt& aInitialSize )
    {
    LOGFN( "CSlotData::CreateL" );
    LOG2( "Size: %d", aInitialSize );
    LOG1( "*** Create record" );
    if ( aInitialSize > KMaxSlotSize )
        {
        User::Leave( KErrArgument );
        }
	iServer->Db()->CreateRecordL( iStore, iNamespace, iHashKey, iUniqueKey, aInitialSize );
    iData.Close();
    iData.CreateL( aInitialSize );
    iData.SetLength( iData.MaxLength() );
    iData.FillZ();
    iDirty = ETrue;
    iExists = ETrue;
    iOpen = ETrue;
	iServer->EnumeratorCache()->AddEntryL( iStore, iNamespace, iHashKey, iUniqueKey );
    //FlushL();
    }

TInt CSlotData::OpenL( TInt& aCurrentSize )
    {
    TInt r = KErrNone;
    RBuf8 compressedData;
    TUint32 uncompressedSize;
    TPckg<TUint32> pckg( uncompressedSize );
    RBuf8 encryptedData;
    CBufferedDecryptor* decryptor = NULL;
    CModeCBCDecryptor* cbcDecryptor = NULL;
    CAESDecryptor* aesDecryptor = NULL;
    CPaddingPKCS7* padding = NULL;
    TInt size;
    
    LOGFN( "CSlotData::OpenL" );
	LOG3( "Size: %d, data size: %d", aCurrentSize, iData.Length() );
	if ( !IsOpen() )
	    {
	    if ( iExists )
	        {
    	    LOG1( "*** Opening record" );
            TRAP( r, iServer->Db()->ReadRecordL( iStore, iNamespace, iHashKey, iUniqueKey ) );
    	    if ( !r )
    	        {
    	        TBuf8<KAESKeyLength> key;
    	        TBuf8<KAESKeyLength> iv;
                RBuf8 buffer;

                iServer->Db()->GetDataSizeL( size );
                LOG2( "Record size: %d", size );
                if( size <= 0 )
                    {
                    iServer->Db()->DeleteData();
                    iServer->Db()->DeleteRecordL( iStore, iNamespace, iHashKey, iUniqueKey );
                    User::Leave( KErrArgument );
                    }
                    
                encryptedData.CreateL( size - sizeof( TUint32 ) - KAESKeyLength );
                encryptedData.CleanupClosePushL();
                
                compressedData.CreateL( size - sizeof( TUint32 ) - KAESKeyLength );
                compressedData.CleanupClosePushL();
                

                buffer.CreateL( size );
                buffer.CleanupClosePushL();
                iServer->Db()->ReadDataL( buffer );
                //LOGHEX( buffer.Ptr(), buffer.Size() );
                pckg.Copy( buffer.Left( sizeof( TUint32 ) ) );
                LOG2( "Uncompressed size: %d", uncompressedSize );
                iv.Copy( buffer.Mid ( sizeof( TUint32 ), KAESKeyLength ) );
                //LOGHEX( iv.Ptr(), iv.Size () );
                encryptedData.Copy( buffer.Right( size - sizeof( TUint32 ) - KAESKeyLength ) );
                //LOGHEX( encryptedData.Ptr(), encryptedData.Size() );
#ifdef __WINSCW__
                key.Copy( KDummyKey );
#else
                iServer->Cache()->iKeyStorage->GetDeviceSpecificKeyL( key );
#endif
                aesDecryptor = CAESDecryptor::NewL( key );
                CleanupStack::PushL( aesDecryptor );
                
                cbcDecryptor = CModeCBCDecryptor::NewL( aesDecryptor, iv );
                CleanupStack::Pop( aesDecryptor );
                CleanupStack::PushL( cbcDecryptor );
                
                padding = CPaddingPKCS7::NewL( KAESKeyLength );
                CleanupStack::PushL( padding );
                
                decryptor = CBufferedDecryptor::NewL( cbcDecryptor, padding );
                CleanupStack::Pop( 2, cbcDecryptor ); //padding, cbcDecryptor
                CleanupStack::PushL( decryptor );

                decryptor->ProcessFinalL( encryptedData, compressedData );

                iData.Close();
                iData.CreateMaxL( uncompressedSize );
                User::LeaveIfError( uncompress( const_cast<TUint8*>( iData.Ptr() ), &uncompressedSize, 
                                                                     compressedData.Ptr(), compressedData.Size() ) );
                CleanupStack::PopAndDestroy( 4, &encryptedData ); //decryptor, buffer, compressedData, encryptedData
                TRandom::RandomL( key );
                iOpen = ETrue;
                }
            else 
                {
                iExists = EFalse;
                if ( r == KErrPathNotFound )
                    {
                    r = KErrNotFound;
                    }
                }
            }
        else
            {
            r = KErrNotFound;
            }
        }
	else
	    {
	    LOG1( "Record already open" );
	    }
    aCurrentSize = iData.Length();

    return r;
    }

TInt CSlotData::Read( TInt aPosition, TDes8& aBuffer )
    {
    TInt r = KErrNone;
    TInt length;

    LOGFNR( "CSlotData::Read", r );
	if ( IsOpen() )
	    {
	    length = Min( aBuffer.MaxLength(), iData.Length() - aPosition );
	    LOG4( "Read %d bytes from %d, buffer size: %d", length, aPosition, aBuffer.MaxLength() );
        aBuffer.Copy( iData.Mid( aPosition, length ) );
//        LOGHEX( aBuffer.Ptr(), aBuffer.Size() );
        }
    else
        {
        r = KErrNotReady;
        iServer->Cache()->Release( this );
        }
    return r;
    }

TInt CSlotData::WriteL( TInt aPosition, const TDesC8& aBuffer )
    {
    TInt r = KErrNone;

	LOGFNR( "CSlotData::Write", r );
	if ( IsOpen() )
	    {
	    LOG3( "Write %d bytes at %d", aBuffer.Length(), aPosition );
//        LOGHEX( aBuffer.Ptr(), aBuffer.Size() );
	    if ( aBuffer.Length() + aPosition > iData.Length() )
	        {
	        iData.ReAllocL( aBuffer.Length() + aPosition );
	        iData.SetLength( iData.MaxLength() );
	        }
	    iData.Replace( aPosition, aBuffer.Length(), aBuffer );
	    iDirty = ETrue;
	    FlushL();
        }
    else
        {
        r = KErrNotReady;
        iServer->Cache()->Release( this );
        }
    return r;
    }

TInt CSlotData::ResizeL( TInt aNewSize )
    {
    TInt r = KErrNone;

	LOGFNR( "CSlotData::Resize", r );
	if ( IsOpen() )
        {
    	LOG2( "New size: %d", aNewSize );
    	if ( aNewSize > iData.Size() )
    	    {
            iData.ReAllocL( aNewSize );
            }
        iData.SetLength( aNewSize );
        }
    else
        {
        r = KErrNotReady;
        iServer->Cache()->Release( this );
        }
    return r;
    }

TInt CSlotData::DeleteL()
    {
    TInt r = KErrNone;
    
	LOGFNR( "CSlotData::Delete", r );
	iServer->EnumeratorCache()->DeleteEntryL( iStore, iNamespace, iHashKey, iUniqueKey );
	r = iServer->Cache()->Delete( this );
	return r;
    }

TInt CSlotData::Size()
    {
    TInt r;
    
    LOGFNR( "CSlotData::Size", r );
	if ( IsOpen() )
	    {
	    r = iData.Length();
	    }
    else
        {
        r = KErrNotReady;
        iServer->Cache()->Release( this );
        }
	return r;
    }

void CSlotData::Close()
    {
    LOGFN( "CSlotData::Close" );
    iServer->Cache()->Release( this );
    }

void CSlotData::CloseFile()
    {
    LOGFN( "CSlotData::CloseFile" );
    iOpen = EFalse;
    }

void CSlotData::FlushL()
    {
    LOGFN( "CSlotData::FlushL" );
    __UHEAP_MARK;
	if ( IsOpen() && iDirty )
	    {
        TBuf8<KAESKeyLength> iv;
        TBuf8<KAESKeyLength> key;
        TInt size = iData.Size();
        RBuf8 compressedData;
        TUint32 compressedDataLen = size + size / 10 + 12;
        TUint8* compressedDataPtr;
        RBuf8 encryptedData;
        CBufferedEncryptor* encryptor = NULL;
        CModeCBCEncryptor* cbcEncryptor = NULL;
        CAESEncryptor* aesEncryptor = NULL;
        CPaddingPKCS7* padding = NULL;
        RBuf8 buffer;
      
        compressedData.CreateL( compressedDataLen );
        compressedData.CleanupClosePushL();
        compressedDataPtr = const_cast<TUint8*>( compressedData.Ptr() );
        User::LeaveIfError( compress( compressedDataPtr, &compressedDataLen, iData.Ptr(), size ) );
        compressedData.SetLength( compressedDataLen );
        
        encryptedData.CreateL( compressedData.Size() + 2 * KAESKeyLength );
        encryptedData.CleanupClosePushL();

        iv.SetLength( KAESKeyLength );
        TRandom::RandomL( iv );
#ifdef __WINSCW__
        key.Copy( KDummyKey );
#else
        iServer->Cache()->iKeyStorage->GetDeviceSpecificKeyL( key );
#endif
        aesEncryptor = CAESEncryptor::NewL( key );
        CleanupStack::PushL( aesEncryptor );
        
        cbcEncryptor = CModeCBCEncryptor::NewL( aesEncryptor, iv );
        CleanupStack::Pop( aesEncryptor );
        CleanupStack::PushL( cbcEncryptor );
        
        padding = CPaddingPKCS7::NewL( KAESKeyLength );
        CleanupStack::PushL( padding );
        
        encryptor = CBufferedEncryptor::NewL( cbcEncryptor, padding );
        CleanupStack::Pop( 2, cbcEncryptor ); //padding, cbcEncryptor
        CleanupStack::PushL( encryptor );
        
        encryptor->ProcessFinalL( compressedData, encryptedData );
        
        LOG1( "*** Write record" );
        LOG3( "Uncompressed size: %d, compresseded size: %d", size, encryptedData.Size() );
        buffer.CreateL( sizeof( TUint32 ) + iv.Size() + encryptedData.Size() );
        buffer.CleanupClosePushL();
        buffer.Append( TPckgC<TUint32>( size ) );
        buffer.Append( iv );
        buffer.Append( encryptedData );
        //LOGHEX( buffer.Ptr(), buffer.Size() );

        iServer->Db()->WriteDataL( iStore, iNamespace, iHashKey, iUniqueKey, buffer );
        iDirty = EFalse;
        TRandom::RandomL( key );
        CleanupStack::PopAndDestroy( 4, &compressedData ); //buffer, encryptor, encryptedData, compressedData
        }
    __UHEAP_MARKEND;        
    }

TBool CSlotData::IsOpen()
    {
    return iOpen;
    }

