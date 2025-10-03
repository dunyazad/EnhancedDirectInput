#pragma once

#include <dinput.h>

extern "C"
{
    /**
     * @brief DirectInput을 초기화하고 연결된 첫 번째 조이스틱을 찾습니다.
     * @param hwnd 게임 윈도우의 핸들(HWND)입니다.
     * @return 초기화 성공 시 true, 실패 시 false를 반환합니다.
     */
    __declspec(dllexport) bool InitializeInput(HWND hwnd);

    /**
     * @brief 현재 조이스틱의 상태를 읽어옵니다.
     * @param outState 조이스틱 상태를 저장할 DIJOYSTATE2 구조체 포인터입니다.
     * @return 상태 읽기 성공 시 true, 실패 시 false를 반환합니다.
     */
    __declspec(dllexport) bool ReadJoystick(DIJOYSTATE2* outState);

    /**
     * @brief 사용 중인 DirectInput 객체와 디바이스를 해제합니다.
     */
    __declspec(dllexport) void ShutdownInput();
}
