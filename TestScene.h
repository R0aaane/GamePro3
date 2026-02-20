
#pragma once

#include "Scene.h"
#include "Actor.h"
#include "ColorAnimActor.h"
#include "SpriteActor.h"
#include "BlockSpriteActor.h"
#include "BlockAnimActor.h"
#include "CameraActor.h"
#include "TrackingCameraActor.h"
#include "FPVCameraActor.h"
#include "TPVCameraActor.h"
#include "PlayerActor.h"
#include "PlaneActor.h"
#include "CubeActor.h"
#include "HemisphereActor.h"
#include "SphereActor.h"
#include "FileMeshActor.h"
#include "BallActor.h"
#include "TerrainActor.h"
#include "StringActor.h"
#include "StarSystemActor.h"
#include "RoboActor.h"
#include "FileAnimMeshActor.h"

class TestScene : public Scene
{
public:
	TestScene(class Game* game);
	~TestScene();

	void update(float deltaTime) override;
	void draw() override;

	TerrainActor* getTerrain() { return m_terrain.get(); }
	std::vector<Actor*>& getObjects() { return m_objects; }

	void deleteBlock(Ray ray);
	void createBlock(Ray ray);

private:
	std::unique_ptr<BlockAnimActor> m_UI;
	std::unique_ptr<FPVCameraActor> m_fpvCamera;
	std::unique_ptr<TPVCameraActor> m_tpvCamera;
	std::unique_ptr<PlayerActor> m_player;
	std::unique_ptr<FileMeshActor> m_chara;
	std::unique_ptr<FileMeshActor> m_test;
	std::unique_ptr<FileMeshActor> m_dragon;
	std::unique_ptr<FileAnimMeshActor> m_charaAnim;
	bool m_tpvMode;

	std::unique_ptr<TerrainActor>    m_terrain;
	std::unique_ptr<CubeActor>       m_skybox;
	std::unique_ptr<HemisphereActor> m_skydome;

	std::unique_ptr<StarSystemActor> m_stars;
	std::unique_ptr<RoboActor> m_robo;
	std::vector<Actor*> m_trees;
	std::vector<Actor*> m_persons;

	std::vector<Actor*> m_objects;
	static const int BlockXNum = 10;
	static const int BlockYNum = 10;
	static const int BlockZNum = 10;
	static const int BlockTextureNum = 9;
	float m_blockWidth;
	XMFLOAT3 m_blockBasePos;
	const wchar_t* m_blockTexNames[BlockTextureNum];
	int m_blockTexIndex;

	std::unique_ptr<StringActor> m_strBlockName;
	FontData m_font;

	int m_pointLights[4];

};






















