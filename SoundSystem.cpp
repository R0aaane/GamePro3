
#include "SoundSystem.h"
#include "fstream"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

struct Chunk
{
	char id[4];
};

bool loadWAVFile(const wchar_t* filePath, WAVEFORMATEX& format,
	std::vector<BYTE>& waveData);
bool loadWAVFileMF(const wchar_t* filePath, WAVEFORMATEX& format,
	std::vector<BYTE>& waveData);

SoundSystem::SoundSystem()
	: m_masteringVoice(nullptr)
	, m_bgm(nullptr)
	, m_soundEffects{nullptr}
	, m_bgmVolume(1.0f)
	, m_seVolume(1.0f)
{
}

SoundSystem::~SoundSystem()
{
	if (m_bgm != nullptr)
	{
		delete m_bgm;
		m_bgm = nullptr;
	}
	for (int i = 0; i < SoundEffectMaxNum; ++i)
	{
		if (m_soundEffects[i] == nullptr) continue;

		delete m_soundEffects[i];
		m_soundEffects[i] = nullptr;
	}

	if (m_masteringVoice != nullptr)
	{
		m_masteringVoice->DestroyVoice();
		m_masteringVoice = nullptr;
	}

	for (auto it = m_soundDataList.begin(); it != m_soundDataList.end(); ++it)
	{
		SoundData* s = it->second;
		delete s;
	}
	m_soundDataList.clear();
}

bool SoundSystem::initialize()
{
	HRESULT hr = XAudio2Create(m_xaudio2.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = m_xaudio2->CreateMasteringVoice(&m_masteringVoice);
	if (FAILED(hr)) return false;

	return true;
}

bool SoundSystem::loadSoundFile(std::wstring filePath)
{
	auto it = m_soundDataList.find(filePath);
	if (it != m_soundDataList.end()) return true;

	size_t pathSize = filePath.size();
	if (pathSize <= 4) return false;
	size_t dotPos = filePath.find_last_of(L".");
	if (dotPos == std::wstring::npos) return false;
	std::wstring ename = filePath.substr(dotPos + 1, pathSize - dotPos - 1);
	SoundData* data = new SoundData();
	if (ename == L"wav")
	{
		if (!loadWAVFile(filePath.c_str(), data->format, data->waveData))
		{
			delete data;
			return false;
		}
	}
	else
	{
		if (!loadWAVFileMF(filePath.c_str(), data->format, data->waveData))
		{
			delete data;
			return false;
		}
	}
	m_soundDataList[filePath] = data;

	return true;
}

bool SoundSystem::setBGM(std::wstring filePath, bool start)
{
	if (m_bgm)
	{
		delete m_bgm;
		m_bgm = nullptr;
	}

	if (!loadSoundFile(filePath)) return false;
	SoundData* data = m_soundDataList[filePath];
	m_bgm = new SoundSource(m_xaudio2.Get(), data, start, 
		XAUDIO2_LOOP_INFINITE, m_bgmVolume);

	return true;
}

bool SoundSystem::startBGM()
{
	if (m_bgm == nullptr) return false;

	m_bgm->start();
	return true;
}

bool SoundSystem::pauseBGM()
{
	if (m_bgm == nullptr) return false;

	m_bgm->pause();
	return false;
}

bool SoundSystem::stopBGM()
{
	if (m_bgm == nullptr) return false;

	delete m_bgm;
	m_bgm = nullptr;
	return true;
}

bool SoundSystem::setSoundEffect(std::wstring filePath)
{
	if (!loadSoundFile(filePath)) return false;

	for (int i = 0; i < SoundEffectMaxNum; ++i)
	{
		if (m_soundEffects[i] != nullptr) continue;

		SoundData* data = m_soundDataList[filePath];
		m_soundEffects[i] = 
			new SoundSource(m_xaudio2.Get(), data, true, 0, m_seVolume);
		return true;
	}

	return false;
}

void SoundSystem::update(float deltaTime)
{
	for (int i = 0; i < SoundEffectMaxNum; ++i)
	{
		if (m_soundEffects[i] == nullptr) continue;

		m_soundEffects[i]->update(deltaTime);
		if (m_soundEffects[i]->getState() == SoundSource::SoundState::Stop)
		{
			delete m_soundEffects[i];
			m_soundEffects[i] = nullptr;
		}
	}
}

void SoundSystem::setBGMVolume(float volume)
{
	m_bgmVolume = volume;
	if (m_bgm == nullptr) return;

	m_bgm->setVolume(m_bgmVolume);
}

void SoundSystem::setSEVolume(float volume)
{
	m_seVolume = volume;
	for (int i = 0; i < SoundEffectMaxNum; ++i)
	{
		if (m_soundEffects[i] == nullptr) continue;
		m_soundEffects[i]->setVolume(m_seVolume);
	}
}

bool loadWAVFile(const wchar_t* filePath, WAVEFORMATEX& format,
	std::vector<BYTE>& waveData)
{
	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin) return false;

	Chunk criff;
	int32_t riffSize;
	Chunk cwave;
	Chunk cformat;
	int32_t formatSize;

	fin.read((char*)&criff, sizeof(Chunk));
	fin.read((char*)&riffSize, sizeof(int32_t));
	fin.read((char*)&cwave, sizeof(Chunk));
	fin.read((char*)&cformat, sizeof(Chunk));
	fin.read((char*)&formatSize, sizeof(int32_t));

	if (formatSize == 14)
	{
		WAVEFORMAT wformat = {};
		fin.read((char*)&wformat, sizeof(WAVEFORMAT));
		memcpy((void*)&format, (void*)&wformat, sizeof(WAVEFORMAT));
		format.wBitsPerSample = (8 * wformat.nBlockAlign) / wformat.nChannels;
		format.cbSize = 0;
	}
	else if (formatSize == 16)
	{
		PCMWAVEFORMAT wformat = {};
		fin.read((char*)&wformat, sizeof(PCMWAVEFORMAT));
		memcpy((void*)&format, (void*)&wformat, sizeof(PCMWAVEFORMAT));
		format.cbSize = 0;
	}
	else if (formatSize == 18)
	{
		fin.read((char*)&format, sizeof(WAVEFORMATEX));
		char buf[2];
		fin.read(buf, 2);
	}
	else
	{
		return false;
	}

	Chunk cdata;
	int32_t dataSize;
	fin.read((char*)&cdata, sizeof(Chunk));
	fin.read((char*)&dataSize, sizeof(int32_t));

	waveData.resize(dataSize);
	fin.read((char*)waveData.data(), dataSize);

	return true;
}

bool loadWAVFileMF(const wchar_t* filePath, WAVEFORMATEX& format,
	std::vector<BYTE>& waveData)
{
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	ComPtr<IMFSourceReader> reader;
	HRESULT hr = MFCreateSourceReaderFromURL(filePath, nullptr, reader.GetAddressOf());
	if (FAILED(hr))
	{
		MFShutdown();
		return false;
	}

	ComPtr<IMFMediaType> mediaType;
	MFCreateMediaType(mediaType.GetAddressOf());
	mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		nullptr, mediaType.Get());
	reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		mediaType.GetAddressOf());

	WAVEFORMATEX* pformat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(mediaType.Get(), &pformat, nullptr);
	while (true)
	{
		ComPtr<IMFSample> sample;
		DWORD streamFlags = 0;
		reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr,
			&streamFlags, nullptr, sample.GetAddressOf());
		if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) break;
		ComPtr<IMFMediaBuffer> mediaBuffer;
		sample->ConvertToContiguousBuffer(mediaBuffer.GetAddressOf());

		BYTE* buffer = nullptr;
		DWORD length = 0;
		mediaBuffer->Lock(&buffer, nullptr, &length);
		waveData.resize(waveData.size() + length);
		memcpy(waveData.data() + waveData.size() - length, buffer, length);
		mediaBuffer->Unlock();
	}

	memcpy((void*)&format, (void*)pformat, sizeof(WAVEFORMATEX));
	CoTaskMemFree(pformat);
	MFShutdown();

	return true;
}
