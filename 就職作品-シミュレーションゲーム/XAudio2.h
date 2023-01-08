#pragma once
//=============================================================================
//
// サウンド処理 [XAudio2.h]
//
//=============================================================================
#define _XAUDIO2_H_

#include <xaudio2.h>
#include <string>
#include <vector>

#define SOUND_MAX 50

class SoundMgr
{
public:

	// パラメータ構造体
	typedef struct
	{
		std::string filename;	// 音声ファイルまでのパスを設定
		bool bLoop;			// trueでループ。通常BGMはture、SEはfalse。
	} PARAM;

	//インスタンス取得
	static SoundMgr& GetInstance() {
		static SoundMgr Instance;
		return Instance;
	}
	//サウンドリスト
	std::vector<PARAM> g_soundlist;

	//初期処理
	HRESULT XA_Initialize(void);
	//音量調整
	HRESULT SetVolume(float Volume, std::string label);

	//マスターボリューム設定
	void SetMasterVolume(float volume)
	{
		m_master_volume = volume;

		for (int i = 0; i > g_soundlist.size(); i++)
		{
			SetVolume(m_master_volume,g_soundlist[i].filename);
		}
	}

	//解放処理
	void XA_Release(void);
	//再生
	void XA_Play(std::string label);
	//BGM再生
	void XA_PlayBGM(std::string label);
	//再生中止
	void XA_Stop(std::string label);

	//再生中止
	void XA_StopAll();

	//ポーズ
	void XA_Pause(std::string label);
private:
	float m_master_volume =0;
	IXAudio2* g_pXAudio2 = NULL;
	IXAudio2MasteringVoice* g_pMasteringVoice = NULL;
	IXAudio2SourceVoice* g_pSourceVoice[SOUND_MAX];

	WAVEFORMATEXTENSIBLE	g_wfx[SOUND_MAX];			// WAVフォーマット
	XAUDIO2_BUFFER			g_buffer[SOUND_MAX];
	BYTE* g_DataBuffer[SOUND_MAX];
};

