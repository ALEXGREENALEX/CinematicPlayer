// Copyright 2023 - 2025 Olexandr Zelenskyi. All Rights Reserved.

using UnrealBuildTool;

public class CinematicPlayer : ModuleRules
{
	public CinematicPlayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.Add(ModuleDirectory);

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"EnhancedInput",
			"LevelSequence",
			"MediaAssets",
			"MovieScene",
			"UMG",
		});
	}
}