#pragma once

#include "CinematicPlayerContent.h"
#include "CinematicPlayerCutscene.generated.h"

class ULevelSequence;
class ALevelSequenceActor;
class ULevelSequencePlayer;

UCLASS(Abstract, HideDropdown, Blueprintable)
class CINEMATICPLAYER_API ACinematicPlayerCutscene : public ACinematicPlayerContent
{
	GENERATED_BODY()

public:
	// Begin ACinematicPlayerContent overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OpenAndPlayContent() override;
	virtual void Skip() override;
	virtual void Pause() override;
	virtual void Resume() override;
	// End ACinematicPlayerContent overrides

private:
	UFUNCTION()
	void OnPlayCallback();

	UFUNCTION()
	void OnStopCallback();

	UFUNCTION()
	void OnFinishedCallback();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CinematicPlayer")
	ULevelSequence* LevelSequence;

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<ALevelSequenceActor> LevelSequenceActor;

	UPROPERTY(Transient)
	TWeakObjectPtr<ULevelSequencePlayer> LevelSequencePlayer;
};
