// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class OnlineSubsystemB3atZDirect : ModuleRules
{
	public OnlineSubsystemB3atZDirect(TargetInfo Target)
	{
		Definitions.Add("ONLINESUBSYSTEMDIRECT_PACKAGE=1");
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core", 
				"CoreUObject", 
				"Engine", 
				"Sockets", 
				"OnlineSubsystemB3atZ", 
				"OnlineSubsystemB3atZUtils",
				"Json"
			}
			);
	}
}