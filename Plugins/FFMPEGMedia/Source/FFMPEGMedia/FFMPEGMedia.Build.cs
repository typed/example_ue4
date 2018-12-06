// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class FFMPEGMedia : ModuleRules
{

	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
	}

	private string UProjectPath
	{
		get { return Directory.GetParent(ModulePath).Parent.FullName; }
	}

	public bool LoadFFmpeg(ReadOnlyTargetRules Target)
	{
		bool isLibrarySupported = false;

		if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
		{
			isLibrarySupported = true;

			string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "win32";
			string LibrariesPath = Path.Combine(Path.Combine(Path.Combine(ThirdPartyPath, "ffmpeg", "lib"), "vs"), PlatformString);


			System.Console.WriteLine("... LibrariesPath -> " + LibrariesPath);

			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavcodec.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavdevice.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavfilter.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavformat.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavutil.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libswresample.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libswscale.lib"));

            PublicAdditionalLibraries.Add("Secur32.lib");
            PublicAdditionalLibraries.Add("Bcrypt.lib");

        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			isLibrarySupported = true;
			string LibrariesPath = Path.Combine(Path.Combine(ThirdPartyPath, "ffmpeg", "lib"), "osx");

			System.Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            
            string[] libs = {"libavcodec.58.dylib","libavdevice.58.dylib", "libavfilter.7.dylib", "libavformat.58.dylib", "libavutil.56.dylib", "libswresample.3.dylib", "libswscale.5.dylib", "libpostproc.55.dylib"};
            foreach (string lib in libs)
            {
                PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, lib));
                PublicDelayLoadDLLs.Add(Path.Combine(LibrariesPath, lib));
	            RuntimeDependencies.Add(Path.Combine(LibrariesPath, lib), StagedFileType.NonUFS);
            }

		}

		if (isLibrarySupported)
		{
			// Include path
			PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "ffmpeg", "include"));
		}


		return isLibrarySupported;
	}

	public FFMPEGMedia(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;
		bEnableExceptions = true;
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"Media",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"MediaUtils",
				"RenderCore",
				"FFMPEGMediaFactory",
				"Projects",
			});

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"Media",
			});

		PrivateIncludePaths.AddRange(
			new string[] {
				"FFMPEGMedia/Private",
				"FFMPEGMedia/Private/Player",
				"FFMPEGMedia/Private/FFMPEG",
			});

		LoadFFmpeg(Target);
	}
}
