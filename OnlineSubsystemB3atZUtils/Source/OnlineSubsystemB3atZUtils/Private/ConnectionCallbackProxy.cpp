// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved
// Plugin written by Philipp Buerki. Copyright 2017. All Rights reserved..

#include "ConnectionCallbackProxy.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystemB3atZ.h"
#include "OnlineSubsystemBPCallHelper.h"

//////////////////////////////////////////////////////////////////////////
// UDirectConnectionCallbackProxy

UDirectConnectionCallbackProxy::UDirectConnectionCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
{

	OnLoginCompleteDelegate.BindUObject(this, &ThisClass::OnLoginCompleted);

}

UDirectConnectionCallbackProxy* UDirectConnectionCallbackProxy::ConnectToService(UObject* WorldContextObject, class APlayerController* PlayerController)
{
	UDirectConnectionCallbackProxy* Proxy = NewObject<UDirectConnectionCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UDirectConnectionCallbackProxy::Activate()
{
	FOnlineSubsystemBPCallHelper Helper(TEXT("ConnectToService"), GEngine->GetWorldFromContextObject(WorldContextObject));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		IOnlineIdentityPtr OnlineIdentity = Helper.OnlineSub->GetIdentityInterface();
		if (OnlineIdentity.IsValid())
		{
			const int32 ControllerId = CastChecked<ULocalPlayer>(PlayerControllerWeakPtr.Get()->Player)->GetControllerId();

			if (!OnlineIdentity->OnLoginCompleteDelegates[ControllerId].IsBoundToObject(this))
			{
				OnLoginCompleteDelegateHandle = OnlineIdentity->AddOnLoginCompleteDelegate_Handle(ControllerId, OnLoginCompleteDelegate);
				OnlineIdentity->Login(ControllerId, FOnlineAccountCredentials()); /// Probably need to supply real creds here somehow... doesn't apply for all imple however.
			}
			else
			{
				// already trying to login!
			}
			
			// OnQueryCompleted will get called, nothing more to do now
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Connection control not supported by online subsystem"), ELogVerbosity::Warning);
		}
	}

	// Fail immediately
	OnFailure.Broadcast(0);
}

void UDirectConnectionCallbackProxy::OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	FOnlineSubsystemBPCallHelper Helper(TEXT("ConnectToService"), GEngine->GetWorldFromContextObject(WorldContextObject));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		IOnlineIdentityPtr OnlineIdentity = Helper.OnlineSub->GetIdentityInterface();
		if (OnlineIdentity.IsValid())
		{
			OnlineIdentity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, OnLoginCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast(0);
	}
	else
	{
		OnFailure.Broadcast(0);
	}
}
