//=============================================================================
//
// �T�E���h���� [XAudio2.cpp]
//
//=============================================================================

#include "XAudio2.h"
#include <string>

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif


//-----------------------------------------------------------------
//    �O���[�o���ϐ�
//-----------------------------------------------------------------


//-----------------------------------------------------------------
//    �v���g�^�C�v�錾
//-----------------------------------------------------------------
HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
HRESULT ReadChunkData( HANDLE , void* , DWORD , DWORD );
std::vector<std::string> getFileName(std::string dir_name);
//=============================================================================
// ������
//=============================================================================
HRESULT SoundMgr::XA_Initialize()
{
	HRESULT      hr;
	
	HANDLE               hFile;
	DWORD                dwChunkSize;
	DWORD                dwChunkPosition;
	DWORD                filetype;
		
	// �b�n�l�̏�����
	CoInitializeEx( NULL , COINIT_MULTITHREADED );

	std::vector<std::string>bgm = getFileName(std::string("assets/sound/BGM/"));
	for (int i = 0; i < bgm.size(); i++)
	{
		PARAM param;
		param.bLoop = true;
		std::string filepath = "assets/sound/BGM/";
		param.filename = filepath + bgm[i];
		g_soundlist.push_back(param);
	}
	std::vector<std::string>se = getFileName(std::string("assets/sound/SE/"));
	for (int i = 0; i < se.size(); i++)
	{
		PARAM param;
		param.bLoop = false;
		std::string filepath = "assets/sound/SE/";
		param.filename = filepath + se[i];
		g_soundlist.push_back(param);
	}
	/**** Create XAudio2 ****/
	hr = XAudio2Create( &g_pXAudio2, 0);		// �������ͤ����t���O �f�o�b�O���[�h�̎w��(���݂͖��g�p�Ȃ̂�0�ɂ���)
	//hr=XAudio2Create(&g_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);		// ��O�����́Awindows�ł͖���
	if( FAILED( hr ) ){
		CoUninitialize();
		return -1;
	}
	
	/**** Create Mastering Voice ****/
	hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice);			// ����͂o�b�̃f�t�H���g�ݒ�ɔC���Ă���
	/*, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL*/		// �{���U�̈����������Ă���
	if( FAILED( hr ) ){
		if( g_pXAudio2 )	g_pXAudio2->Release();
		CoUninitialize();
		return -1;
	}

	/**** Initalize Sound ****/
	for(int i=0; i<g_soundlist.size(); i++)
	{
		memset( &g_wfx[i] , 0 , sizeof( WAVEFORMATEXTENSIBLE ) );
		memset( &g_buffer[i] , 0 , sizeof( XAUDIO2_BUFFER ) );
	
		hFile = CreateFileA( g_soundlist[i].filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING , 0 , NULL );
		if ( hFile == INVALID_HANDLE_VALUE ){
			return HRESULT_FROM_WIN32( GetLastError() );
		}
		if ( SetFilePointer( hFile , 0 , NULL , FILE_BEGIN ) == INVALID_SET_FILE_POINTER ){
			return HRESULT_FROM_WIN32( GetLastError() );
		}

		//check the file type, should be fourccWAVE or 'XWMA'
		FindChunk(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
		ReadChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);
		if (filetype != fourccWAVE)		return S_FALSE;

		FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
		ReadChunkData(hFile, &g_wfx[i], dwChunkSize, dwChunkPosition );

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
		g_DataBuffer[i] = new BYTE[dwChunkSize];
		ReadChunkData(hFile, g_DataBuffer[i], dwChunkSize, dwChunkPosition);
		
		CloseHandle(hFile);

		// 	�T�u�~�b�g�{�C�X�ŗ��p����T�u�~�b�g�o�b�t�@�̐ݒ�
		g_buffer[i].AudioBytes = dwChunkSize;
		g_buffer[i].pAudioData = g_DataBuffer[i];
		g_buffer[i].Flags = XAUDIO2_END_OF_STREAM;
		if(g_soundlist[i].bLoop)
			g_buffer[i].LoopCount = XAUDIO2_LOOP_INFINITE;
		else
			g_buffer[i].LoopCount = 0;

		g_pXAudio2->CreateSourceVoice( &g_pSourceVoice[i] , &(g_wfx[i].Format) );
	}

	return hr;
}

//���ʒ���
HRESULT SoundMgr::SetVolume(float Volume, std::string label)
{
	int index = -1;
	//�t�@�C���ԍ�������
	for (int i = 0; g_soundlist.size(); i++)
	{
		if (g_soundlist[i].filename == label)
		{
			index = i;
			break;
		}
	}

	if (index != -1) {
		g_pSourceVoice[index]->SetVolume(0.1f);
	}
	return E_NOTIMPL;
}

//=============================================================================
// �J������
//=============================================================================
void SoundMgr::XA_Release(void)
{
	for(int i=0; i<g_soundlist.size(); i++)
	{
		if(g_pSourceVoice[i])
		{
			g_pSourceVoice[i]->Stop( 0 );
			g_pSourceVoice[i]->FlushSourceBuffers();
			g_pSourceVoice[i]->DestroyVoice();			// �I�[�f�B�I�O���t����\�[�X�{�C�X���폜
			delete[]  g_DataBuffer[i];
		}
	}

	g_pMasteringVoice->DestroyVoice();
	
	if ( g_pXAudio2 ) g_pXAudio2->Release();
	
	// �b�n�l�̔j��
	CoUninitialize();
}

//=============================================================================
// �Đ�
//=============================================================================
void SoundMgr::XA_Play(std::string label)
{
	int index = -1;
	//�t�@�C���ԍ�������
	for (int i = 0; g_soundlist.size(); i++)
	{
		if (g_soundlist[i].filename == label)
		{
			index = i;
			break;
		}
	}

	if (index != -1 ) {

		// �\�[�X�{�C�X�쐬
		g_pXAudio2->CreateSourceVoice(&(g_pSourceVoice[index]), &(g_wfx[index].Format));
		g_pSourceVoice[index]->SubmitSourceBuffer(&(g_buffer[index]));	// �{�C�X�L���[�ɐV�����I�[�f�B�I�o�b�t�@�[��ǉ�

		// �Đ�
		g_pSourceVoice[index]->Start(0);
	}
}

void SoundMgr::XA_PlayBGM(std::string label)
{
	int index = -1;
	//�t�@�C���ԍ�������
	for (int i = 0; g_soundlist.size(); i++)
	{
		if (g_soundlist[i].filename == label)
		{
			index = i;
			break;
		}
	}

	if (index != -1) {

		// �\�[�X�{�C�X�쐬
		g_pXAudio2->CreateSourceVoice(&(g_pSourceVoice[index]), &(g_wfx[index].Format));
		g_pSourceVoice[index]->SubmitSourceBuffer(&(g_buffer[index]));	// �{�C�X�L���[�ɐV�����I�[�f�B�I�o�b�t�@�[��ǉ�

		// �Đ�
		g_pSourceVoice[index]->Start(0);
	}
}

//=============================================================================
// ��~
//=============================================================================
void SoundMgr::XA_Stop(std::string label)
{
	int index = -1;
	//�t�@�C���ԍ�������
	for (int i = 0;i < g_soundlist.size(); i++)
	{
		if (g_soundlist[i].filename == label)
		{
			index = i;
			break;
		}
	}
	if(g_pSourceVoice[index] == NULL) return;

	if (index != -1) {
		XAUDIO2_VOICE_STATE xa2state;
		g_pSourceVoice[index]->GetState(&xa2state);
		if (xa2state.BuffersQueued)
		{
			g_pSourceVoice[index]->Stop(0);
		}
	}
}

void SoundMgr::XA_StopAll()
{
	for (int i = 0;i < SOUND_MAX; i++)
	{
		if (g_pSourceVoice[i] != nullptr)
		{
			g_pSourceVoice[i]->Stop(0);
		}
	}
}

//=============================================================================
// �ꎞ��~
//=============================================================================
void SoundMgr::XA_Pause(std::string label)
{
	// ���ɂ���Ƃ���Ή\�B
}
//=============================================================================
// ���[�e�B���e�B�֐��Q
//=============================================================================
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;
	while (hr == S_OK)
	{
		DWORD dwRead;
		if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
			hr = HRESULT_FROM_WIN32( GetLastError() );
		if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
			hr = HRESULT_FROM_WIN32( GetLastError() );
		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
				hr = HRESULT_FROM_WIN32( GetLastError() );
			break;
		default:
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
				return HRESULT_FROM_WIN32( GetLastError() );
		}
		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}
		dwOffset += dwChunkDataSize;
		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}
	return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );
	DWORD dwRead;
	if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
		hr = HRESULT_FROM_WIN32( GetLastError() );
	return hr;
}

std::vector<std::string> getFileName(std::string dir_name) {

	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	std::vector<std::string> file_names;

	// png,jpg,bmp�̊g���q�̃t�@�C���݂̂�ǂݍ���
	std::string extension = { "wav" };

	std::string search_name = dir_name + "*." + extension;
	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	do {
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		}
		else {
			file_names.push_back(win32fd.cFileName);
		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	return file_names;
}
