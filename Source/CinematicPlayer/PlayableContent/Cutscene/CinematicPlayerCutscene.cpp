// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "CinematicPlayerCutscene.h"
#include <Containers/Ticker.h>
#include <Engine/World.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include "Logs/CinematicPlayerLogs.h"
#include "Data/CinematicDataValidationContainer.h"

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
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] LevelSequence is Not Valid!", FUNC_STR);
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
				SequencePlayer->OnPlay.AddDynamic(this, &ACinematicPlayerCutscene::OnPlay_Callback);
				SequencePlayer->OnFinished.AddDynamic(this, &ACinematicPlayerCutscene::OnFinished_Callback);
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
			SequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlay_Callback);
			SequencePlayer->OnStop.RemoveDynamic(this, &ACinematicPlayerCutscene::OnStop_Callback);
			SequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinished_Callback);
			SequencePlayer->Stop();
		}

		LevelSequenceActor->Destroy(false, false);
		LevelSequenceActor.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

#pragma region Data Validation
#if WITH_EDITOR
void ACinematicPlayerCutscene::ValidateData(FCinematicDataValidationContainer& DataValidationContainer) const
{
	if (!IsValid(LevelSequence))
	{
		DataValidationContainer.AddWarning(INVTEXT("LevelSequence is Not Valid or Empty!"));
	}

	if (!IsValid(LevelSequenceActorClass))
	{
		DataValidationContainer.AddWarning(INVTEXT("LevelSequenceActorClass is Not Valid or Empty!"));
	}

	Super::ValidateData(DataValidationContainer);
}
#endif
#pragma endregion Data Validation

void ACinematicPlayerCutscene::Stop()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid())
		{
			SequencePlayer->OnFinished.RemoveDynamic(this, &ACinematicPlayerCutscene::OnFinished_Callback);
			SequencePlayer->OnStop.AddDynamic(this, &ACinematicPlayerCutscene::OnStop_Callback);
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

void ACinematicPlayerCutscene::RequestDestroy()
{
	// Delay until next frame (Can't Destroy LevelSequenceActor before RunLatentActions() will be called in UMovieSceneSequencePlayer)
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float InDelta)
	{
		Super::RequestDestroy(); // Destroy(false, false);
		return false;
	}));
}

void ACinematicPlayerCutscene::OnPlay_Callback()
{
	if (LevelSequenceActor.IsValid())
	{
		ULevelSequencePlayer* SequencePlayer = LevelSequenceActor->GetSequencePlayer();
		if (IsValid(SequencePlayer) && SequencePlayer->IsValid())
		{
			SequencePlayer->OnPlay.RemoveDynamic(this, &ACinematicPlayerCutscene::OnPlay_Callback);
		}

		PlaybackStarted();
	}
}

void ACinematicPlayerCutscene::OnStop_Callback()
{
	PlaybackStopped();
}

void ACinematicPlayerCutscene::OnFinished_Callback()
{
	PlaybackFinished();
}
