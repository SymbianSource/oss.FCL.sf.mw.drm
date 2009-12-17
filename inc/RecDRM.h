/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Symbian recognizer for DRM protected files
*
*/


#if !defined(__RECDRM_H__)
#define __RECDRM_H__

#include <apmrec.h>

#include <DRMCommon.h>

class CApaDRMRecognizer : public CApaDataRecognizerType
	{
public: // from CApaDataRecognizerType
	CApaDRMRecognizer();
	~CApaDRMRecognizer();
	static CApaDataRecognizerType* CreateRecognizerL();
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL( TInt aIndex ) const;
private: // from CApaDataRecognizerType
	void DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );

private:
  	TBool RecognizeRoapTrigger( const TDesC8& aBuffer );
  	TBool RecognizeODF( const TDesC8& aBuffer );
	TBool iNameRecognized;
	};

#endif
