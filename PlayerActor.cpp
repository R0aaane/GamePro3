
#include "PlayerActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"
#include "TestScene.h"
#include "ContactableActor.h"
#include "TerrainActor.h"

PlayerActor::PlayerActor(Scene* scene,
	int shaderIndex, float radius, const wchar_t* filePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel, 
	const XMFLOAT3& angle, const XMFLOAT3& avel, const XMFLOAT3& scale)
	: FileAnimMeshActor(scene, shaderIndex, filePath,
		pos, vel, angle, avel, scale)
	, m_radius(radius)
	, m_moveState(MoveState::Idle)
	, m_moveForce(100.0f)
	, m_rotateSpeed(90.0f)
	, m_jumpSpeed(5.0f)
	, m_jumpTime(0.0f)
	, m_jumpAssistTime(0.6f)
	, m_fallForce(100.0f)
	, m_jumpVector(UnitVecZ3d)
	, m_permissionHeight(0.6f)
	, m_reticleMode(false)
	, m_reticleSpeed(400.0f)
	, m_reticleRadius(32.0f)
{
	setOriginalMaterial(MeshMaterial(Ones3d, 0.1f * Ones3d, 0.2f, 0.4f * Ones3d));

	m_lightPos = XMFLOAT3(0.0f, 0.0f, 0.8f);
	m_lightVec = unitVecAngleY(XMConvertToRadians(120.0f));

	XMFLOAT3 p = mul(m_lightPos, m_worldTransform, 1.0f);
	XMFLOAT3 v = mul(m_lightVec, m_worldTransform, 0.0f);
	m_spotLight = SpotLight(p, 30.0f, v, 40.0f, 0.0f * Ones3d);
	m_spotLightIndex = 
		m_scene->getGame()->getRenderer()->allocateSpotLight(m_spotLight);

	m_reticle = std::make_unique<SpriteActor>(m_scene, L"src\\reticle.png",
		Renderer::Shader2DAlphaLoopPoint, XMFLOAT2(200.0f, 100.0f));
	if (!m_reticle->isEnabled()) throw std::exception();

	std::vector<FontIndex> findex;
	findex.push_back(FontIndex(L' ', L' ' + 96, 0));
	findex.push_back(FontIndex(L'あ', L'あ' + 190, 96));
	findex.push_back(FontIndex(0x4E00, 0x4E00 + 20950, 96 + 190));
	m_font = FontData(L"src\\PixelMplus12.png", findex, 256, 90, false);
	m_str = std::make_unique<StringActor>(m_scene, L"", m_font, 24.0f * Ones2d, ZeroVec2d, ZeroVec2d,
		0.0f, 0.0f, Ones2d, 1.0f, 20, Renderer::Shader2DAlphaLoopPoint);
	if (!m_str->isEnabled()) throw std::exception();
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::update(float deltaTime)
{
	const Gamepad& pad = m_scene->getGame()->getGamepad(0);
	int vx = 0, vy = 0;
	if (pad.isDown(XINPUT_GAMEPAD_DPAD_UP))    vx += 1;
	if (pad.isDown(XINPUT_GAMEPAD_DPAD_DOWN))  vx -= 1;
	if (pad.isDown(XINPUT_GAMEPAD_DPAD_RIGHT)) vy += 1;
	if (pad.isDown(XINPUT_GAMEPAD_DPAD_LEFT))  vy -= 1;

	XMFLOAT3 force = ZeroVec3d;
	if (m_moveState == MoveState::Idle)
	{
		// Idle状態
		if (pad.isDown(XINPUT_GAMEPAD_A))
		{
			m_moveState = MoveState::Jump;
			m_vel += m_jumpSpeed * UnitVecZ3d;
			m_jumpTime = m_jumpAssistTime;
			m_jumpVector = normalize(m_vel);
		}
		else
		{
			if (vx != 0)
			{
				m_moveState = MoveState::Walk;
				force = (float)vx * getForward() * m_moveForce;
			}
			setAngle(XMFLOAT3(0.0f, 0.0f, m_angle.z + ((float)vy * m_rotateSpeed * deltaTime)));
		}
	}
	else if (m_moveState == MoveState::Walk)
	{
		// Walk状態
		if (pad.isDown(XINPUT_GAMEPAD_A))
		{
			m_moveState = MoveState::Jump;
			m_vel += m_jumpSpeed * UnitVecZ3d;
			m_jumpTime = m_jumpAssistTime;
			m_jumpVector = normalize(m_vel);
		}
		else
		{
			if (vx != 0)
			{
				force = (float)vx * getForward() * m_moveForce;
			}
			else
			{
				m_moveState = MoveState::Idle;
				m_vel = ZeroVec3d;
				m_accel = ZeroVec3d;
			}
			setAngle(XMFLOAT3(0.0f, 0.0f, m_angle.z + ((float)vy * m_rotateSpeed * deltaTime)));
		}
	}
	else if (m_moveState == MoveState::Jump)
	{
		// Jump状態
		if (m_jumpTime > 0.0f)
		{
			if (pad.isDown(XINPUT_GAMEPAD_A))
			{
				m_vel = m_jumpSpeed * m_jumpVector;
				m_jumpTime -= deltaTime;
			}
			else
			{
				m_jumpTime = 0.0f;
			}
		}
		else if (m_vel.z < 0.0f)
		{
			force = -m_fallForce * UnitVecZ3d;
		}
	}
	simulatePhysics(deltaTime, force);

	bool grounded = false;
	{
		TestScene* scene = (TestScene*)m_scene;
		TerrainActor* terrain = scene->getTerrain();
		XMFLOAT3 corr = -1.0f * UnitVecZ3d;
		Ray ray(m_pos, corr);
		GridHeightMap map = terrain->getBoundingGridHeightMap();
		float len = 0.0f;
		bool ret = detectRayToGridMeshCollision(ray, map, &len);
		//if (ret == true && (len - m_radius) < 0.0f)
		if (ret == true && len < 0.0f)
		{
			grounded = true;
			setPos(m_pos + (len + 10.0f * FLT_EPSILON) * corr);
			calcWorldTransform();
			if (m_moveState == MoveState::Idle)
			{
				m_vel = ZeroVec3d;
				m_accel = ZeroVec3d;
			}
			else if (m_moveState == MoveState::Walk)
			{
				m_vel.z = 0.0f;
				m_accel.z = 0.0f;
			}
			else if (m_moveState == MoveState::Jump)
			{
				m_moveState = MoveState::Idle;
				m_vel = ZeroVec3d;
				m_accel = ZeroVec3d;
			}
		}
		//else if (ret == true && (len - m_radius) < m_permissionHeight)
		else if (ret == true && len < m_permissionHeight)
		{
			if (m_moveState != MoveState::Jump)
			{
				grounded = true;
				setPos(m_pos + (len + 10.0f * FLT_EPSILON) * corr);
				calcWorldTransform();
				if (m_moveState == MoveState::Idle)
				{
					m_vel = ZeroVec3d;
					m_accel = ZeroVec3d;
				}
				else if (m_moveState == MoveState::Walk)
				{
					m_vel.z = 0.0f;
					m_accel.z = 0.0f;
				}
			}
		}
	}

	TestScene* scene = (TestScene*)m_scene;
	std::vector<Actor*>& obj = scene->getObjects();
	Sphere s = getBoundingSphere();
	XMFLOAT3 fd = (float)vx * getForward();
	for (int i = 0; i < obj.size(); ++i)
	{
		ContactableActor* a = (ContactableActor*)obj[i];
		if (a == nullptr) continue;
		if (a->isDead()) continue;

		OBB3d b = a->getBoundingBox();
		XMFLOAT3 corr = ZeroVec3d;
		float len = 0.0f;
		if (detectSphereToOBB3dCollision(s, b, &corr, &len))
		{
			if (std::fabs(corr.z - 1.0f) < 0.2f)
			{
				grounded = true;
				if (m_moveState == MoveState::Jump)
				{
					m_moveState = MoveState::Idle;
					m_vel = ZeroVec3d;
				}
			}
			else if (m_moveState == MoveState::Walk)
			{
				float rho = 0.0f;
				Ray ray(m_pos + fd * m_radius + UnitVecZ3d * m_radius, -1.0f * UnitVecZ3d);
				if (detectRayToOBB3dCollision(ray, b, &rho))
				{
					if (std::fabs(m_radius - rho) < m_permissionHeight)
					{
						//corr = UnitVecZ3d * (m_radius - rho) + fd * m_radius;
						corr = UnitVecZ3d * (-rho) + fd * m_radius;
						len = 1.0f;
						grounded = true;
					}
				}
			}
			setPos(m_pos + len * corr);
			calcWorldTransform();
		}
	}

	if (grounded == false && m_moveState != MoveState::Jump)
	{
		m_moveState = MoveState::Jump;
		m_jumpTime = 0.0f;
	}

	// レティクル処理
	if (pad.isPressed(XINPUT_GAMEPAD_BACK)) m_reticleMode = !m_reticleMode;
	if (m_reticleMode)
	{
		float width = (float)m_scene->getGame()->getWidth();
		float height = (float)m_scene->getGame()->getHeight();
		XMFLOAT2 stick = pad.getLeftStick();
		XMFLOAT2 pos = m_reticle->getPos();
		pos += m_reticleSpeed * deltaTime * stick;
		if (pos.x - m_reticleRadius < 0.0f)    pos.x = m_reticleRadius;
		if (pos.x + m_reticleRadius >= width)  pos.x = width - m_reticleRadius;
		if (pos.y - m_reticleRadius < 0.0f)    pos.y = m_reticleRadius;
		if (pos.y + m_reticleRadius >= height) pos.y = height - m_reticleRadius;
		m_reticle->setPos(pos);
		m_reticle->update(deltaTime);

		// レイキャスト
		if (pad.isPressed(XINPUT_GAMEPAD_Y))
		{
			Ray ray = m_scene->getGame()->getRenderer()->getCameraRayOnScreenPos(
				m_reticle->getPos());
			scene->deleteBlock(ray);
		}
		else if (pad.isPressed(XINPUT_GAMEPAD_B))
		{
			Ray ray = m_scene->getGame()->getRenderer()->getCameraRayOnScreenPos(
				m_reticle->getPos());
			scene->createBlock(ray);
		}
	}

	m_spotLight.pos = mul(m_lightPos, m_worldTransform, 1.0f);
	m_spotLight.dir = mul(m_lightVec, m_worldTransform, 0.0f);
	m_scene->getGame()->getRenderer()->setSpotLihgt(m_spotLightIndex, m_spotLight);

	updateAnim(deltaTime);
}

void PlayerActor::draw()
{
	if (m_reticleMode)
	{
		m_reticle->draw();
	}
	
	if (m_visible == false) return;

	//FileMeshActor::draw();
	FileAnimMeshActor::draw();
	if (m_moveState == MoveState::Idle)
	{
		m_str->setString(L"Idle");
	}
	else if (m_moveState == MoveState::Walk)
	{
		m_str->setString(L"Walk");
	}
	else
	{
		m_str->setString(L"Jump");
	}
	m_str->draw();
}






