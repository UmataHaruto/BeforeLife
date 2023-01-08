#pragma once
//=============================================================================
//
// �T�E���h���� [XAudio2.h]
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

	// �p�����[�^�\����
	typedef struct
	{
		std::string filename;	// �����t�@�C���܂ł̃p�X��ݒ�
		bool bLoop;			// true�Ń��[�v�B�ʏ�BGM��ture�ASE��false�B
	} PARAM;

	//�C���X�^���X�擾
	static SoundMgr& GetInstance() {
		static SoundMgr Instance;
		return Instance;
	}
	//�T�E���h���X�g
	std::vector<PARAM> g_soundlist;

	//��������
	HRESULT XA_Initialize(void);
	//���ʒ���
	HRESULT SetVolume(float Volume, std::string label);

	//�}�X�^�[�{�����[���ݒ�
	void SetMasterVolume(float volume)
	{
		m_master_volume = volume;

		for (int i = 0; i > g_soundlist.size(); i++)
		{
			SetVolume(m_master_volume,g_soundlist[i].filename);
		}
	}

	//�������
	void XA_Release(void);
	//�Đ�
	void XA_Play(std::string label);
	//BGM�Đ�
	void XA_PlayBGM(std::string label);
	//�Đ����~
	void XA_Stop(std::string label);

	//�Đ����~
	void XA_StopAll();

	//�|�[�Y
	void XA_Pause(std::string label);
private:
	float m_master_volume =0;
	IXAudio2* g_pXAudio2 = NULL;
	IXAudio2MasteringVoice* g_pMasteringVoice = NULL;
	IXAudio2SourceVoice* g_pSourceVoice[SOUND_MAX];

	WAVEFORMATEXTENSIBLE	g_wfx[SOUND_MAX];			// WAV�t�H�[�}�b�g
	XAUDIO2_BUFFER			g_buffer[SOUND_MAX];
	BYTE* g_DataBuffer[SOUND_MAX];
};

