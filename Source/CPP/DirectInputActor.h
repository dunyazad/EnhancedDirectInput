#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include <dinput.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include "EnhancedDirectInput.h"
#include "DirectInputActor.generated.h"

// DLL에서 가져올 함수들의 포인터 타입 정의
typedef bool (*_InitializeInput)(HWND hwnd);
typedef bool (*_ReadJoystick)(DIJOYSTATE2* outState);
typedef void (*_ShutdownInput)();

// 모든 축과 버튼 상태를 저장할 구조체
USTRUCT(BlueprintType)
struct FControllerInputState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float AxisX;

    UPROPERTY(BlueprintReadOnly)
    float AxisY;

    UPROPERTY(BlueprintReadOnly)
    float AxisZ;

    UPROPERTY(BlueprintReadOnly)
    float RotX;

    UPROPERTY(BlueprintReadOnly)
    float RotY;

    UPROPERTY(BlueprintReadOnly)
    float RotZ;

    UPROPERTY(BlueprintReadOnly)
    TArray<bool> Buttons;

    FControllerInputState()
    {
        Buttons.Init(false, 16); // 16개 버튼 초기화
    }
};

UCLASS()
class UE_DRONE_DEMO_API ADirectInputActor : public AActor
{
    GENERATED_BODY()

public:
    ADirectInputActor();

    UFUNCTION(BlueprintCallable, Category = "Input")
    FControllerInputState GetControllerState() const { return CurrentState; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    // DLL 핸들 및 함수 포인터 변수
    void* DLLHandle;

    _InitializeInput InitializeInput;
    _ReadJoystick ReadJoystick;
    _ShutdownInput ShutdownInput;

    FControllerInputState CurrentState;

    bool bIsDLLLoaded = false;

    // DLL 로드 및 함수 바인딩
    bool LoadDirectInputDLL();
    // DLL 언로드
    void FreeDirectInputDLL();
};





//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "DirectInputActor.generated.h"
//
//UCLASS()
//class UE_DRONE_DEMO_API ADirectInputActor : public AActor
//{
//	GENERATED_BODY()
//	
//public:	
//	// Sets default values for this actor's properties
//	ADirectInputActor();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//};

