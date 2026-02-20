#include "MyGameScene.h"
#include "Game.h"
#include "Renderer.h"
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
			L"src\\balloon.png", anims, 0, 0.2f, 6, 12, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(500.0f, 250.0f));
		if (!m_balloon->isEnabled()) throw std::exception();
	}


	{

		std::vector<UINT> indices{ 0, 1, 2, 3, 4, 5 };
		std::vector<UINT> indicesExplode{ 30, 31, 32, 33, 34, 35 };
		std::vector<std::vector<UINT>> anims{ indices, indicesExplode };
		m_bomb = std::make_unique<PlayerActor2D>(this,
			L"src\\bomb.png", anims, 1, 0.2f, 6, 10, Renderer::Shader2DAlphaLoopPoint,
			XMFLOAT2(500.0f, 250.0f));
		if (!m_bomb->isEnabled()) throw std::exception();
	}

	//一番最後に成功判定をとる
	m_isRunning = true;	
}

MyGameScene::~MyGameScene()
{


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
}