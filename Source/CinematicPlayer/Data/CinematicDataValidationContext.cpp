// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

#include "CinematicDataValidationContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CinematicDataValidationContext)

void FCinematicDataValidationContext::AddError(const FText& Message, const FString& PropertyPath)
{
	AddMessage(EMessageSeverity::Error, Message, PropertyPath);
}

void FCinematicDataValidationContext::AddWarning(const FText& Message, const FString& PropertyPath)
{
	AddMessage(EMessageSeverity::Warning, Message, PropertyPath);
}

void FCinematicDataValidationContext::AddInfoNote(const FText& Message, const FString& PropertyPath)
{
	AddMessage(EMessageSeverity::Info, Message, PropertyPath);
}

void FCinematicDataValidationContext::AddMessage(EMessageSeverity::Type Severity, const FText& Message, const FString& PropertyPath)
{
	Records.Add(FCinematicDataValidationRecord(Severity, Message, PropertyPath));
}

void FCinematicDataValidationContext::AppendMessages(const FCinematicDataValidationContext& Context)
{
	Records.Append(Context.Records);
}

void FCinematicDataValidationContext::ClearMessages()
{
	Records.Empty();
}
