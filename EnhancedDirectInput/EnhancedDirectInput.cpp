#include "pch.h"
#include "EnhancedDirectInput.h"

// 전역 변수로 DirectInput 객체와 디바이스를 관리합니다.
static LPDIRECTINPUT8       g_pDI = nullptr;
static LPDIRECTINPUTDEVICE8 g_pJoystick = nullptr;

// DllMain 함수: DLL이 프로세스에 로드되거나 언로드될 때 호출됩니다.
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


// 연결된 조이스틱 장치를 찾기 위한 콜백 함수입니다.
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
    // 찾은 첫 번째 장치를 사용하기 위해 디바이스 객체를 생성합니다.
    HRESULT hr = g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, NULL);

    if (FAILED(hr))
    {
        return DIENUM_CONTINUE; // 실패 시 다른 장치를 계속 검색합니다.
    }

    return DIENUM_STOP; // 장치를 찾았으므로 열거를 중지합니다.
}


// --- 언리얼에서 호출할 함수들의 실제 구현 ---

bool InitializeInput(HWND hwnd)
{
    if (g_pDI) return true; // 이미 초기화되었다면 아무것도 하지 않음

    HRESULT hr;

    // DirectInput 8 객체를 생성합니다.
    hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&g_pDI, NULL);
    if (FAILED(hr)) return false;

    // 연결된 게임 컨트롤러(조이스틱)를 찾습니다.
    hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
    if (FAILED(hr) || g_pJoystick == nullptr)
    {
        g_pDI->Release();
        g_pDI = nullptr;
        return false;
    }

    // 조이스틱의 데이터 형식을 설정합니다. (DIJOYSTATE2 사용)
    hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2);
    if (FAILED(hr))
    {
        g_pJoystick->Release();
        g_pJoystick = nullptr;
        g_pDI->Release();
        g_pDI = nullptr;
        return false;
    }

    // 협조 레벨을 설정합니다. 백그라운드에서도 입력을 받도록 합니다.
    g_pJoystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);

    // 장치에 대한 제어권을 획득합니다.
    g_pJoystick->Acquire();

    return true;
}

bool ReadJoystick(DIJOYSTATE2* outState)
{
    if (g_pJoystick == nullptr) return false;

    HRESULT hr = g_pJoystick->Poll();
    if (FAILED(hr))
    {
        // 제어권을 잃었을 경우 다시 획득 시도
        hr = g_pJoystick->Acquire();
        while (hr == DIERR_INPUTLOST)
        {
            hr = g_pJoystick->Acquire();
        }
        if (FAILED(hr)) return false;
    }

    // 장치의 현재 상태를 가져옵니다.
    hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), outState);
    if (FAILED(hr)) return false;

    return true;
}

void ShutdownInput()
{
    if (g_pJoystick)
    {
        g_pJoystick->Unacquire();
        g_pJoystick->Release();
        g_pJoystick = nullptr;
    }
    if (g_pDI)
    {
        g_pDI->Release();
        g_pDI = nullptr;
    }
}