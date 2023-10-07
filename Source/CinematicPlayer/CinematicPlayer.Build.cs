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
			"Engine",
			"LevelSequence",
			"MediaAssets",
			"MovieScene",
			"UMG",
		});
	}
}