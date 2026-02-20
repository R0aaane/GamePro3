
#include "RoboActor.h"
#include "RobolinkActor.h"
#include "Scene.h"
#include "Game.h"

RoboActor::RoboActor(class Scene* scene,
	const XMFLOAT3& pos, const XMFLOAT3& angle)
	: Actor3d(scene, pos, ZeroVec3d, angle)
	, m_rootNode(nullptr)
{
	// ルートノード
	ActorNode* node = new ActorNode;
	node->actor = this;
	m_rootNode = node;

	// リンク1
	std::vector<const wchar_t*> texName = { Renderer::WhiteTexture };
	node = new ActorNode;
	RobolinkActor* link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, ZeroVec3d, ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.2f, 0.2f, 1.0f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 0.2f, 0.2f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.3f, 0.1f, 0.1f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, -0.5f));
	node->actor = link;
	m_rootNode->addNode(node);

	// リンク2
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, 0.0f, 1.0f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.2f, 0.2f, 1.0f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.2f, 1.0f, 0.2f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.1f, 0.3f, 0.1f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, -0.5f));
	node->actor = link;
	m_rootNode->children[0]->addNode(node);

	// リンク3
	node = new ActorNode;
	link = new RobolinkActor(scene, Renderer::Shader3DClampLinearLit,
		texName, true, XMFLOAT3(0.0f, 0.0f, 1.0f), ZeroVec3d, ZeroVec3d, ZeroVec3d,
		XMFLOAT3(0.2f, 0.2f, 1.0f));
	link->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.2f, 0.2f, 1.0f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.1f, 0.1f, 0.3f)));
	link->setPivot(XMFLOAT3(0.0f, 0.0f, -0.5f));
	node->actor = link;
	m_rootNode->children[0]->children[0]->addNode(node);
}

RoboActor::~RoboActor()
{
	releaseActorNode(m_rootNode);
}

void RoboActor::update(float deltaTime)
{
	const Keyboard& keyboard = m_scene->getGame()->getKeyboard();
	const float speed = 30.0f;

	int a1 = 0;
	if (keyboard.isDown('U')) ++a1;
	if (keyboard.isDown('I')) --a1;
	RobolinkActor* link = (RobolinkActor*)m_rootNode->children[0]->actor;
	link->setAngleVel(XMFLOAT3(speed * (float)a1, 0.0f, 0.0f));

	int a2 = 0;
	if (keyboard.isDown('O')) ++a2;
	if (keyboard.isDown('P')) --a2;
	link = (RobolinkActor*)m_rootNode->children[0]->children[0]->actor;
	link->setAngleVel(XMFLOAT3(speed * (float)a2, 0.0f, 0.0f));

	int a3 = 0;
	if (keyboard.isDown('J')) ++a3;
	if (keyboard.isDown('K')) --a3;
	link = (RobolinkActor*)m_rootNode->children[0]->children[0]->children[0]->actor;
	link->setAngleVel(XMFLOAT3(speed * (float)a3, 0.0f, 0.0f));

	simulate(deltaTime);
	calcWorldTransform();
	updateActorNode(m_rootNode, deltaTime);
}

void RoboActor::draw()
{
	drawActorNode(m_rootNode);
}



