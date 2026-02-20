
#pragma once

#include <Windows.h>
#include <xaudio2.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <vector>
#include <string>
#include <unordered_map>
#include "GameUtil.h"
#include "SoundSource.h"

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	bool initialize();
	bool loadSoundFile(std::wstring filePath);

	bool setBGM(std::wstring filePath, bool start = false);
	bool startBGM();
	bool pauseBGM();
	bool stopBGM();

	bool setSoundEffect(std::wstring filePath);

	void update(float deltaTime);
	void setBGMVolume(float volume);
	void setSEVolume(float volume);

private:
	static const UINT SoundEffectMaxNum = 20;

	ComPtr<IXAudio2> m_xaudio2;
	IXAudio2MasteringVoice* m_masteringVoice;
	std::unordered_map<std::wstring, SoundData*> m_soundDataList;

	SoundSource* m_bgm;
	SoundSource* m_soundEffects[SoundEffectMaxNum];
	float m_bgmVolume;
	float m_seVolume;

};




