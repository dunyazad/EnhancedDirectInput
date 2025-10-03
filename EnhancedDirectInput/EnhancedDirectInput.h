#pragma once

#include <dinput.h>

extern "C"
{
    /**
     * @brief DirectInput�� �ʱ�ȭ�ϰ� ����� ù ��° ���̽�ƽ�� ã���ϴ�.
     * @param hwnd ���� �������� �ڵ�(HWND)�Դϴ�.
     * @return �ʱ�ȭ ���� �� true, ���� �� false�� ��ȯ�մϴ�.
     */
    __declspec(dllexport) bool InitializeInput(HWND hwnd);

    /**
     * @brief ���� ���̽�ƽ�� ���¸� �о�ɴϴ�.
     * @param outState ���̽�ƽ ���¸� ������ DIJOYSTATE2 ����ü �������Դϴ�.
     * @return ���� �б� ���� �� true, ���� �� false�� ��ȯ�մϴ�.
     */
    __declspec(dllexport) bool ReadJoystick(DIJOYSTATE2* outState);

    /**
     * @brief ��� ���� DirectInput ��ü�� ����̽��� �����մϴ�.
     */
    __declspec(dllexport) void ShutdownInput();
}
