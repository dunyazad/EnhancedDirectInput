#include "pch.h"
#include "EnhancedDirectInput.h"

// ���� ������ DirectInput ��ü�� ����̽��� �����մϴ�.
static LPDIRECTINPUT8       g_pDI = nullptr;
static LPDIRECTINPUTDEVICE8 g_pJoystick = nullptr;

// DllMain �Լ�: DLL�� ���μ����� �ε�ǰų� ��ε�� �� ȣ��˴ϴ�.
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


// ����� ���̽�ƽ ��ġ�� ã�� ���� �ݹ� �Լ��Դϴ�.
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
    // ã�� ù ��° ��ġ�� ����ϱ� ���� ����̽� ��ü�� �����մϴ�.
    HRESULT hr = g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, NULL);

    if (FAILED(hr))
    {
        return DIENUM_CONTINUE; // ���� �� �ٸ� ��ġ�� ��� �˻��մϴ�.
    }

    return DIENUM_STOP; // ��ġ�� ã�����Ƿ� ���Ÿ� �����մϴ�.
}


// --- �𸮾󿡼� ȣ���� �Լ����� ���� ���� ---

bool InitializeInput(HWND hwnd)
{
    if (g_pDI) return true; // �̹� �ʱ�ȭ�Ǿ��ٸ� �ƹ��͵� ���� ����

    HRESULT hr;

    // DirectInput 8 ��ü�� �����մϴ�.
    hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&g_pDI, NULL);
    if (FAILED(hr)) return false;

    // ����� ���� ��Ʈ�ѷ�(���̽�ƽ)�� ã���ϴ�.
    hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
    if (FAILED(hr) || g_pJoystick == nullptr)
    {
        g_pDI->Release();
        g_pDI = nullptr;
        return false;
    }

    // ���̽�ƽ�� ������ ������ �����մϴ�. (DIJOYSTATE2 ���)
    hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2);
    if (FAILED(hr))
    {
        g_pJoystick->Release();
        g_pJoystick = nullptr;
        g_pDI->Release();
        g_pDI = nullptr;
        return false;
    }

    // ���� ������ �����մϴ�. ��׶��忡���� �Է��� �޵��� �մϴ�.
    g_pJoystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);

    // ��ġ�� ���� ������� ȹ���մϴ�.
    g_pJoystick->Acquire();

    return true;
}

bool ReadJoystick(DIJOYSTATE2* outState)
{
    if (g_pJoystick == nullptr) return false;

    HRESULT hr = g_pJoystick->Poll();
    if (FAILED(hr))
    {
        // ������� �Ҿ��� ��� �ٽ� ȹ�� �õ�
        hr = g_pJoystick->Acquire();
        while (hr == DIERR_INPUTLOST)
        {
            hr = g_pJoystick->Acquire();
        }
        if (FAILED(hr)) return false;
    }

    // ��ġ�� ���� ���¸� �����ɴϴ�.
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