UE project Root에 ThirdParty 폴더 생성 -> EnhancedDirectInput 폴더 생성 -> Include, lib 폴더 생성
 -> include 폴더에 EnhancedDirectInput.h 파일 복사 -> lib 폴더에 EnhancedDirectInput.lib 파일 복사

[UE project Root]/Binaries/Win64 폴더에 EnhancedDirectInput.dll 파일 복사

[UE project Root]/Source/[UE project]/[UE project].Build.cs 파일에 내용 추가

'''
using UnrealBuildTool;
using System.IO;

public class [UE project] : ModuleRules
{
    // 아래 내용 추가
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }

    public [UE project](ReadOnlyTargetRules Target) : base(Target)
	  {
		    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		    PrivateDependencyModuleNames.AddRange(new string[] {  });

        // 아래 내용 추가
		    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "EnhancedDirectInput", "include"));
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "EnhancedDirectInput", "lib", "EnhancedDirectInput.lib"));
        PublicDelayLoadDLLs.Add("EnhancedDirectInput.dll");
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "../../Binaries/Win64/EnhancedDirectInput.dll"));
	  }
}
'''
