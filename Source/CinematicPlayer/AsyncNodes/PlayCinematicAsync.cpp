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
		UE_LOG(LogCinematicPlayer, Error, TEXT("%s :: PlayerController is Not Valid!"), FUNC_STR);
		SetReadyToDestroy();
		return;
	}

	const TSubclassOf<ACinematicPlayerContent> ContentClass = ContentSoftClass.IsNull() ? nullptr : ContentSoftClass.LoadSynchronous();
	if (!IsValid(ContentClass))
	{
		UE_LOG(LogCinematicPlayer, Error, TEXT("%s :: ContentClass is Not Valid!"), FUNC_STR);
		SetReadyToDestroy();
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Name = ContentClass->GetFName();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags = RF_Transient;
	SpawnInfo.bHideFromSceneOutliner = true;
	SpawnInfo.CustomPreSpawnInitalization = [this](AActor* SpawnedActor)
	{
		if (const auto PlayerContent = CastChecked<ACinematicPlayerContent>(SpawnedActor))
		{
			PlayerContent->OnStart.AddDynamic(this, &UPlayCinematicAsync::StartCallback);
			PlayerContent->OnStop.AddDynamic(this, &UPlayCinematicAsync::StopCallback);
			PlayerContent->OnFinish.AddDynamic(this, &UPlayCinematicAsync::FinishCallback);
			PlayerContent->Initialize(PlayerController);
		}
	};

	PlayableContent = World->SpawnActor<ACinematicPlayerContent>(ContentClass, SpawnInfo);
}

void UPlayCinematicAsync::SetReadyToDestroy()
{
	if (PlayableContent.IsValid())
	{
		PlayableContent->OnStart.RemoveDynamic(this, &UPlayCinematicAsync::StartCallback);
		PlayableContent->OnStop.RemoveDynamic(this, &UPlayCinematicAsync::StopCallback);
		PlayableContent->OnFinish.RemoveDynamic(this, &UPlayCinematicAsync::FinishCallback);

		PlayableContent->Destroy();
		PlayableContent.Reset();
	}

	Super::SetReadyToDestroy();
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
