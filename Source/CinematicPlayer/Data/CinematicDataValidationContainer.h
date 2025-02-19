// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Logging/TokenizedMessage.h>
#include "CinematicDataValidationContainer.generated.h"

USTRUCT(BlueprintType)
struct CINEMATICPLAYER_API FCinematicDataValidationRecord
{
	GENERATED_BODY()

	FCinematicDataValidationRecord() = default;

	FCinematicDataValidationRecord(EMessageSeverity::Type InSeverity, const FText& InMessage, const FString& InPropertyPath = TEXT(""))
		: Severity(InSeverity), Message(InMessage), PropertyPath(InPropertyPath)
	{
	}

	EMessageSeverity::Type Severity = EMessageSeverity::Warning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PropertyPath;
};

/**
 * Used for print Errors and Warnings to MessageLog during BP and Nodes compilation
 */
USTRUCT(BlueprintType)
struct CINEMATICPLAYER_API FCinematicDataValidationContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCinematicDataValidationRecord> Records;

	void AddError(const FText& Message, const FString& PropertyPath = TEXT(""));
	void AddWarning(const FText& Message, const FString& PropertyPath = TEXT(""));
	void AddInfoNote(const FText& Message, const FString& PropertyPath = TEXT(""));
	void AddMessage(EMessageSeverity::Type Severity, const FText& Message, const FString& PropertyPath = TEXT(""));
	void AppendMessages(const FCinematicDataValidationContainer& DataValidationContainer);
	void ClearMessages();
};
