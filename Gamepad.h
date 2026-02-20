
#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "GameUtil.h"

#pragma comment(lib, "xinput.lib")

class Gamepad
{
public:
	Gamepad(DWORD id);
	~Gamepad();

	void initialize();
	void input();

	bool isPressed(int button) const;
	bool isReleased(int button) const;
	bool isDown(int button) const;
	bool isUp(int button) const;

	int getLeftTrigger() const { return m_leftTrigger; }
	int getRightTrigger() const { return m_rightTrigger; }
	XMFLOAT2 getLeftStick() const { return m_leftStick; }
	XMFLOAT2 getRightStick() const { return m_rightStick; }

	void setVibration(float leftPower, float rightPower, float time);
	void stopVibration();
	void update(float deltaTime);

private:
	static constexpr int ButtonNum = 14;
	static constexpr int ButtonList[ButtonNum] =
	{
		XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y,
		XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
		XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT,
		XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_BACK,
		XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
		XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER
	};
	static constexpr float StickMinLength = 8000.0f;
	static constexpr float StickMaxLength = 30000.0f;

	DWORD m_padID;
	int m_leftTrigger;
	int m_rightTrigger;
	XMFLOAT2 m_leftStick;
	XMFLOAT2 m_rightStick;
	bool m_buttonStates[ButtonNum];
	bool m_buttonOldStates[ButtonNum];
	XINPUT_VIBRATION m_vibration;
	float m_vibrationTime;
	bool  m_isSettingVibration;

	bool buttonIndex(int button, int& index) const;
	XMFLOAT2 calcStickVector(float x, float y, float lenMin, float lenMax) const;

};








