// Copyright 2023 - 2026 Alex Zelenskyi. All Rights Reserved.

#include "PlayCinematicAsync.h"
#include <Engine/AssetManager.h>
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>
#include "Logs/CinematicPlayerLogs.h"
#include "PlayableContent/CinematicPlayerContent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayCinematicAsync)

UPlayCinematicAsync* UPlayCinematicAsync::PlayCinematic(UObject* WorldContext, APlayerController* PlayerController, TSubclassOf<ACinematicPlayerContent> Content,
	bool bAutoDestroy)
{
	if (IsValid(Content))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] Content is Not Valid! Path: {Path}", __FUNCTION__, GetPathNameSafe(WorldContext));
		return nullptr;
	}

	UPlayCinematicAsync* Action = NewObject<UPlayCinematicAsync>(WorldContext);
	Action->PlayerController = PlayerController;
	Action->ContentSoftClass = Content;
	Action->bAutoDestroy = bAutoDestroy;
	Action->RegisterWithGameInstance(WorldContext);
	return Action;
}

UPlayCinematicAsync* UPlayCinematicAsync::PlayCinematicAsync(UObject* WorldContext, APlayerController* PlayerController, TSoftClassPtr<ACinematicPlayerContent> Content,
	int32 AsyncLoadPriority, bool bAutoDestroy)
{
	if (Content.IsNull())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] Content is Null! Path: {Path}", __FUNCTION__, GetPathNameSafe(WorldContext));
		return nullptr;
	}

	UPlayCinematicAsync* Action = NewObject<UPlayCinematicAsync>(WorldContext);
	Action->PlayerController = PlayerController;
	Action->ContentSoftClass = Content;
	Action->AsyncLoadPriority = AsyncLoadPriority;
	Action->bAutoDestroy = bAutoDestroy;
	Action->RegisterWithGameInstance(WorldContext);
	return Action;
}

void UPlayCinematicAsync::Activate()
{
	if (ContentSoftClass.IsNull())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] ContentSoftClass is Null!", __FUNCTION__);
		SetReadyToDestroy();
		return;
	}

	if (bAutoDestroy)
	{
		const UObject* Outer = GetOuter();
		if (IsValid(Outer)) // Register globally to listen for any UObject deletions
		{
			AutoDestroyOwnerPtr = Outer;
			GUObjectArray.AddUObjectDeleteListener(this);
		}
	}

	// Already loaded
	if (ContentSoftClass.IsValid())
	{
		SpawnCinematicPlayerContent(ContentSoftClass.Get());
		return;
	}

	// Load Async
	StreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(ContentSoftClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateWeakLambda(this, [this]
		{
			SpawnCinematicPlayerContent(ContentSoftClass.Get());
			StreamingHandle.Reset();
		}),
		AsyncLoadPriority, false, false, TEXT("QuestAction SpawnWidget: Load Widget Class"));
}

void UPlayCinematicAsync::SetReadyToDestroy()
{
	if (AutoDestroyOwnerPtr)
	{
		GUObjectArray.RemoveUObjectDeleteListener(this);
		AutoDestroyOwnerPtr = nullptr;
	}

	// Stop assets Async loading, release them.
	if (StreamingHandle.IsValid())
	{
		if (StreamingHandle->IsActive())
		{
			StreamingHandle->CancelHandle();
		}

		StreamingHandle.Reset();
	}

	// If already started - don't skip/stop, just unsubscribe from delegates.
	if (PlayableContent.IsValid())
	{
		PlayableContent->OnStart.RemoveDynamic(this, &UPlayCinematicAsync::StartCallback);
		PlayableContent->OnStop.RemoveDynamic(this, &UPlayCinematicAsync::StopCallback);
		PlayableContent->OnFinish.RemoveDynamic(this, &UPlayCinematicAsync::FinishCallback);
		PlayableContent.Reset();
	}

	Super::SetReadyToDestroy();
	MarkAsGarbage();
}

void UPlayCinematicAsync::BeginDestroy()
{
	// Prevent call Cancel() and SetReadyToDestroy() in UCancellableAsyncAction::BeginDestroy() for CDO.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UBlueprintAsyncActionBase::BeginDestroy();
		return;
	}

	Super::BeginDestroy();
}

void UPlayCinematicAsync::NotifyUObjectDeleted(const UObjectBase* Object, int32 Index)
{
	if (Object == AutoDestroyOwnerPtr)
	{
		SetReadyToDestroy();
	}
}

void UPlayCinematicAsync::OnUObjectArrayShutdown()
{
	GUObjectArray.RemoveUObjectDeleteListener(this);
}

#pragma region PlayableContent
APlayerController* UPlayCinematicAsync::GetPlayerController() const
{
	return PlayableContent.IsValid() ? PlayableContent->GetPlayerController() : nullptr;
}

UUserWidget* UPlayCinematicAsync::GetPlayerWidget() const
{
	return PlayableContent.IsValid() ? PlayableContent->GetPlayerWidget() : nullptr;
}

void UPlayCinematicAsync::Stop()
{
	if (PlayableContent.IsValid())
	{
		PlayableContent->Stop();
	}
}

void UPlayCinematicAsync::Pause()
{
	if (PlayableContent.IsValid())
	{
		PlayableContent->Pause();
	}
}

void UPlayCinematicAsync::Resume()
{
	if (PlayableContent.IsValid())
	{
		PlayableContent->Resume();
	}
}
#pragma endregion PlayableContent

void UPlayCinematicAsync::SpawnCinematicPlayerContent(const TSubclassOf<ACinematicPlayerContent> ContentClass)
{
	if (!IsValid(ContentClass))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] ContentClass is Not Valid!", __FUNCTION__);
		SetReadyToDestroy();
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World) || World->bIsTearingDown)
	{
		SetReadyToDestroy();
		return;
	}

	if (!PlayerController.IsValid())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] PlayerController is Not Valid, trying to get First one!", __FUNCTION__);
		PlayerController = World->GetFirstPlayerController();

		if (!PlayerController.IsValid())
		{
			UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] PlayerController is Not Valid!", __FUNCTION__);
			SetReadyToDestroy();
			return;
		}
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags = RF_Transient;
	SpawnInfo.CustomPreSpawnInitialization = [this](AActor* SpawnedActor)
	{
		if (const auto PlayerContent = CastChecked<ACinematicPlayerContent>(SpawnedActor))
		{
			PlayerContent->OnStart.AddDynamic(this, &UPlayCinematicAsync::StartCallback);
			PlayerContent->OnStop.AddDynamic(this, &UPlayCinematicAsync::StopCallback);
			PlayerContent->OnFinish.AddDynamic(this, &UPlayCinematicAsync::FinishCallback);
			PlayerContent->Initialize(PlayerController);
			PlayableContent = PlayerContent;
		}
	};

	World->SpawnActor<ACinematicPlayerContent>(ContentClass, SpawnInfo);
}

void UPlayCinematicAsync::StartCallback()
{
	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
	}
}

void UPlayCinematicAsync::StopCallback()
{
	if (OnStop.IsBound())
	{
		OnStop.Broadcast();
	}

	SetReadyToDestroy();
}

void UPlayCinematicAsync::FinishCallback()
{
	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast();
	}

	SetReadyToDestroy();
}
