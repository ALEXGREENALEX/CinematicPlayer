// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "CinematicDataValidationContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicDataValidationContainer)

void FCinematicDataValidationContainer::AddError(const FText& Message, const FString& PropertyPath)
{
	AddMessage(EMessageSeverity::Error, Message, PropertyPath);
}

void FCinematicDataValidationContainer::AddWarning(const FText& Message, const FString& PropertyPath)
{
	AddMessage(EMessageSeverity::Warning, Message, PropertyPath);
}

void FCinematicDataValidationContainer::AddInfoNote(const FText& Message, const FString& PropertyPath)
{
	AddMessage(EMessageSeverity::Info, Message, PropertyPath);
}

void FCinematicDataValidationContainer::AddMessage(EMessageSeverity::Type Severity, const FText& Message, const FString& PropertyPath)
{
	Records.Add(FCinematicDataValidationRecord(Severity, Message, PropertyPath));
}

void FCinematicDataValidationContainer::AppendMessages(const FCinematicDataValidationContainer& DataValidationContainer)
{
	Records.Append(DataValidationContainer.Records);
}

void FCinematicDataValidationContainer::ClearMessages()
{
	Records.Empty();
}
