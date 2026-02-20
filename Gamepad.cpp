
#include "Gamepad.h"

Gamepad::Gamepad(DWORD id)
	: m_padID(id)
	, m_leftTrigger(0)
	, m_rightTrigger(0)
	, m_leftStick(ZeroVec2d)
	, m_rightStick(ZeroVec2d)
	, m_vibration{ 0, 0 }
	, m_vibrationTime(0.0f)
	, m_isSettingVibration(false)
{
	initialize();
}

Gamepad::~Gamepad()
{
	m_vibration = { 0, 0 };
	XInputSetState(m_padID, &m_vibration);
}

void Gamepad::initialize()
{
	memset(m_buttonStates, false, sizeof(bool) * ButtonNum);
	memset(m_buttonOldStates, false, sizeof(bool) * ButtonNum);
	m_leftTrigger = 0;
	m_rightTrigger = 0;
	m_leftStick = ZeroVec2d;
	m_rightStick = ZeroVec2d;
}

void Gamepad::input()
{
	XINPUT_STATE state = {};
	DWORD result = XInputGetState(m_padID, &state);
	if (result != ERROR_SUCCESS)
	{
		initialize();
		return;
	}

	memcpy(m_buttonOldStates, m_buttonStates, sizeof(bool) * ButtonNum);
	for (int i = 0; i < ButtonNum; ++i)
	{
		m_buttonStates[i] = state.Gamepad.wButtons & ButtonList[i];
	}
	m_leftTrigger = (int)state.Gamepad.bLeftTrigger;
	m_rightTrigger = (int)state.Gamepad.bRightTrigger;

	m_leftStick = calcStickVector((float)state.Gamepad.sThumbLX,
		-1.0f * (float)state.Gamepad.sThumbLY, StickMinLength, StickMaxLength);
	m_rightStick = calcStickVector((float)state.Gamepad.sThumbRX,
		-1.0f * (float)state.Gamepad.sThumbRY, StickMinLength, StickMaxLength);
}

bool Gamepad::isPressed(int button) const
{
	int index = 0;
	if (!buttonIndex(button, index)) return false;

	return m_buttonStates[index] && !m_buttonOldStates[index];
}

bool Gamepad::isReleased(int button) const
{
	int index = 0;
	if (!buttonIndex(button, index)) return false;

	return !m_buttonStates[index] && m_buttonOldStates[index];
}

bool Gamepad::isDown(int button) const
{
	int index = 0;
	if (!buttonIndex(button, index)) return false;

	return m_buttonStates[index];
}

bool Gamepad::isUp(int button) const
{
	int index = 0;
	if (!buttonIndex(button, index)) return false;

	return !m_buttonStates[index];
}

bool Gamepad::buttonIndex(int button, int& index) const
{
	for (index = 0; index < ButtonNum; ++index)
	{
		if (button == ButtonList[index]) return true;
	}
	return false;
}

XMFLOAT2 Gamepad::calcStickVector(float x, float y, float lenMin, float lenMax) const
{
	XMFLOAT2 s(x, y);
	float len = length(s);
	if (len <= lenMin) return ZeroVec2d;

	len = (len >= lenMax) ? lenMax : len;
	return normalize(s) * ((len - lenMin) / (lenMax - lenMin));
}

void Gamepad::setVibration(float leftPower, float rightPower, float time)
{
	leftPower = (leftPower > 1.0f) ? 1.0f : leftPower;
	leftPower = (leftPower < 0.0f) ? 0.0f : leftPower;
	rightPower = (rightPower > 1.0f) ? 1.0f : rightPower;
	rightPower = (rightPower < 0.0f) ? 0.0f : rightPower;

	m_isSettingVibration = true;
	m_vibration.wLeftMotorSpeed = (WORD)(leftPower * 65535.0f);
	m_vibration.wRightMotorSpeed = (WORD)(rightPower * 65535.0f);
	m_vibrationTime = time;
}

void Gamepad::stopVibration()
{
	setVibration(0.0f, 0.0f, 0.0f);
}

void Gamepad::update(float deltaTime)
{
	if (m_vibrationTime > 0.0f && !m_isSettingVibration)
	{
		m_vibrationTime -= deltaTime;
		if (m_vibrationTime < 0.0f)
		{
			m_vibration = { 0, 0 };
			m_vibrationTime = 0.0f;
			XInputSetState(m_padID, &m_vibration);
		}
	}

	if (m_isSettingVibration)
	{
		m_isSettingVibration = false;
		XInputSetState(m_padID, &m_vibration);
	}
}

