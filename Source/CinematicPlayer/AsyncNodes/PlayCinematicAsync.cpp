// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "PlayCinematicAsync.h"
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>
#include "Logs/CinematicPlayerLogs.h"
#include "PlayableContent/CinematicPlayerContent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayCinematicAsync)

UPlayCinematicAsync* UPlayCinematicAsync::PlayCinematicAsync(UObject* WorldContextObject, APlayerController* PlayerController, TSoftClassPtr<ACinematicPlayerContent> Content)
{
	UPlayCinematicAsync* Action = NewObject<UPlayCinematicAsync>(WorldContextObject);
	Action->PlayerController = PlayerController;
	Action->ContentSoftClass = Content;

	if (!Action->PlayerController.IsValid())
	{
		const UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
		if (IsValid(World))
		{
			Action->PlayerController = World->GetFirstPlayerController();
		}
	}

	return Action;
}

void UPlayCinematicAsync::Activate()
{
	Super::Activate();

	UWorld* World = GetWorld();
	if (!IsValid(World) || World->bIsTearingDown)
	{
		return;
	}

	if (!PlayerController.IsValid())
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] PlayerController is Not Valid!", FUNC_STR);
		SetReadyToDestroy();
		return;
	}

	const TSubclassOf<ACinematicPlayerContent> ContentClass = ContentSoftClass.IsNull() ? nullptr : ContentSoftClass.LoadSynchronous();
	if (!IsValid(ContentClass))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] ContentClass is Not Valid!", FUNC_STR);
		SetReadyToDestroy();
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags = RF_Transient;
	SpawnInfo.CustomPreSpawnInitalization = [this](AActor* SpawnedActor)
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

void UPlayCinematicAsync::SetReadyToDestroy()
{
	if (PlayableContent.IsValid())
	{
		PlayableContent->OnStart.RemoveDynamic(this, &UPlayCinematicAsync::StartCallback);
		PlayableContent->OnStop.RemoveDynamic(this, &UPlayCinematicAsync::StopCallback);
		PlayableContent->OnFinish.RemoveDynamic(this, &UPlayCinematicAsync::FinishCallback);
		// PlayableContent will destroy themselves later. If we try to Destroy it, ensure will occur!
		PlayableContent.Reset();
	}

	Super::SetReadyToDestroy();

	MarkAsGarbage();
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
