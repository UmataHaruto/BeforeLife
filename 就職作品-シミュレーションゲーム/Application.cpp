//*****************************************************************************
//!	@file	Application.cpp
//!	@brief	
//!	@note	アプリケーションクラス
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <Crtdbg.h>
#include <Windows.h>
#include <chrono>
#include <thread>
//#include <cinttypes>
#include "Application.h"
#include "CWindow.h"
#include "macro.h"
#include "game.h"
#include "myimgui.h"
#include "Title.h"
#include "SceneMgr.h"
#include "ModelMgr.h"
#include "NameGenerator.h"
#include "XAudio2.h"
#include "Sprite2DMgr.h"

//-----------------------------------------------------------------------------
// スタティック　メンバー
//-----------------------------------------------------------------------------
const char* Application::WINDOW_TITLE = "Before Life";
const char* Application::WINDOW_CLASS_NAME = "win32app";

const uint32_t		Application::WINDOW_STYLE_WINDOWED = (WS_VISIBLE | WS_CAPTION | WS_SYSMENU);
const uint32_t		Application::WINDOW_EX_STYLE_WINDOWED = (0);
const uint32_t		Application::WINDOW_STYLE_FULL_SCREEN = (WS_VISIBLE | WS_POPUP);
const uint32_t		Application::WINDOW_EX_STYLE_FULL_SCREEN = (0);

const uint32_t		Application::CLIENT_WIDTH = 1280;
const uint32_t		Application::CLIENT_HEIGHT = 720;

uint32_t			Application::SYSTEM_WIDTH = 0;
uint32_t			Application::SYSTEM_HEIGHT = 0;

const float			Application::FPS = 60;

float               Application::GAME_SPEED = 1.0f;
int                 Application::GAME_TIME = 240;


//==============================================================================
//!	@fn		CApplication
//!	@brief	コンストラクタ
//!	@param	
//!	@retval	
//==============================================================================
Application :: Application(): m_SystemCounter		( 0 )
{}

//==============================================================================
//!	@fn		~Application
//!	@brief	デストラクタ
//!	@param	
//!	@retval	
//==============================================================================
Application :: ~Application()
{
	Dispose();
}

//==============================================================================
//!	@fn		GetInstance
//!	@brief	インスタンス取得
//!	@param	
//!	@retval	インスタンス
//==============================================================================
Application* Application::Instance()
{
	static Application Instance;

	return &Instance;
}

//==============================================================================
//!	@fn		InitSystemWH
//!	@brief	システム領域の幅と高さを初期化
//!	@param	
//!	@retval	
//==============================================================================
void Application :: InitSystemWH()
{

}

void InitThred00()
{
	for (int i = 0; i < 6; i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
		SceneMgr::GetInstance().AddLoadingRatio();
	}
	SceneMgr::GetInstance().SetInitStatus(0, true);
}

void InitThred01()
{
	for (int i = 6; i < 9; i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
		SceneMgr::GetInstance().AddLoadingRatio();

	}
	SceneMgr::GetInstance().SetInitStatus(1, true);
}

void InitThred02()
{
	for (int i = 9; i < 11; i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
		SceneMgr::GetInstance().AddLoadingRatio();
	}
	SceneMgr::GetInstance().SetInitStatus(2, true);
}

void InitThred03()
{
	//名前生成の初期化
	NameGenerator::GetInstance().Init();

	SceneMgr::GetInstance().SetInitStatus(3, true);

}

void InitThred04()
{
	for (int i = 11; i < ModelMgr::GetInstance().g_modellist.size(); i++)
	{
		ModelMgr::GetInstance().LoadModel(
			ModelMgr::GetInstance().g_modellist[i].modelname,
			ModelMgr::GetInstance().g_modellist[i].vsfilename,
			ModelMgr::GetInstance().g_modellist[i].psfilename,
			ModelMgr::GetInstance().g_modellist[i].texfolder);
		SceneMgr::GetInstance().AddLoadingRatio();
	}
	SceneMgr::GetInstance().SetInitStatus(4, true);

}

//==============================================================================
//!	@fn		Init
//!	@brief	初期化
//!	@param	インスタンスハンドル
//!	@retval	
//==============================================================================
bool Application :: Init( HINSTANCE h_Instance )
{
	// メモリーリークを検出
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);

	// コンソールを割り当てる
	AllocConsole();

	// 標準出力の割り当て
	freopen_s(&m_fp, "CON", "w", stdout);    

	// 幅と高さ初期化
	InitSystemWH();

	// ウインドウ作成
	CWindow* window = CWindow::Instance();
	window->RegisterClass( h_Instance, WINDOW_CLASS_NAME, CS_OWNDC );

	window->SetWindow( h_Instance,
					   WINDOW_STYLE_WINDOWED,
					   WINDOW_EX_STYLE_WINDOWED,
					   NULL,
					   CLIENT_WIDTH + SYSTEM_WIDTH,
					   CLIENT_HEIGHT + SYSTEM_HEIGHT,
					   WINDOW_CLASS_NAME,
					   WINDOW_TITLE,
					   false);
			
	// HWND
	m_hWnd = window->GetHandle();

	//マウスカーソル表示OFF
	ShowCursor(FALSE);

	//
	m_hInst = h_Instance;
	return true;
}

//==============================================================================
//!	@fn		Dispose
//!	@brief  終了処理
//!	@param	
//!	@retval	
//==============================================================================
void Application :: Dispose()
{
	// 標準出力クローズ
	fclose(m_fp);
	// コンソール開放
	::FreeConsole();

	return;
}

//==============================================================================
//!	@fn		MainLoop
//!	@brief	メインループ
//!	@param	
//!	@retval	メッセージID
//==============================================================================
unsigned long Application :: MainLoop()
{
	SoundMgr::GetInstance().XA_Initialize();

	MSG		msg;	
	ZeroMemory( &msg, sizeof( msg ) );	

	CWindow* window = CWindow::Instance();

	uint64_t current_time = 0;
	uint64_t last_time = 0;

	// ゲームの初期処理
	TitleInit();

	//初期ロードをマルチスレッドで行う
	std::thread thr1(InitThred00);
	std::thread thr2(InitThred01);
	std::thread thr3(InitThred02);
	std::thread thr4(InitThred03);
	std::thread thr5(InitThred04);

	// タイマ解像度をミリ秒に
	::timeBeginPeriod(1);

	while (window->ExecMessage()) {

		if (SceneMgr::GetInstance().GetEndFlg())
		{
			break;
		}

		// timeGetTime関数は、ミリ秒単位でシステム時刻を取得します。 
		// システム時間は、Windowsを起動してからの経過時間です。
		current_time = ::timeGetTime();

		//ウィンドウのアクティブ情報を取得
		HWND hWnd;
		hWnd = GetActiveWindow();

		//ウィンドウアクティブ時のみゲーム更新
		if (hWnd == CWindow::Instance()->GetHandle()) {
			uint64_t delta_time = current_time - last_time;

			last_time = current_time;

			Update(delta_time);

			switch (SceneMgr::GetInstance().GetSceneNo())
			{
			case SCENETYPE::TITLE:
				TitleUpdate();
				TitleDraw();
			break;

			case SCENETYPE::GAME:
				GameInput(delta_time);		// ｹﾞｰﾑ入力	
				GameUpdate(delta_time);		// ｹﾞｰﾑ更新
				GameRender(delta_time);		// ｹﾞｰﾑ描画
				break;

			default:
				break;
			}

			int64_t sleep_time = 16666 - delta_time;

			if (sleep_time > 0) {
				float tt = sleep_time / 1000.0f;
				//			printf("sleep:%f \n", tt);
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(tt)));
				//指定した相対時間だけ現スレッドをブロックする (function template)
			}
		}
	}

	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();

	// タイマ解像度を元に戻す
	::timeEndPeriod(1);

	// ゲームの終了処理
	GameDispose();

	return window->GetMessage();
}

//==============================================================================
//!	@fn		Input
//!	@brief	キー入力
//!	@param	
//!	@retval	
//==============================================================================
void Application :: Input(uint64_t deltataime)
{
}

//==============================================================================
//!	@fn		Update
//!	@brief	更新
//!	@param	
//!	@retval	
//==============================================================================
void Application :: Update(uint64_t deltataime)
{
	// システムカウンタ
	m_SystemCounter ++;
	static float timecount;

	ImGuiIO& io = ImGui::GetIO();
	//ストップ時は加算しない
	if(GAME_SPEED != 0)
	timecount += io.DeltaTime;

	if (timecount >= (8.3f / GAME_SPEED) / 10)
	{
		timecount = 0;
		GAME_TIME += 1;

		//24時間経過
		if (GAME_TIME >= 1440)
		{
			GAME_TIME = 0;
		}
	}

}

//==============================================================================
//!	@fn		Render
//!	@brief	描画
//!	@param	
//!	@retval	
//==============================================================================
void Application :: Render(uint64_t deltatime)
{
}

//==============================================================================
//!	@fn		GetHWnd
//!	@brief	HWND 取得
//!	@param	
//!	@retval	HWND
//==============================================================================
HWND Application :: GetHWnd()
{
	return m_hWnd;
}

//==============================================================================
//!	@fn		GetHInst
//!	@brief	HINSTANCE 取得
//!	@param	
//!	@retval	HINSTANCE
//==============================================================================
HINSTANCE Application::GetHInst()
{
	return m_hInst;
}

//******************************************************************************
//	End of file.
//******************************************************************************
