// Copyright 2023 - 2026 Alex Zelenskyi. All Rights Reserved.

#pragma once

#include <InputTriggers.h>
#include "InputTriggerChordActionBlocker.generated.h"

/**
 * Block this trigger when Blocking ChordAction triggered.
 */
UCLASS(NotBlueprintable, Meta = (DisplayName = "Chorded Action Blocker", NotInputConfigurable = "true"))
class CINEMATICPLAYER_API UInputTriggerChordActionBlocker : public UInputTriggerChordAction
{
	GENERATED_BODY()

protected:
	virtual ETriggerType GetTriggerType_Implementation() const override { return ETriggerType::Blocker; }
	virtual bool IsBlocking(const ETriggerState State) const override { return State == ETriggerState::Triggered; }
};
