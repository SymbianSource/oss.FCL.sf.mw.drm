/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  API wrapper side implementation
*
*/


// INCLUDE FILES
#include    "DcfRep.h"
#include    "DcfEntry.h"
#include    "DcfRepCommon.h"
#include    <e32std.h>
#include    <e32base.h>
#include    <DRMRightsClient.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
_LIT(KSqlOrderList , "select filename,position,cid,group from dcf");
_LIT(KSqlOrderListStart , "select filename,position,cid,group from dcf where cid = '");
_LIT(KSqlOrderListEnd , "' order by filename");
_LIT(KSqlOrderListGroupStart , "select filename,position,cid,group from dcf where group = '");
_LIT(KSqlOrderListGroupEnd , "' order by filename");
_LIT(KSqlGetTtidStart , "select ttid from dcf where cid = '");
_LIT(KSqlGetTtidEnd , "' order by ttid");
_LIT(KSqlGetFileStart , "select filename from dcf where cid = '");
_LIT(KSqlGetFileEnd , "' order by filename");
_LIT(KSqlGetCidStart , "select cid from dcf where filename = '");
_LIT(KSqlGetCidEnd , "' order by cid");
_LIT(KSqlDeleteStart , "delete from dcf where filename = '");
_LIT(KSqlDeleteEnd , "'");
_LIT8(KFlkString, "flk:");
_LIT8(KCidString, "cid:");
// MACROS
// LOCAL CONSTANTS AND MACROS
static const TInt KDcfRepExtra( 100 );
#ifdef RD_MULTIPLE_DRIVE
_LIT( KDbFileLoc, "%c:\\system\\data\\DcfRepDb" );
#endif

// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// From8To16
//
// -----------------------------------------------------------------------------
//
LOCAL_C TInt From8To16( const TDesC8& a8 , HBufC16*& a16 )
    {
    TInt err = KErrNone;
    a16 = HBufC::NewMax( a8.Length() );
    if ( a16 )
        {
        TPtr ptr( a16->Des() );
        ptr.SetLength( a8.Length() );
        for (TInt i = 0 ; i<a8.Length() ; i++ )
            {
            ptr[i] = ( unsigned char ) a8[i];
            }
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// From16To8
//
// -----------------------------------------------------------------------------
//
LOCAL_C TInt From16To8( const TDesC& a16 , HBufC8*& a8 )
    {
    TInt err = KErrNone;
    a8 = HBufC8::NewMax( a16.Length() );
    if ( a8 )
        {
        TPtr8 ptr( a8->Des() );
        ptr.SetLength(a16.Length());
        for (TInt i = 0 ; i<a16.Length() ; i++ )
            {
            ptr[i] = ( unsigned char ) a16[i];
            }
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDcfRep::CDcfRep
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDcfRep::CDcfRep()
    {
    }

// -----------------------------------------------------------------------------
// CDcfRep::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDcfRep::ConstructL()
    {
    User::LeaveIfError(iClient.Connect());
    OpenDatabaseL();
    }

// -----------------------------------------------------------------------------
// CDcfRep::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CDcfRep* CDcfRep::NewL()
    {
    CDcfRep* self = new( ELeave ) CDcfRep;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
EXPORT_C CDcfRep::~CDcfRep()
    {
    iClient.Close();
    iView.Close();
    iDb.Close();
    iDbs.Close();
    }

// -----------------------------------------------------------------------------
// CDcfRep::UpdateL
// Add new entry to database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::UpdateL( const CDcfEntry* aEntry)
    {
    if ( !aEntry || ( aEntry->FileName().Length() <= 0 && aEntry->Cid().Length() <= 0 ) )
        {
        User::Leave(KErrArgument);
        }
    else if ( aEntry->FileName().Length() > 0)
        {
        // Filename is defined. If content ID is defined as well, we igore content ID
        TInt err = 0;
        HBufC8* buf8 = NULL;
        RPointerArray<HBufC8> list8;
        CleanupClosePushL(list8);
        User::LeaveIfError(iClient.AddFile(aEntry->FileName()));
        GetCidL(aEntry->FileName(),list8);
        for (TInt i = 0 ; i<list8.Count() && (!err) ; i++ )
            {
            TParse p;
            err = p.Set(aEntry->FileName(),NULL,NULL);
            if (!err)
                {
                buf8 = list8[i];
                err = SetName(*buf8,p.NameAndExt());
                }
            }
        // clean list first then leave if errors occured
        list8.ResetAndDestroy();
        User::LeaveIfError(err);
        CleanupStack::PopAndDestroy(&list8);
        }
    else
        {
        // only content ID is defined
        TInt err = 0;
        HBufC* buf16 = NULL;
        RPointerArray<HBufC> list16;
        CleanupClosePushL(list16);
        GetFilenameL(aEntry->Cid(),list16);
        for (TInt i = 0 ; i<list16.Count() && (!err) ; i++ )
            {
            if (i==0)
                {
                // set name with the first found file. the name is set only when there is no name set before
                TParse p;
                err = p.Set(*(list16[i]),NULL,NULL);
                if (!err)
                    {
                    err = SetName(aEntry->Cid(),p.NameAndExt());
                    }
                }
            buf16 = list16[i];
            if (!err)
                {
                err = iClient.AddFile(*buf16);
                }
            }
        // clean list first then leave if errors occured
        list16.ResetAndDestroy();
        User::LeaveIfError(err);
        CleanupStack::PopAndDestroy(&list16);
        }
    }


// -----------------------------------------------------------------------------
// CDcfRep::GetTtid
// Get transaction ID from database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::GetTtidL(RArray<TPair>& aList)
    {
    TInt i = 0;
    for (;i<aList.Count();i++)
        {
        if ( !(aList[i].iCid) )
            {
            User::Leave(KErrArgument);
            }
        GetTtidL( *(aList[i].iCid) , aList[i].iTtid);
        }
    }

// -----------------------------------------------------------------------------
// CDcfRep::GetTtidL
// Get transaction ID from database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDcfRep::GetTtidL(const TDesC8& aCid , HBufC8*& aTtid)
    {
    TBool flag = ETrue;
    HBufC* temp = NULL;
    RDbView view;
    CleanupClosePushL(view);
    HBufC* buf = HBufC::NewLC(aCid.Length()+KDcfRepExtra);
    TPtr ptr(buf->Des());
    ptr.SetLength(0);


    User::LeaveIfError(From8To16(aCid,temp));
    CleanupStack::PushL(temp);
    ptr.Append(KSqlGetTtidStart);
    ptr.Append(*temp);
    ptr.Append(KSqlGetTtidEnd);
    CleanupStack::PopAndDestroy(temp); //temp

    User::LeaveIfError(view.Prepare(iDb,TDbQuery(ptr,EDbCompareNormal),TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());
    CleanupStack::PopAndDestroy(buf); //buf
    flag = view.FirstL();
    if (!flag)
        {
        aTtid = NULL;
        }
    else
        {
        view.GetL();
        buf = HBufC::NewL(view.ColLength(1));
        *buf = view.ColDes(1);
        CleanupStack::PushL(buf);
        User::LeaveIfError(From16To8(*buf,aTtid));
        CleanupStack::PopAndDestroy(buf); //buf
        }
    CleanupStack::PopAndDestroy(&view);
    }

// -----------------------------------------------------------------------------
// CDcfRep::GetFileHandle
// Get File Handle for a Dcf associated with a specific CID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::GetFileHandleL(
            const TDesC8& aCid ,
            RFile& aFile , RFs& aFs )
    {
    TInt error = KErrNone;
    TPtr8 data(NULL,0);


    // Try to find the file and get a handle to it with the given string
    TRAP(error, DoGetFileHandleL( aCid, aFile, aFs ));


    // If we can't find a filehandle and the contentid starts with "cid:"
    // We may have a FL + SD content, so change the "cid:" to flk and search
    // again
    if( error == KErrNotFound &&
        !(aCid.Left(KCidString().Length()).Compare(KCidString()) ) )
        {
        HBufC8* url( aCid.AllocLC() );
        data.Set( const_cast<TUint8*>(url->Ptr()), url->Length(), url->Length() );
        data.Replace( 0, KCidString().Length(), KFlkString );

        DoGetFileHandleL( data, aFile, aFs );
        CleanupStack::PopAndDestroy( url );
        }
    else
        {
        User::LeaveIfError( error );
        }
    }



void CDcfRep::DoGetFileHandleL(
            const TDesC8& aCid ,
            RFile& aFile , RFs& aFs )
    {
    TBool flag = ETrue;
    HBufC* temp = NULL;
    RDbView view;
    CleanupClosePushL(view);
    HBufC* buf = HBufC::NewLC(aCid.Length()+KDcfRepExtra);
    TPtr ptr(buf->Des());
    ptr.SetLength(0);


    User::LeaveIfError(From8To16(aCid,temp));
    CleanupStack::PushL(temp);
    ptr.Append(KSqlGetFileStart);
    ptr.Append(*temp);
    ptr.Append(KSqlGetFileEnd);
    CleanupStack::PopAndDestroy(temp); //temp
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(ptr,EDbCompareNormal),TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());
    CleanupStack::PopAndDestroy(buf); //buf
    flag = view.FirstL();
    if (!flag)
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        RDbColReadStream strm;
        TInt err = 0;
        TBool flag = ETrue;
        TUint len = 0;
        while (flag&&view.AtRow())
            {
            view.GetL();
            len = view.ColLength(1);
            temp = HBufC::NewLC(len);
            ptr.Set(temp->Des());
            strm.OpenLC(view,1);
            strm.ReadL(ptr,len);
            CleanupStack::PopAndDestroy(&strm);
            err = aFile.Open(aFs, ptr, EFileRead|EFileShareReadersOrWriters);
            CleanupStack::PopAndDestroy(temp); // temp
            if(err == KErrNotFound || err == KErrAccessDenied)
                {
                view.NextL();
                }
            else
                {
                flag = EFalse;
                }
            }
        User::LeaveIfError(err);
        }
    CleanupStack::PopAndDestroy(&view);
    }



// -----------------------------------------------------------------------------
// CDcfRep::NextL
// Return entries as iteration.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDcfEntry* CDcfRep::NextL()
    {
    CDcfEntry* entry = NULL;
    HBufC16* buf = NULL;
    HBufC8* temp = NULL;
    if (iView.AtRow())
        {
        TInt len = 0;
        TUint position = 0;
        TPtr ptr(NULL,0,0);
        RDbColReadStream strm;

        entry = CDcfEntry::NewL();
        CleanupStack::PushL(entry);
        iView.GetL();

        //Set filename and postion
        len = iView.ColLength(KOrdFilename);
        buf = HBufC::NewLC(len);
        ptr.Set(buf->Des());
        strm.OpenLC(iView,KOrdFilename);
        strm.ReadL(ptr,len);
        position = iView.ColUint16(KOrdPosition);
        entry->SetLocationL(ptr,position);
        CleanupStack::PopAndDestroy(&strm);
        CleanupStack::PopAndDestroy(buf);


        // Set content ID
        len = iView.ColLength(KOrdCid);
        buf = HBufC::NewLC(len);
        ptr.Set(buf->Des());
        strm.OpenLC(iView,KOrdCid);
        strm.ReadL(ptr,len);
        CleanupStack::PopAndDestroy(&strm);
        User::LeaveIfError(From16To8(*buf,temp));
        CleanupStack::PopAndDestroy(buf); // buf
        CleanupStack::PushL(temp);
        entry->SetCidL(*temp);
        CleanupStack::PopAndDestroy(temp);


        //Set Group Id
        len = iView.ColLength(KOrdGroupId);
        buf = HBufC::NewLC(len);
        ptr.Set(buf->Des());
        if (len>0)
            {
            strm.OpenLC(iView,KOrdGroupId);
            strm.ReadL(ptr,len);
            CleanupStack::PopAndDestroy(&strm);
            }
        else
            {
            ptr.SetLength(0);
            }
        User::LeaveIfError(From16To8(*buf,temp));
        CleanupStack::PopAndDestroy(buf); // buf
        CleanupStack::PushL(temp);
        entry->SetGroupIdL(*temp);
        CleanupStack::PopAndDestroy(temp); // temp

        iView.NextL();
        CleanupStack::Pop(entry); //entry
        }
    else
        {
        iView.Reset();
        iView.Close();
        }
    return entry;
    }


// -----------------------------------------------------------------------------
// CDcfRep::OrderListL
// Order all entries from DB.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void  CDcfRep::OrderListL()
    {
    TBool flag = ETrue;

    iView.Close();

    User::LeaveIfError(iView.Prepare(
        iDb,TDbQuery(KSqlOrderList(),EDbCompareNormal),
        TDbWindow::EUnlimited));
    User::LeaveIfError(iView.EvaluateAll());
    flag = iView.FirstL();
    if (!flag)
        {
        iView.Reset();
        iView.Close();
        User::Leave(KErrNotFound);
        }
    }

// -----------------------------------------------------------------------------
// CDcfRep::OrderListL
// Order all entries with specific CID from DB.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void  CDcfRep::OrderListL(const TDesC8& aCid)
    {
    TBool flag = ETrue;
    HBufC* temp = NULL;
    HBufC* buf = NULL;
    TPtr ptr(NULL,0,0);

    iView.Close();

    User::LeaveIfError(From8To16(aCid,temp));
    CleanupStack::PushL(temp);
    buf = HBufC::NewLC(aCid.Length()+KDcfRepExtra);
    ptr.Set(buf->Des());
    ptr.SetLength(0);
    ptr.Append(KSqlOrderListStart);
    ptr.Append(*temp);
    ptr.Append(KSqlOrderListEnd);
    User::LeaveIfError(iView.Prepare(
        iDb,TDbQuery(ptr,EDbCompareNormal),
        TDbWindow::EUnlimited));
    User::LeaveIfError(iView.EvaluateAll());
    flag = iView.FirstL();
    if (!flag)
        {
        // try group id
        iView.Reset();
        iView.Close();
        CleanupStack::PopAndDestroy(buf);
        buf = HBufC::NewLC(aCid.Length()+KDcfRepExtra);
        ptr.Set(buf->Des());
        ptr.SetLength(0);
        ptr.Append(KSqlOrderListGroupStart);
        ptr.Append(*temp);
        ptr.Append(KSqlOrderListGroupEnd);
        User::LeaveIfError(iView.Prepare(
            iDb,TDbQuery(ptr,EDbCompareNormal),
            TDbWindow::EUnlimited));
        User::LeaveIfError(iView.EvaluateAll());
        flag = iView.FirstL();
        if (!flag)
            {
            // not group id
            iView.Reset();
            iView.Close();
            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( temp );
            User::Leave(KErrNotFound);
            }
        }
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( temp );
    }

// -----------------------------------------------------------------------------
// CDcfRep::RemoveDcfFromDbL
// Remove all the entries that have the filename in database.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::RemoveDcfFromDbL( const TDesC& aFileName )
    {

    TPtr ptr(NULL,0,0);
    HBufC* sql = NULL;
    sql= HBufC16::NewLC(KSqlDeleteStart().Length()+aFileName.Length()+KSqlDeleteEnd().Length());
    *sql = KNullDesC;
    ptr.Set(sql->Des());
    ptr.Append(KSqlDeleteStart);
    ptr.Append(aFileName);
    ptr.Append(KSqlDeleteEnd);
    TInt err =iDb.Execute(ptr);
    CleanupStack::PopAndDestroy(sql); // sql
    }

// -----------------------------------------------------------------------------
// CDcfRep::RefreshDcf
// let server scan file system for Dcfs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::RefreshDcf( TRequestStatus& aStatus )
    {
    iClient.ScanDcf(aStatus);
    }

// -----------------------------------------------------------------------------
// CDcfRep::RefreshDcf
// let server scan file system for Dcfs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::RefreshDcf( RPointerArray<HBufC8>& aList , TRequestStatus& aStatus )
    {
    iClient.ScanDcf( aList , aStatus );
    }

// -----------------------------------------------------------------------------
// CDcfRep::SetTtid
// Set transaction ID to all Dcfs associated with a specific CID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::SetTtid(
            RArray<TPair>& aList ,
            TRequestStatus& aStatus,
            TBool aRefresh)
    {
    if (aRefresh)
        {
        iClient.SetTtid( aList , aStatus );
        }
    }


// -----------------------------------------------------------------------------
// CDcfRep::NotifyServerIdle
// notify if server is in idle.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CDcfRep::NotifyServerIdle( TRequestStatus& aStatus )
    {
    iClient.NotifyServerIdle( aStatus );
    }


// -----------------------------------------------------------------------------
// CDcfRep::OpenDatabaseL
// connect database server and open session for it
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDcfRep::OpenDatabaseL()
    {
    User::LeaveIfError(iDbs.Connect());

#ifndef RD_MULTIPLE_DRIVE

    User::LeaveIfError(iDb.Open(iDbs,KDbFileLocation()));

#else //RD_MULTIPLE_DRIVE

    RFs fs;
    TInt driveNumber( -1 );
    TChar driveLetter;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultSystem, driveNumber );

    User::LeaveIfError( fs.Connect() );
    fs.DriveToChar( driveNumber, driveLetter );
    fs.Close();

    TFileName dbFile;
    dbFile.Format( KDbFileLoc, (TUint)driveLetter );

    User::LeaveIfError( iDb.Open( iDbs, dbFile ) );

#endif
    }

// -----------------------------------------------------------------------------
// CDcfRep::SetName
// set name of CID into rights DB
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CDcfRep::SetName(const TDesC8& aCid , const TDesC16& aName)
    {
    RDRMRightsClient r;
    TInt err = 0;
    err = r.Connect();
    if (!err)
        {
        HBufC* buf = NULL;
        err = r.GetName(aCid,buf);
        if ( !err && buf->Des().Length()==0 )
            {
            err = r.SetName(aCid,aName);
            }
        if (err == KErrNotFound)
            {
            err = KErrNone; // if there is no cid, then just ignore adding name to it.
            }
        delete buf;
        }
    r.Close();
    return err;
    }

// -----------------------------------------------------------------------------
// CDcfRep::GetCidL
// get cid list for a filename from repository DB
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDcfRep::GetCidL(const TDesC16& aName , RPointerArray<HBufC8>& aList)
    {
    TBool flag = EFalse;
    HBufC8* temp8 = NULL;
    HBufC* temp16 = NULL;
    RDbView view;
    TInt len = 0;
    CleanupClosePushL(view);
    HBufC* buf = HBufC::NewLC(aName.Length()+KDcfRepExtra);
    TPtr ptr(buf->Des());
    ptr.SetLength(0);
    ptr.Append(KSqlGetCidStart);
    ptr.Append(aName);
    ptr.Append(KSqlGetCidEnd);
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(ptr,EDbCompareNormal),TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());
    CleanupStack::PopAndDestroy(buf); //buf
    flag = view.FirstL();
    if (!flag)
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        while (view.AtRow())
            {
            view.GetL();
            len = view.ColLength(1);
            temp16 = HBufC::NewLC(len);
            ptr.Set(temp16->Des());

            RDbColReadStream strm;
            strm.OpenLC(view,1);
            strm.ReadL(ptr,len);
            CleanupStack::PopAndDestroy(&strm);

            User::LeaveIfError(From16To8(*temp16,temp8));
            CleanupStack::PopAndDestroy(temp16); // temp16
            CleanupStack::PushL(temp8);
            User::LeaveIfError(aList.Append(temp8));
            CleanupStack::Pop(temp8); // temp8
            view.NextL();
            }
        }
    CleanupStack::PopAndDestroy(&view);
    }

// -----------------------------------------------------------------------------
// CDcfRep::GetFilenameL
// get filename list for a cid from repository DB
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDcfRep::GetFilenameL(const TDesC8& aCid , RPointerArray<HBufC>& aList)
    {
    TBool flag = EFalse;
    HBufC* temp = NULL;
    RDbView view;
    TInt len = 0;
    CleanupClosePushL(view);
    HBufC* buf = HBufC::NewLC(aCid.Length()+KDcfRepExtra);
    TPtr ptr(buf->Des());
    ptr.SetLength(0);
    User::LeaveIfError(From8To16(aCid,temp));
    CleanupStack::PushL(temp);
    ptr.Append(KSqlGetFileStart);
    ptr.Append(*temp);
    ptr.Append(KSqlGetFileEnd);
    CleanupStack::PopAndDestroy(temp); //temp
    User::LeaveIfError(view.Prepare(iDb,TDbQuery(ptr,EDbCompareNormal),TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());
    CleanupStack::PopAndDestroy(buf); //buf
    flag = view.FirstL();
    if (!flag)
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        while (view.AtRow())
            {
            RDbColReadStream strm;
            view.GetL();
            len = view.ColLength(1);
            temp = HBufC::NewLC(len);
            ptr.Set(temp->Des());
            strm.OpenLC(view,1);
            strm.ReadL(ptr,len);
            CleanupStack::PopAndDestroy(&strm);
            User::LeaveIfError(aList.Append(temp));
            CleanupStack::Pop(temp); // temp
            view.NextL();
            }
        }
    CleanupStack::PopAndDestroy(&view);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File


