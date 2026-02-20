
#include "BoxPersonActor.h"
#include "RobolinkActor.h"
#include "Scene.h"
#include "Game.h"

BoxPersonActor::BoxPersonActor(class Scene* scene,
	const XMFLOAT3& pos, const XMFLOAT3& angle)
	: Actor3d(scene, pos, ZeroVec3d, angle)
	, m_rootNode(nullptr)
	, m_interval(2.0f)
	, m_time(0.0f)
	, m_moveTime(0.0f)
	, m_footAngle(30.0f)
	, m_armAngle(60.0f)
	, m_state(MoveState::Idle)
{
	// ルートノード
	ActorNode* node = new ActorNode;
	node->actor = this;
	m_rootNode = node;

	// 胴体
	std::vector<const wchar_t*> texName = { Renderer::WhiteTexture };
	node = new ActorNode;
	RobolinkActor* link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, 0.0f, 1.05f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.3f, 0.6f, 0.7f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.3f, 1.0f, 0.3f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.15f, 0.5f, 0.15f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, -0.35f));
	node->actor = link;
	m_rootNode->addNode(node);

	// 左足（胴体の子ノード）
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, -0.15f, -0.7f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.2f, 0.2f, 0.7f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.3f, 0.3f, 1.0f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.15f, 0.15f, 0.5f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, 0.35f));
	node->actor = link;
	m_rootNode->children[0]->addNode(node);

	// 右足（胴体の子ノード）
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, 0.15f, -0.7f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.2f, 0.2f, 0.7f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.3f, 0.3f, 1.0f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.15f, 0.15f, 0.5f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, 0.35f));
	node->actor = link;
	m_rootNode->children[0]->addNode(node);

	// 頭部（胴体の子ノード）
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, 0.0f, 0.55f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.4f, 0.4f, 0.4f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.95f, 0.73f, 0.58f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.475f, 0.365f, 0.290f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, -0.2f));
	node->actor = link;
	m_rootNode->children[0]->addNode(node);

	// 左手（胴体の子ノード）
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, -0.35f, 0.0f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.1f, 0.1f, 0.6f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.3f, 1.0f, 0.3f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.15f, 0.5f, 0.15f)));
	link->setPivot(XMFLOAT3(0.0f, 0.05f, 0.25f));
	node->actor = link;
	m_rootNode->children[0]->addNode(node);

	// 右手（胴体の子ノード）
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, 0.35f, 0.0f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.1f, 0.1f, 0.6f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.3f, 1.0f, 0.3f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.15f, 0.5f, 0.15f)));
	link->setPivot(XMFLOAT3(0.0f, -0.05f, 0.25f));
	node->actor = link;
	m_rootNode->children[0]->addNode(node);
}

BoxPersonActor::~BoxPersonActor()
{
	releaseActorNode(m_rootNode);
}

void BoxPersonActor::update(float deltaTime)
{
	m_moveTime += deltaTime;

	if (m_state == MoveState::Idle)
	{
		if (m_moveTime >= 5.0f)
		{
			m_moveTime = 0.0f;
			double rnd = m_scene->getGame()->getRand();
			if (rnd < 0.25)
			{
				m_state = MoveState::Idle;
			}
			else if (rnd < 0.75)
			{
				m_state = MoveState::Idle;
				int angleRnd = m_scene->getGame()->getRand(-180, 180);
				setAngle(XMFLOAT3(0.0f, 0.0f, m_angle.z + angleRnd));
			}
			else
			{
				m_state = MoveState::Walk;
				m_time = 0.0f;
				setVel(2.0f * getForward());
			}
		}
	}
	else if (m_state == MoveState::Walk)
	{
		if (m_moveTime >= 5.0f)
		{
			m_moveTime = 0.0f;
			setVel(ZeroVec3d);
			idle();
			m_state = MoveState::Idle;
		}
		else
		{
			translate(deltaTime);
			walk(deltaTime);
		}
	}

	calcWorldTransform();
	updateActorNode(m_rootNode, deltaTime);
}

void BoxPersonActor::draw()
{
	drawActorNode(m_rootNode);
}

void BoxPersonActor::walk(float deltaTime)
{
	m_time += deltaTime;
	m_time = (m_time >= m_interval) ? m_time - m_interval : m_time;

	float theta = XM_2PI * m_time / m_interval;
	RobolinkActor* leftFoot = (RobolinkActor*)m_rootNode->children[0]->children[0]->actor;
	leftFoot->setAngle(XMFLOAT3(0.0f, m_footAngle * std::sin(theta), 0.0f));
	RobolinkActor* rightFoot = (RobolinkActor*)m_rootNode->children[0]->children[1]->actor;
	rightFoot->setAngle(XMFLOAT3(0.0f, m_footAngle * -std::sin(theta), 0.0f));
	RobolinkActor* leftArm = (RobolinkActor*)m_rootNode->children[0]->children[3]->actor;
	leftArm->setAngle(XMFLOAT3(0.0f, m_armAngle * -std::sin(theta), 0.0f));
	RobolinkActor* rightArm = (RobolinkActor*)m_rootNode->children[0]->children[4]->actor;
	rightArm->setAngle(XMFLOAT3(0.0f, m_armAngle * std::sin(theta), 0.0f));
}

void BoxPersonActor::idle()
{
	RobolinkActor* leftFoot = (RobolinkActor*)m_rootNode->children[0]->children[0]->actor;
	leftFoot->setAngle(ZeroVec3d);
	RobolinkActor* rightFoot = (RobolinkActor*)m_rootNode->children[0]->children[1]->actor;
	rightFoot->setAngle(ZeroVec3d);
	RobolinkActor* leftArm = (RobolinkActor*)m_rootNode->children[0]->children[3]->actor;
	leftArm->setAngle(ZeroVec3d);
	RobolinkActor* rightArm = (RobolinkActor*)m_rootNode->children[0]->children[4]->actor;
	rightArm->setAngle(ZeroVec3d);
}



