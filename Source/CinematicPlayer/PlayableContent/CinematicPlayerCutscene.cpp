#include "CinematicPlayerCutscene.h"
#include <Containers/Ticker.h>
#include <Engine/World.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include "Logs/CinematicPlayerLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerCutscene)

ACinematicPlayerCutscene::ACinematicPlayerCutscene(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  LevelSequenceActorClass(ALevelSequenceActor::StaticClass())
{
}

void ACinematicPlayerCutscene::BeginPlay()
{
	if (!IsValid(LevelSequence))
	{
		UE_LOGFMT(LogCinematicPlayerContent, Error, "[{FUNC}] LevelSequence is Not Valid!", FUNC_STR);
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
		if (ALevelSequenceActor* SequenceActor = CastChecked<ALevelSequenceActor>(SpawnedActor))
		{
			ULevelSequencePlayer* SequencePlayer = SequenceActor->GetSequencePlayer();
			if (IsValid(SequencePlayer))
			{
				SequencePlayer->OnPlay.AddDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
				SequencePlayer->OnFinished.AddDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);
			}

			SequenceActor->SetSequence(LevelSequence);
			SequenceActor->InitializePlayer();
		}
	};

	LevelSequenceActor = World->SpawnActor<ALevelSequenceActor>(LevelSequenceActorClass, SpawnInfo);

	Super::BeginPlay();
}

void ACinematicPlayerCutscene::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer))
		{
			SequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
			SequencePlayer->OnStop.RemoveDynamic(this, &ACinematicPlayerCutscene::OnStopCallback);
			SequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);
			SequencePlayer->Stop();
		}

		LevelSequenceActor->Destroy(false, false);
		LevelSequenceActor.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerCutscene::OpenAndPlayContent()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid())
		{
			SequencePlayer->Play();
		}
	}
}

void ACinematicPlayerCutscene::Stop()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid())
		{
			SequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinishedCallback);
			SequencePlayer->OnStop.AddDynamic(this, &ACinematicPlayerCutscene::OnStopCallback);
			SequencePlayer->Stop();
		}
	}
}

void ACinematicPlayerCutscene::Pause()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid() && SequencePlayer->IsPlaying())
		{
			SequencePlayer->Pause();
		}
	}
}

void ACinematicPlayerCutscene::Resume()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid() && SequencePlayer->IsPaused())
		{
			SequencePlayer->Play();
		}
	}
}

void ACinematicPlayerCutscene::OnPlayCallback()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid())
		{
			SequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlayCallback);
		}

		PlaybackStarted();
	}
}

void ACinematicPlayerCutscene::OnStopCallback()
{
	PlaybackStopped();

	// Delay until next frame (Can't Destroy LevelSequenceActor before RunLatentActions() will be called in UMovieSceneSequencePlayer)
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float InDelta)
	{
		Destroy(false, false);
		return false;
	}));
}

void ACinematicPlayerCutscene::OnFinishedCallback()
{
	PlaybackFinished();

	// Delay until next frame (Can't Destroy LevelSequenceActor before RunLatentActions() will be called in UMovieSceneSequencePlayer)
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float InDelta)
	{
		Destroy(false, false);
		return false;
	}));
}
