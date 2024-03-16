#include "CinematicPlayerCutscene.h"
#include <Engine/World.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include "Logs/CinematicPlayerLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerCutscene)

ACinematicPlayerCutscene::ACinematicPlayerCutscene(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  LevelSequence(nullptr),
	  LevelSequenceActorClass(ALevelSequenceActor::StaticClass())
{
}

void ACinematicPlayerCutscene::BeginPlay()
{
	if (!IsValid(LevelSequence))
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: LevelSequence is Not Valid!"), FUNC_STR);
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World) || World->bIsTearingDown)
	{
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.Owner = this;
	SpawnInfo.CustomPreSpawnInitalization = [this](AActor* SpawnedActor)
	{
		if (const auto LvlSequenceActor = CastChecked<ALevelSequenceActor>(SpawnedActor))
		{
			LvlSequenceActor->SetSequence(LevelSequence);
			LvlSequenceActor->InitializePlayer();
		}
	};

	LevelSequenceActor = World->SpawnActor<ALevelSequenceActor>(LevelSequenceActorClass, SpawnInfo);

	LevelSequencePlayer = LevelSequenceActor->SequencePlayer;
	if (!LevelSequencePlayer.IsValid())
	{
		UE_LOG(LogCinematicPlayerContent, Error, TEXT("%s :: LevelSequencePlayer is Not Valid!"), FUNC_STR);
		Destroy();
		return;
	}

	LevelSequencePlayer->OnPlay.AddDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
	LevelSequencePlayer->OnFinished.AddDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);

	Super::BeginPlay();
}

void ACinematicPlayerCutscene::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
		LevelSequencePlayer->OnStop.RemoveDynamic(this, &ACinematicPlayerCutscene::OnStopCallback);
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);
		LevelSequencePlayer->Stop();
		LevelSequencePlayer.Reset();
	}

	if (LevelSequenceActor.IsValid())
	{
		LevelSequenceActor->SetSequence(nullptr);
		LevelSequenceActor->SequencePlayer = nullptr;
		LevelSequenceActor->Destroy(false, false);
		LevelSequenceActor.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerCutscene::OpenAndPlayContent()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->Play();
	}
}

void ACinematicPlayerCutscene::Stop()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);
		LevelSequencePlayer->OnStop.AddDynamic(this, &ACinematicPlayerCutscene::OnStopCallback);
		LevelSequencePlayer->Stop();
	}
}

void ACinematicPlayerCutscene::Pause()
{
	if (LevelSequencePlayer.IsValid() && LevelSequencePlayer->IsPlaying())
	{
		LevelSequencePlayer->Pause();
	}
}

void ACinematicPlayerCutscene::Resume()
{
	if (LevelSequencePlayer.IsValid() && LevelSequencePlayer->IsPaused())
	{
		LevelSequencePlayer->Play();
	}
}

void ACinematicPlayerCutscene::OnPlayCallback()
{
	if (LevelSequencePlayer.IsValid())
	{
		LevelSequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
	}

	PlaybackStarted();
}

void ACinematicPlayerCutscene::OnStopCallback()
{
	StopAndDestroy();
}

void ACinematicPlayerCutscene::OnFinishedCallback()
{
	FinishAndDestroy();
}
