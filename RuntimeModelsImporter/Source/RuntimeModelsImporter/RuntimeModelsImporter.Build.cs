// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class RuntimeModelsImporter : ModuleRules
{
    public RuntimeModelsImporter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore",
            "EnhancedInput", "ProceduralMeshComponent",
            "AssetRegistry", "RenderCore", "RHI", "MeshDescription",
            "StaticMeshDescription", "ImageWrapper"
        });

        string PluginRoot = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));

        // ✅ Path to Assimp inside plugin folder
        string AssimpPath = Path.Combine(PluginRoot, "ThirdParty", "Assimp");

        // ✅ Include Assimp headers
        PublicIncludePaths.Add(Path.Combine(AssimpPath, "include"));

        // ✅ Link Assimp static lib
        PublicAdditionalLibraries.Add(Path.Combine(AssimpPath, "lib", "assimp-vc143-mt.lib"));

        // ✅ Delay-load Assimp DLL
        PublicDelayLoadDLLs.Add("assimp-vc143-mt.dll");

        // ✅ RuntimeDependencies for packaged build
        string DLLPath = Path.Combine(AssimpPath, "bin", "assimp-vc143-mt.dll");
        if (File.Exists(DLLPath))
        {
            RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/assimp-vc143-mt.dll", DLLPath);
        }
        else
        {
            System.Console.WriteLine("❌ Assimp DLL missing: " + DLLPath);
        }

        // ✅ Enable RTTI (Assimp uses dynamic_cast etc.)
        bUseRTTI = true;
		
		PublicIncludePaths.Add(Path.Combine(PluginRoot, "ThirdParty/DirectXTex/Include"));
		PublicAdditionalLibraries.Add(Path.Combine(PluginRoot, "ThirdParty/DirectXTex/Lib/Win64/DirectXTex.lib"));

    }
}