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
* Description:  OMA DRM 2.0 Key Storage
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <asymmetric.h>
#include <symmetric.h>
#include <hash.h>
#include <asn1dec.h>
#include <x509cert.h>
#include "DrmKeyStorage.h"

#ifdef _DEBUG
#define LOGGING
#endif
/*
#ifdef LOGGING
_LIT(KLogDir, "DRM");
_LIT(KLogName, "KeyStorage.log");
#include "flogger.h"
#define LOG(string) \
    RFileLogger::Write(KLogDir, KLogName, \
        EFileLoggingModeAppend, string);
#define LOGHEX(buffer) \
    RFileLogger::HexDump(KLogDir, KLogName, \
        EFileLoggingModeAppend, _S(""), _S(""), \
        buffer.Ptr(), buffer.Length());
#else
#define LOG(string)
#define LOGHEX(buffer)      
#endif
*/
typedef MDrmKeyStorage*(*TStorageConstructor)(RLibrary);

_LIT(KStdKeyStorageName, "drmstdkeystorage.dll");
_LIT(KBb5KeyStorageName, "drmbb5keystorage.dll");
static const TInt KConstructorOrdinal = 1;

// -----------------------------------------------------------------------------
// MDrmKeyStorage::
// 
// -----------------------------------------------------------------------------
//
MDrmKeyStorage::~MDrmKeyStorage()
    {
    }

EXPORT_C MDrmKeyStorage* DrmKeyStorageNewL()
    {
    RLibrary library;
    MDrmKeyStorage* storage;
    TInt r;
    TStorageConstructor constructor;
    
    r = library.Load(KBb5KeyStorageName);
    if (r != KErrNone)
        {
        r = library.Load(KStdKeyStorageName);
        }
    User::LeaveIfError(r);
    constructor = reinterpret_cast<TStorageConstructor>(
        library.Lookup(KConstructorOrdinal));
    if (constructor == NULL)
        {
        User::Leave(KErrNotFound);
        }
    storage = reinterpret_cast<MDrmKeyStorage*>(constructor(library));
    return storage;
    }

// end of file
