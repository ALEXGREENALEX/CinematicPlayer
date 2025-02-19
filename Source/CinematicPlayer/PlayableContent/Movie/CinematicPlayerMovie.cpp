// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "CinematicPlayerMovie.h"
#include <GameFramework/PlayerController.h>
#include <FileMediaSource.h>
#include <MediaPlayer.h>
#include "Logs/CinematicPlayerLogs.h"
#include "Data/CinematicDataValidationContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicPlayerMovie)

void ACinematicPlayerMovie::BeginPlay()
{
	if (!IsValid(MediaPlayer))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] MediaPlayer is Not Valid!", FUNC_STR);
		Destroy();
		return;
	}

	MediaPlayer->PlayOnOpen = false;
	MediaPlayer->SetLooping(false);

	MediaPlayer->OnMediaOpened.AddDynamic(this, &ACinematicPlayerMovie::OnMediaOpened_Callback);
	MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &ACinematicPlayerMovie::OnMediaOpenFailed_Callback);
	MediaPlayer->OnMediaClosed.AddDynamic(this, &ACinematicPlayerMovie::OnMediaClosed_Callback);
	MediaPlayer->OnEndReached.AddDynamic(this, &ACinematicPlayerMovie::OnEndReached_Callback);

	Super::BeginPlay();
}

void ACinematicPlayerMovie::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(MediaPlayer))
	{
		MediaPlayer->OnMediaOpened.RemoveDynamic(this, &ACinematicPlayerMovie::OnMediaOpened_Callback);
		MediaPlayer->OnMediaOpenFailed.RemoveDynamic(this, &ACinematicPlayerMovie::OnMediaOpenFailed_Callback);
		MediaPlayer->OnMediaClosed.RemoveDynamic(this, &ACinematicPlayerMovie::OnMediaClosed_Callback);
		MediaPlayer->OnEndReached.RemoveDynamic(this, &ACinematicPlayerMovie::OnEndReached_Callback);

		MediaPlayer->Close();
	}

	Super::EndPlay(EndPlayReason);
}

void ACinematicPlayerMovie::OpenAndPlayContent()
{
	if (!IsValid(MediaPlayer) || !IsValid(MediaFile) || !MediaPlayer->OpenSource(MediaFile))
	{
		UE_LOGFMT(LogCinematicPlayer, Error, "[{FUNC}] Can't Play MediaFile!", FUNC_STR);
		PlaybackStopped();
	}
}

#pragma region Data Validation
#if WITH_EDITOR
void ACinematicPlayerMovie::ValidateData(FCinematicDataValidationContainer& DataValidationContainer) const
{
	if (!IsValid(MediaFile))
	{
		DataValidationContainer.AddWarning(INVTEXT("MediaFile is Not Valid or Empty!"));
	}

	if (!IsValid(MediaPlayer))
	{
		DataValidationContainer.AddWarning(INVTEXT("MediaPlayer is Not Valid or Empty!"));
	}

	Super::ValidateData(DataValidationContainer);
}
#endif
#pragma endregion Data Validation

void ACinematicPlayerMovie::Stop()
{
	if (IsValid(MediaPlayer))
	{
		MediaPlayer->Close();
	}
}

void ACinematicPlayerMovie::Pause()
{
	if (bCanPause && IsValid(MediaPlayer) && MediaPlayer->CanPause() && MediaPlayer->IsPlaying())
	{
		MediaPlayer->Pause();
	}
}

void ACinematicPlayerMovie::Resume()
{
	if (bCanPause && IsValid(MediaPlayer) && !MediaPlayer->IsPlaying())
	{
		MediaPlayer->Play(); // Start or Resume Playback

		if (!MediaPlayer->IsPaused()) // Game was paused on Start
		{
			PlaybackStarted();
		}
	}
}

void ACinematicPlayerMovie::OnMediaOpened_Callback(FString OpenedUrl)
{
	if (IsValid(MediaPlayer))
	{
		const bool bNeedPauseOnStart = bCanPause && OwningPlayerController.IsValid() && OwningPlayerController->IsPaused();
		if (!bNeedPauseOnStart)
		{
			MediaPlayer->Play();
			PlaybackStarted();
		}
	}
	else // On some Error
	{
		PlaybackStopped();
	}
}

void ACinematicPlayerMovie::OnMediaOpenFailed_Callback(FString FailedUrl)
{
	PlaybackStopped();
}

void ACinematicPlayerMovie::OnMediaClosed_Callback()
{
	PlaybackStopped();
}

void ACinematicPlayerMovie::OnEndReached_Callback()
{
	PlaybackFinished();
}
