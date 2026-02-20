
#include "TestScene.h"
#include "Game.h"
#include <exception>
#include "Renderer.h"
#include "ContactableActor.h"
#include "EffectActor.h"
#include "BillboardActor.h"
#include "BoxPersonActor.h"

TestScene::TestScene(Game* game)
	: Scene(game)
	, m_tpvMode(true)
	, m_blockWidth(1.0f)
	, m_blockBasePos(-0.5f * m_blockWidth * BlockXNum, -0.5f * m_blockWidth * BlockYNum, 0.0f)
	, m_objects(BlockXNum * BlockYNum * BlockZNum)
	, m_blockTexIndex(0)
	, m_blockTexNames{ Renderer::GrayTexture, Renderer::BlackTexture,
		Renderer::WhiteTexture, Renderer::RedTexture, Renderer::GreenTexture,
		Renderer::BlueTexture, Renderer::YellowTexture, Renderer::CyanTexture,
		Renderer::MagentaTexture }
{
	{
		// 平行光源
		XMFLOAT3 vec = normalize(XMFLOAT3(0.0f, -1.0f, -1.0f));
		XMFLOAT3 col = Ones3d;							// 昼
		//XMFLOAT3 col = XMFLOAT3(0.93f, 0.36f, 0.06f);	// 夕
		//XMFLOAT3 col = XMFLOAT3(0.06f, 0.07f, 0.13f);	// 夜
		m_game->getRenderer()->setParallelLight(ParallelLight(vec, col));

		// ポイントライト
		m_pointLights[0] = m_game->getRenderer()->allocatePointLight(
			PointLight(XMFLOAT3(8.0f, 0.0f, 5.0f), 0.0f * Ones3d, 10.0f));
		if (m_pointLights[0] == -1) throw std::exception();

		m_pointLights[1] = m_game->getRenderer()->allocatePointLight(
			PointLight(XMFLOAT3(8.0f, -10.0f, 5.0f), 
				0.0f * XMFLOAT3(10.0f, 1.0f, 1.0f), 10.0f));
		if (m_pointLights[1] == -1) throw std::exception();

		m_pointLights[2] = m_game->getRenderer()->allocatePointLight(
			PointLight(XMFLOAT3(15.0f, -10.0f, 5.0f),
				0.0f * XMFLOAT3(1.0f, 6.0f, 1.0f), 10.0f));
		if (m_pointLights[2] == -1) throw std::exception();

		m_pointLights[3] = m_game->getRenderer()->allocatePointLight(
			PointLight(XMFLOAT3(22.0f, -10.0f, 5.0f),
				0.0f * XMFLOAT3(1.0f, 1.0f, 10.0f), 10.0f));
		if (m_pointLights[3] == -1) throw std::exception();

		// 環境ライト
		m_game->getRenderer()->setAmbientLight(0.8f * XMFLOAT3(0.8f, 0.8f, 0.9f));
	}

	{
		std::vector<UINT> indices{ 1, 0, 1, 2 };
		std::vector<std::vector<UINT>> anims{ indices };
		m_UI = std::make_unique<BlockAnimActor>(this, L"src\\kabocha.png",
			anims, 0, 0.5f, 3, 4, Renderer::Shader2DAlphaLoopPoint, XMFLOAT2(30.0f, 40.0f));
		if (!m_UI->isEnabled()) throw std::exception();
	}

	{
		m_player = std::make_unique<PlayerActor>(this,
			Renderer::Shader3DClampAnimLitSW, 0.85f, L"src\\chara.ganim",
			XMFLOAT3(-30.0f, 0.0f, 0.0f));
		if (!m_player->isEnabled()) throw std::exception();
		m_player->setMass(5.0f);
		m_player->setDragCoefficient(20.0f);
		m_player->setMaxSpeed(15.0f);
		m_player->setInstanceNum(2);
		m_player->setShadowMatrix(Plane(UnitVecZ3d, -0.005f),
			m_game->getRenderer()->getParallelLight().direction);
	}

	m_chara = std::make_unique<FileMeshActor>(this, Renderer::Shader3DClampLinearLitSW,
		L"src\\charaPose.gmesh", XMFLOAT3(0.0f, -20.0f, 0.0f));
	if (!m_chara->isEnabled()) throw std::exception();
	m_chara->setOriginalMaterial(MeshMaterial(Ones3d, 0.1f * Ones3d, 0.2f,
		0.4f * Ones3d));
	m_chara->setInstanceNum(2);
	m_chara->setShadowMatrix(Plane(UnitVecZ3d, -0.005f),
		m_game->getRenderer()->getParallelLight().direction);

	m_test = std::make_unique<FileMeshActor>(this, Renderer::Shader3DClampLinearLitSW,
		L"src\\testAdjust.gmesh", XMFLOAT3(-5.0f, -20.0f, 3.0f));
	if (!m_test->isEnabled()) throw std::exception();
	m_test->setOriginalMaterial(MeshMaterial(Ones3d, 0.1f * Ones3d, 0.2f,
		0.4f * Ones3d));
	m_test->setInstanceNum(2);
	m_test->setShadowMatrix(Plane(UnitVecZ3d, -0.005f),
		m_game->getRenderer()->getParallelLight().direction);

	m_dragon = std::make_unique<FileMeshActor>(this, Renderer::Shader3DClampLinearLitSW,
		L"src\\DragonAdjust.gmesh", XMFLOAT3(10.0f, -20.0f, 0.0f));
	if (!m_dragon->isEnabled()) throw std::exception();
	m_dragon->setOriginalMaterial(MeshMaterial(Ones3d, 0.1f * Ones3d, 0.2f,
		0.4f * Ones3d));
	m_dragon->setInstanceNum(2);
	m_dragon->setShadowMatrix(Plane(UnitVecZ3d, -0.005f),
		m_game->getRenderer()->getParallelLight().direction);

	m_charaAnim = std::make_unique<FileAnimMeshActor>(this,
		Renderer::Shader3DClampAnimLitSW,
		L"src\\chara.ganim", XMFLOAT3(-20.0f, 0.0f, 0.0f), ZeroVec3d,
		XMFLOAT3(0.0f, 0.0f, 45.0f));
	if (!m_charaAnim->isEnabled()) throw std::exception();
	m_charaAnim->setOriginalMaterial(MeshMaterial(Ones3d, 0.1f * Ones3d, 0.2f,
		0.4f * Ones3d));
	m_charaAnim->setInstanceNum(2);
	m_charaAnim->setShadowMatrix(Plane(UnitVecZ3d, -0.005f),
		m_game->getRenderer()->getParallelLight().direction);
	
	m_fpvCamera = std::make_unique<FPVCameraActor>(this,
		Camera(CamInParam(90.0f, 0.01f, 10000.0f),
			CamExtParam(ZeroVec3d, UnitVecX3d, UnitVecZ3d)), m_player.get(),
		XMFLOAT3(0.0f, 0.0f, 0.68f), 0.0f, 45.0f);

	m_tpvCamera = std::make_unique<TPVCameraActor>(this,
		Camera(CamInParam(90.0f, 0.01f, 10000.0f),
			CamExtParam(ZeroVec3d, UnitVecX3d, UnitVecZ3d)), m_player.get(),
		90.0f, 30.0f, 3.0f, 1.65f);

	if (m_tpvMode)
	{
		m_tpvCamera->uploadParam();
	}
	else
	{
		m_fpvCamera->uploadParam();
	}

	{
		float xwidth = 5000.0f, ywidth = 5000.0f;
		int xnum = 250, ynum = 250;
		std::vector<float> height(xnum * ynum);
		for (int i = 0; i < ynum; ++i)
		{
			for (int j = 0; j < xnum; ++j)
			{
				height[j + i * xnum] = 0.0f;
			}
		}
		GridHeightMap map((float*)height.data(), xnum, ynum,
			XMFLOAT2(-0.5f * xwidth, -0.5f * ywidth), xwidth, ywidth);

		m_terrain = std::make_unique<TerrainActor>(this, Renderer::Shader3DLoopLinearLit,
			L"src\\grass_loop.png", L"terrain", map, ZeroVec3d, ZeroVec3d, ZeroVec3d, ZeroVec3d,
			XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(300.0f, 300.0f));
		if (!m_terrain->isEnabled()) throw std::exception();
		m_terrain->setOriginalMaterial(MeshMaterial(Ones3d, 0.01f * Ones3d, 0.01f, ZeroVec3d));
	}

	{
		std::vector<const wchar_t*> texturePath(6);
		texturePath[0] = L"src\\skybox_x_p.png";
		texturePath[1] = L"src\\skybox_y_p.png";
		texturePath[2] = L"src\\skybox_x_n.png";
		texturePath[3] = L"src\\skybox_y_n.png";
		texturePath[4] = L"src\\skybox_z_p.png";
		texturePath[5] = L"src\\skybox_z_n.png";
		m_skybox = std::make_unique<CubeActor>(this, Renderer::Shader3DClampLinearTwoSided,
			texturePath, false, XMFLOAT3(0.0f, 0.0f, -500.0f), ZeroVec3d, ZeroVec3d,
			ZeroVec3d, XMFLOAT3(5000.0f, 5000.0f, 5000.0f));
		if (!m_skybox->isEnabled()) throw std::exception();
	}

	m_skydome = std::make_unique<HemisphereActor>(this, Renderer::Shader3DLoopLinearTwoSided,
		L"src\\skydome.png", false, XMFLOAT3(0.0f, 0.0f, -500.0f), ZeroVec3d, ZeroVec3d,
		ZeroVec3d, XMFLOAT3(5000.0f, 5000.0f, 5000.0f), 
		XMFLOAT2(0.6f, 0.6f), XMFLOAT2(0.2f, 0.2f));
	if (!m_skydome->isEnabled()) throw std::exception();

	m_stars = std::make_unique<StarSystemActor>(this, XMFLOAT3(0.0f, 20.0f, 4.0f),
		XMFLOAT3(0.0f, 20.0f, 0.0f));

	m_robo = std::make_unique<RoboActor>(this, XMFLOAT3(0.0f, -10.0f, 0.5f),
		XMFLOAT3(0.0f, 0.0f, -30.0f));

	{
		BoxPersonActor* bp = new BoxPersonActor(this, XMFLOAT3(-20.0f, 5.0f, 0.0f));
		m_persons.push_back(bp);
		bp = new BoxPersonActor(this, XMFLOAT3(0.0f, 30.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 180.0f));
		m_persons.push_back(bp);
		bp = new BoxPersonActor(this, XMFLOAT3(0.0f, -30.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 120.0f));
		m_persons.push_back(bp);
		bp = new BoxPersonActor(this, XMFLOAT3(30.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 225.0f));
		m_persons.push_back(bp);
	}

	{
		BillboardActor* plane = new BillboardActor(this, Renderer::Shader3DClampLinearTwoSidedTranslucent,
			L"src\\tree.png", XMFLOAT3(-20.0f, 10.0f, 4.0f), ZeroVec3d, 0.0f, 0.0f,
			8.0f * Ones3d, Ones2d, ZeroVec2d, true, true);
		if (!plane->isEnabled()) throw std::exception();
		m_trees.push_back(plane);

		plane = new BillboardActor(this, Renderer::Shader3DClampLinearTwoSidedTranslucent,
			L"src\\tree.png", XMFLOAT3(-28.0f, 10.0f, 4.0f), ZeroVec3d, 0.0f, 0.0f,
			8.0f * Ones3d, Ones2d, ZeroVec2d, true, true);
		if (!plane->isEnabled()) throw std::exception();
		m_trees.push_back(plane);

		plane = new BillboardActor(this, Renderer::Shader3DClampLinearTwoSidedTranslucent,
			L"src\\tree.png", XMFLOAT3(-36.0f, 10.0f, 4.0f), ZeroVec3d, 0.0f, 0.0f,
			8.0f * Ones3d, Ones2d, ZeroVec2d, true, true);
		if (!plane->isEnabled()) throw std::exception();
		m_trees.push_back(plane);

		plane = new BillboardActor(this, Renderer::Shader3DClampLinearTwoSidedTranslucent,
			L"src\\tree.png", XMFLOAT3(-20.0f, -10.0f, 4.0f), ZeroVec3d, 0.0f, 0.0f,
			8.0f * Ones3d, Ones2d, ZeroVec2d, true, true);
		if (!plane->isEnabled()) throw std::exception();
		m_trees.push_back(plane);

		plane = new BillboardActor(this, Renderer::Shader3DClampLinearTwoSidedTranslucent,
			L"src\\tree.png", XMFLOAT3(-28.0f, -10.0f, 4.0f), ZeroVec3d, 0.0f, 0.0f,
			8.0f * Ones3d, Ones2d, ZeroVec2d, true, true);
		if (!plane->isEnabled()) throw std::exception();
		m_trees.push_back(plane);

		plane = new BillboardActor(this, Renderer::Shader3DClampLinearTwoSidedTranslucent,
			L"src\\tree.png", XMFLOAT3(-36.0f, -10.0f, 4.0f), ZeroVec3d, 0.0f, 0.0f,
			8.0f * Ones3d, Ones2d, ZeroVec2d, true, true);
		if (!plane->isEnabled()) throw std::exception();
		m_trees.push_back(plane);
	}

	{
		std::vector<FontIndex> findex;
		findex.push_back(FontIndex(L' ', L' ' + 96, 0));
		findex.push_back(FontIndex(L'あ', L'あ' + 190, 96));
		findex.push_back(FontIndex(0x4E00, 0x4E00 + 20950, 96 + 190));
		m_font = FontData(L"src\\PixelMplus12.png", findex, 256, 90, false);
		m_strBlockName = std::make_unique<StringActor>(this, L"", m_font,
			24.0f * Ones2d, XMFLOAT2(0.0f, 70.0f), ZeroVec2d,
			0.0f, 0.0f, Ones2d, 1.0f, 20, Renderer::Shader2DAlphaLoopPoint);
		if (!m_strBlockName->isEnabled()) throw std::exception();
	}

	{
		// ブロックメイク
		std::vector<const wchar_t*> texturePath(1);
		texturePath[0] = m_blockTexNames[m_blockTexIndex];

		for (int z = 0; z < BlockZNum; ++z)
		{
			for (int y = 0; y < BlockYNum; ++y)
			{
				for (int x = 0; x < BlockXNum; ++x)
				{
					int index = x + y * BlockXNum + z * BlockXNum * BlockYNum;
					if (z == 0)
					{
						XMFLOAT3 p = m_blockBasePos + XMFLOAT3(x * m_blockWidth,
							y * m_blockWidth, 0.5f + z * m_blockWidth);
						Actor* obj = new ContactableActor(this, Renderer::Shader3DClampLinearLit,
							texturePath, p, ZeroVec3d, ZeroVec3d, ZeroVec3d, m_blockWidth * Ones3d);
						m_objects[index] = obj;
					}
					else
					{
						m_objects[index] = nullptr;
					}
				}
			}
		}
	}

	m_isRunning = true;
}

TestScene::~TestScene()
{
	releaseActors(m_objects);
	releaseActors(m_trees);
	releaseActors(m_persons);
}

void TestScene::update(float deltaTime)
{
	const Gamepad& pad = m_game->getGamepad(0);
	const Keyboard& keyboard = m_game->getKeyboard();
	m_UI->update(deltaTime);
	m_player->update(deltaTime);
	updateActors(m_objects, deltaTime);
	updateActors(m_actors, deltaTime);
	m_chara->update(deltaTime);
	m_test->update(deltaTime);
	m_dragon->update(deltaTime);
	m_charaAnim->update(deltaTime);
	m_stars->update(deltaTime);
	m_robo->update(deltaTime);
	updateActors(m_trees, deltaTime);
	updateActors(m_persons, deltaTime);

	if (keyboard.isPressed('X'))
	{
		m_game->getRenderer()->setPostEffectShader(Renderer::PostEffectNormal);
	}
	if (keyboard.isPressed('C'))
	{
		m_game->getRenderer()->setPostEffectShader(Renderer::PostEffectInvert);
	}

	if (pad.isPressed(XINPUT_GAMEPAD_X))
	{
		if (m_tpvMode)
		{
			m_tpvMode = false;
			m_fpvCamera->uploadInParam();
			m_player->setVisible(false);
		}
		else
		{
			m_tpvMode = true;
			m_tpvCamera->resetPose();
			m_tpvCamera->uploadInParam();
			m_player->setVisible(true);
		}
	}

	if (m_tpvMode)
	{
		m_tpvCamera->update(deltaTime);
		m_tpvCamera->uploadExtParam();
	}
	else
	{
		m_fpvCamera->update(deltaTime);
		m_fpvCamera->uploadExtParam();
	}

	if (pad.isPressed(XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		m_blockTexIndex = (m_blockTexIndex + 1) % BlockTextureNum;
	}

	m_terrain->update(deltaTime);
	m_skybox->update(deltaTime);
	//m_skydome->update(deltaTime);

	removeActors(m_objects, false);
	removeActors(m_actors);
	moveActors(m_actorsTemp, m_actors);
}

void TestScene::draw()
{
	m_UI->draw();
	m_terrain->draw();
	m_skybox->draw();
	//m_skydome->draw();

	m_player->draw();
	drawActors(m_objects);
	drawActors(m_actors);
	m_chara->draw();
	m_test->draw();
	m_dragon->draw();
	m_charaAnim->draw();
	m_stars->draw();
	m_robo->draw();
	drawActors(m_trees);
	drawActors(m_persons);

	m_strBlockName->setString(m_blockTexNames[m_blockTexIndex]);
	m_strBlockName->draw();
}

void TestScene::deleteBlock(Ray ray)
{
	int minIndex = -1;
	float minRho = FLT_MAX;
	for (int i = 0; i < m_objects.size(); ++i)
	{
		if (m_objects[i] == nullptr) continue;
		if (m_objects[i]->isDead()) continue;

		ContactableActor* a = (ContactableActor*)m_objects[i];
		AABB3d aabb = a->getAABB();
		float rho = 0.0f;
		if (detectRayToAABB3dCollision(ray, aabb, &rho))
		{
			if (rho > 0.0f && rho < minRho)
			{
				minRho = rho;
				minIndex = i;
			}
		}
	}
	if (minIndex >= 0)
	{
		std::vector<UINT> indices{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		std::vector<std::vector<UINT>> anims{ indices };
		XMFLOAT2 size(64.0f, 64.0f);
		ContactableActor* a = (ContactableActor*)m_objects[minIndex];
		XMFLOAT2 spos =
			m_game->getRenderer()->projectToScreenPos(a->getPos());
		createMultipleEffects(this, L"src\\pipo-mapeffect005.png", anims, 0, 0.06f,
			10, 1, Renderer::Shader2DAddLoopPoint, 5, 0.06f, 20.0f, 16.0f, 16.0f, spos,
			Ones2d, &size);
		m_objects[minIndex]->setDead();
	}
}

void TestScene::createBlock(Ray ray)
{
	int minIndex = -1;
	float minRho = FLT_MAX;
	int minX = -1;
	int minY = -1;
	int minZ = -1;
	int minPindex = 0;
	for (int z = 0; z < BlockZNum; ++z)
	{
		for (int y = 0; y < BlockYNum; ++y)
		{
			for (int x = 0; x < BlockXNum; ++x)
			{
				int index = x + y * BlockXNum + z * BlockXNum * BlockYNum;
				if (m_objects[index] == nullptr) continue;
				if (m_objects[index]->isDead()) continue;

				ContactableActor* a = (ContactableActor*)m_objects[index];
				AABB3d aabb = a->getAABB();
				float rho = 0.0f;
				int pindex = 0;
				if (detectRayToAABB3dCollision(ray, aabb, &rho, &pindex))
				{
					if (rho > 0.0f && rho < minRho)
					{
						minIndex = index;
						minRho = rho;
						minX = x;  minY = y;  minZ = z;
						minPindex = pindex;
					}
				}
			}
		}
	}

	if (minIndex < 0)
	{
		Plane plane = m_terrain->getBoundingPlane();
		float rho = 0.0f;
		if (detectRayToPlaneCollision(ray, plane, &rho) == false) return;
		XMFLOAT3 pos = ray.calcPositionOnRay(rho);
		pos -= m_blockBasePos;
		minX = (int)(pos.x / m_blockWidth);
		minY = (int)(pos.y / m_blockWidth);
		minZ = 0;
	}
	else
	{
		if (minPindex == 0) minX += 1;
		if (minPindex == 1) minY += 1;
		if (minPindex == 2) minZ += 1;
		if (minPindex == 3) minX -= 1;
		if (minPindex == 4) minY -= 1;
		if (minPindex == 5) minZ -= 1;
	}

	if (minX < 0 || minX >= BlockXNum) return;
	if (minY < 0 || minY >= BlockYNum) return;
	if (minZ < 0 || minZ >= BlockZNum) return;
	int tindex = minX + minY * BlockXNum + minZ * BlockXNum * BlockYNum;
	if (m_objects[tindex] != nullptr) return;

	std::vector<const wchar_t*> texturePath(1);
	texturePath[0] = m_blockTexNames[m_blockTexIndex];
	XMFLOAT3 p = m_blockBasePos + XMFLOAT3(minX * m_blockWidth,
		minY * m_blockWidth, 0.5f + minZ * m_blockWidth);
	m_objects[tindex] = new ContactableActor(this, Renderer::Shader3DClampLinearLit,
		texturePath, p, ZeroVec3d, ZeroVec3d, ZeroVec3d, m_blockWidth * Ones3d);
}

