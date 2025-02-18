// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#pragma once

#include "CinematicPlayerContent.h"
#include "CinematicPlayerMovie.generated.h"

class UFileMediaSource;
class UMediaPlayer;

/**
 * Allow to play movies by MediaPlayer
 */
UCLASS(Abstract, HideDropdown, Blueprintable)
class CINEMATICPLAYER_API ACinematicPlayerMovie : public ACinematicPlayerContent
{
	GENERATED_BODY()

	// Begin ACinematicPlayerContent overrides
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OpenAndPlayContent() override;

public:
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Resume() override;
	// End ACinematicPlayerContent overrides

private:
	UFUNCTION()
	void OnMediaOpened_Callback(FString OpenedUrl);

	UFUNCTION()
	void OnMediaOpenFailed_Callback(FString FailedUrl);

	UFUNCTION()
	void OnMediaClosed_Callback();

	UFUNCTION()
	void OnEndReached_Callback();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	TObjectPtr<UFileMediaSource> MediaFile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	bool bCanPause = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer", AdvancedDisplay)
	TObjectPtr<UMediaPlayer> MediaPlayer;
};
