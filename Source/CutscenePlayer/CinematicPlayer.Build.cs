using UnrealBuildTool;

public class CinematicPlayer : ModuleRules
{
	public CinematicPlayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new[] { ModuleDirectory });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
			"LevelSequence",
			"MediaAssets",
			"MovieScene",
			"Slate",
			"SlateCore",
			"UMG",
		});
	}
}