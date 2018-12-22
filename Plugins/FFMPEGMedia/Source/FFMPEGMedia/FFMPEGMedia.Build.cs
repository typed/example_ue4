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

		string PlatformString = "win64";
        
        if (Target.Platform == UnrealTargetPlatform.Win64) {
            PlatformString = "win64";
        }
        else if (Target.Platform == UnrealTargetPlatform.Win32) {
            PlatformString = "win32";
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac) {
            PlatformString = "mac";
        }
        else if (Target.Platform == UnrealTargetPlatform.Android) {
            PlatformString = "android";
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS) {
            PlatformString = "ios";
        }

        string LibrariesPath = Path.Combine(ThirdPartyPath, "ffmpeg", "lib", PlatformString);
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "ffmpeg", "include"));

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
		{

			System.Console.WriteLine("... LibrariesPath -> " + LibrariesPath);

			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavcodec.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavdevice.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavfilter.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavformat.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavutil.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libswresample.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libswscale.a"));

            PublicAdditionalLibraries.Add("Secur32.lib");
            PublicAdditionalLibraries.Add("Bcrypt.lib");

        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			
            System.Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavcodec.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavdevice.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavfilter.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavformat.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libavutil.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libswresample.a"));
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libswscale.a"));

            PublicAdditionalLibraries.Add("z");
            PublicAdditionalLibraries.Add("bz2");
            PublicAdditionalLibraries.Add("iconv");
            PublicAdditionalFrameworks.Add(new UEBuildFramework("AudioToolbox"));
            PublicAdditionalFrameworks.Add(new UEBuildFramework("VideoToolbox"));

        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {

            System.Console.WriteLine("... LibrariesPath -> " + LibrariesPath);

            PublicLibraryPaths.Add(LibrariesPath + "/armeabi-v7a");
            PublicLibraryPaths.Add(LibrariesPath + "/arm64-v8a");

            PublicAdditionalLibraries.Add("avcodec");
            PublicAdditionalLibraries.Add("avdevice");
            PublicAdditionalLibraries.Add("avfilter");
            PublicAdditionalLibraries.Add("avformat");
            PublicAdditionalLibraries.Add("avutil");
            PublicAdditionalLibraries.Add("swresample");
            PublicAdditionalLibraries.Add("swscale");

        }

		return true;
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
