#include "DirectInputActor.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Windows/MinWindows.h"
#include "Misc/App.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"

ADirectInputActor::ADirectInputActor()
{
    PrimaryActorTick.bCanEverTick = true;
    DLLHandle = nullptr;
}

void ADirectInputActor::BeginPlay()
{
    Super::BeginPlay();

    if (!LoadDirectInputDLL())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load DirectInput DLL!"));
        return;
    }

    HWND GameWindowHandle = nullptr;

    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
    {
        GameWindowHandle = reinterpret_cast<HWND>(
            GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle()
            );
    }

    if (!GameWindowHandle)
    {
        GameWindowHandle = reinterpret_cast<HWND>(GetModuleHandle(nullptr));
    }

    if (!GameWindowHandle)
    {
        GameWindowHandle = reinterpret_cast<HWND>(GetModuleHandle(nullptr));
    }

    if (GameWindowHandle && InitializeInput(GameWindowHandle))
    {
        UE_LOG(LogTemp, Warning, TEXT("DirectInput Initialized Successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize DirectInput!"));
    }
}

void ADirectInputActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 게임이 종료될 때 DLL 자원 해제
    FreeDirectInputDLL();
    Super::EndPlay(EndPlayReason);
}

void ADirectInputActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDLLLoaded && ReadJoystick)
    {
        DIJOYSTATE2 rawState;
        if (ReadJoystick(&rawState))
        {
            FControllerInputState processedState;

            // --- 모든 축 매핑 및 정규화 ---
            // DirectInput의 축 값 범위는 보통 0 ~ 65535 입니다.
            // 언리얼에서 사용하기 편한 -1.0 ~ 1.0 으로 변환합니다.
            processedState.AxisX = (rawState.lX / 32767.5f) - 1.0f; // Roll
            processedState.AxisY = (rawState.lY / 32767.5f) - 1.0f; // Pitch
            processedState.AxisZ = (rawState.lZ / 32767.5f) - 1.0f; // Knob VRA
            processedState.RotX = (rawState.lRx / 32767.5f) - 1.0f; // Knob VRB
            processedState.RotY = (rawState.lRy / 32767.5f) - 1.0f; // Throttle Raw
            processedState.RotZ = (rawState.lRz / 32767.5f) - 1.0f; // Yaw

            // --- 모든 버튼 매핑 ---
            // 버튼 1번부터 16번까지의 상태를 배열에 저장합니다.
            for (int32 i = 0; i < 16; ++i)
            {
                // 0x80 비트가 켜져 있으면 버튼이 눌린 상태입니다.
                processedState.Buttons[i] = (rawState.rgbButtons[i] & 0x80) != 0;
            }

            // --- 변환된 데이터 사용 (예: 로그 출력) ---
            // Y 회전 값은 스로틀이므로 0~1 범위로 다시 계산하는 것이 좋습니다.
            float Throttle = 1.0f - ((processedState.RotY + 1.0f) / 2.0f);

            UE_LOG(LogTemp, Log, TEXT("Roll:%.2f, Pitch:%.2f, Yaw:%.2f, Thr:%.2f, Btn1:%d, Btn5:%d"),
                processedState.AxisX,
                processedState.AxisY,
                processedState.RotZ,
                Throttle,
                processedState.Buttons[0], // 버튼 1 (SWA)
                processedState.Buttons[4]  // 버튼 5 (SWC)
            );

            // 이제 processedState 변수를 다른 액터로 전달하여 모든 입력을 사용할 수 있습니다.

            CurrentState = processedState;
        }
    }
}

bool ADirectInputActor::LoadDirectInputDLL()
{
    // DLL 경로 설정
    //FString DLLPath = FPaths::ProjectBinariesDir() / TEXT("Win64/EnhancedDirectInput.dll");
    FString DLLPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/Win64/EnhancedDirectInput.dll"));

    if (!FPaths::FileExists(DLLPath))
    {
        UE_LOG(LogTemp, Error, TEXT("EnhancedDirectInput.dll not found at path: %s"), *DLLPath);
        return false;
    }

    DLLHandle = FPlatformProcess::GetDllHandle(*DLLPath);
    if (DLLHandle == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load EnhancedDirectInput.dll!"));
        return false;
    }

    // DLL에서 함수 주소 가져오기
    InitializeInput = (_InitializeInput)FPlatformProcess::GetDllExport(DLLHandle, TEXT("InitializeInput"));
    ReadJoystick = (_ReadJoystick)FPlatformProcess::GetDllExport(DLLHandle, TEXT("ReadJoystick"));
    ShutdownInput = (_ShutdownInput)FPlatformProcess::GetDllExport(DLLHandle, TEXT("ShutdownInput"));

    if (InitializeInput && ReadJoystick && ShutdownInput)
    {
        bIsDLLLoaded = true;
        UE_LOG(LogTemp, Warning, TEXT("EnhancedDirectInput.dll and all functions loaded successfully!"));
        return true;
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to get one or more functions from DLL."));
    FreeDirectInputDLL(); // 하나라도 실패하면 핸들 해제
    return false;
}

void ADirectInputActor::FreeDirectInputDLL()
{
    if (bIsDLLLoaded && ShutdownInput)
    {
        ShutdownInput();
        UE_LOG(LogTemp, Warning, TEXT("Called ShutdownInput() in DLL."));
    }

    if (DLLHandle != nullptr)
    {
        FPlatformProcess::FreeDllHandle(DLLHandle);
        DLLHandle = nullptr;
        bIsDLLLoaded = false;
        UE_LOG(LogTemp, Warning, TEXT("EnhancedDirectInput.dll unloaded."));
    }
}








//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "CPP/DirectInputActor.h"
//
//// Sets default values
//ADirectInputActor::ADirectInputActor()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//}
//
//// Called when the game starts or when spawned
//void ADirectInputActor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void ADirectInputActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//

