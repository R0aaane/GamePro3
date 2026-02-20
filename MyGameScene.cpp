#include "MyGameScene.h"
#include "Game.h"
#include "Renderer.h"
#include "SoundSystem.h"
#include <exception>

MyGameScene::MyGameScene(Game* game)
	: Scene(game)
{	
	// spriteの初期化
	m_bg = std::make_unique<SpriteActor>(this, 
		L"src\\pipo-battlebg018b.jpg", Renderer::Shader2DAlphaLoopPoint, 
		XMFLOAT2(230.0f, 240.0f));
	m_sprite = std::make_unique<SpriteActor>(this, 
		L"src\\kabocha.png", Renderer::Shader2DAlphaLoopPoint, 
		XMFLOAT2(100.0f, 200.0f), XMFLOAT2(200.0f,100.0f));
	if (!m_sprite->isEnabled()) throw std::exception();

	m_blockSprite = std::make_unique<BlockSpriteActor>(this, 
		L"src\\kabocha.png", 3, 4, 7, Renderer::Shader2DAlphaLoopPoint,
		XMFLOAT2(500.0f, 200.0f));
	if (!m_blockSprite->isEnabled()) throw std::exception();

	//ブロック内でしか使用しない値を設定
	{

		std::vector<UINT> indicesDown{ 1, 0, 1, 2 };
		std::vector<UINT> indicesLeft{ 4, 3, 4, 5 };
		std::vector<UINT> indicesRight{ 7, 6, 7, 8 };
		std::vector<UINT> indicesUp{ 10, 9, 10, 11 };
		std::vector<std::vector<UINT>> anims{ indicesDown, indicesLeft, 
			indicesRight, indicesUp };
		m_AnimSprite = std::make_unique<BlockAnimActor>(this, 
			L"src\\kabocha.png", anims, 0, 0.2f, 3, 4, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(500.0f, 250.0f));
		if (!m_AnimSprite->isEnabled()) throw std::exception();
	}

	{

		std::vector<UINT> indices{ 0, 1, 2, 3, 4, 5 };
		std::vector<std::vector<UINT>> anims{ indices };
		m_balloon = std::make_unique<PlayerActor2D>(this,
			L"src\\balloon.png", 16.0f, anims, 0, 0.2f, 6, 12, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(500.0f, 250.0f));
		if (!m_balloon->isEnabled()) throw std::exception();
	}

	{

		std::vector<UINT> indices{ 0, 1, 2, 3, 4, 5 };
		std::vector<UINT> indicesExplode{ 30, 31, 32, 33, 34, 35 };
		std::vector<std::vector<UINT>> anims{ indices, indicesExplode };
		m_bomb = std::make_unique<PlayerActor2D>(this,
			L"src\\bomb.png", 16.0f, anims, 1, 0.2f , 6, 10, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(500.0f, 250.0f));
		if (!m_bomb->isEnabled()) throw std::exception();
	}

	/*
	{
		std::vector<UINT> indices{ 1, 0, 1, 2 };
		std::vector<std::vector<UINT>> anims{ indices };
		ItemActor* act = new ItemActor(this,
			L"src\\pipo-hikarimono007.png", 16.0f, anims, 0, 0.2f, 3, 4, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(600.0f, 100.0f));
		if (!act->isEnabled()) throw std::exception();
		m_items.push_back(act);

		act = new ItemActor(this,
			L"src\\pipo-hikarimono007.png", 16.0f, anims, 0, 0.2f, 3, 4, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(80.0f, 200.0f));
		if (!act->isEnabled()) throw std::exception();
		m_items.push_back(act);

		act = new ItemActor(this,
			L"src\\pipo-hikarimono007.png", 16.0f, anims, 0, 0.2f, 3, 4, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(300.0f, 300.0f));
		if (!act->isEnabled()) throw std::exception();
		m_items.push_back(act);
	}
	*/

	m_emitter = std::make_unique<EmitterActor>(this);

	{
		std::vector<FontIndex> findex;
		findex.push_back(FontIndex(L' ', L' ' + 96, 0));
		findex.push_back(FontIndex(L'あ', L'あ' + 190, 96));
		findex.push_back(FontIndex(0x4E00, 0x4E00 + 20950, 96 + 190));
		m_font = FontData(L"src\\PixelMplus12.png", findex, 256, 90, false);
		m_stringNum = std::make_unique<StringActor>(this, L"", m_font,
			24.0f * Ones2d, XMFLOAT2(0.0f, 70.0f), ZeroVec2d,
			0.0f, 0.0f, Ones2d, 1.0f, 20, Renderer::Shader2DAlphaLoopPoint);
		if (!m_stringNum->isEnabled()) throw std::exception();
	}



	SoundSystem* ss = m_game->getSoundSystem();
	if (!ss->loadSoundFile(L"src\\maou_bgm_cyber45.mp3")) throw std::exception();
	ss->setBGMVolume(0.1f);
	ss->setBGM(L"src\\maou_bgm_cyber45.mp3");
	ss->startBGM();

	if (!ss->loadSoundFile(L"src\\balloonBoom.mp3")) throw std::exception();
	ss->setSEVolume(0.1f);

	//一番最後に成功判定をとる
	m_isRunning = true;	
}

MyGameScene::~MyGameScene()
{
	releaseActors(m_items);
	releaseActors(m_itemsTemp);
}

void MyGameScene::update(float deltaTime)
{
	// ゲームの更新処理をここに記述

	//　スプライトの更新
	m_bg->update(deltaTime);
	m_sprite->update(deltaTime);
	m_blockSprite->update(deltaTime);
	m_AnimSprite->update(deltaTime);
	m_balloon->update(deltaTime);
	m_bomb->update(deltaTime);

	updateActors(m_items, deltaTime);

	m_emitter->update(deltaTime);

	static wchar_t strBuff[19];
	wsprintfW(strBuff, L"balloon num: %d", (int)m_items.size());
	m_stringNum->setString(strBuff);
	m_stringNum->update(deltaTime);

	// 削除処理
	removeActors(m_items);

	//　一時退避していたアイテムを追加
	moveActors(m_itemsTemp, m_items);
}

void MyGameScene::draw()
{
	// ゲームの描画処理をここに記述

	//　スプライトの描画
	m_bg->draw();
	m_sprite->draw();
	m_blockSprite->draw();
	m_AnimSprite->draw();
	m_balloon->draw();
	m_bomb->draw();

	drawActors(m_items);

	m_stringNum->draw();	
}