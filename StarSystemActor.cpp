
#include "StarSystemActor.h"
#include "StarActor.h"
#include "Scene.h"
#include "Game.h"

StarSystemActor::StarSystemActor(Scene* scene,
	const XMFLOAT3& pos, const XMFLOAT3& angle)
	: Actor3d(scene, pos, ZeroVec3d, angle, ZeroVec3d, Ones3d)
	, m_rootNode(nullptr)
{
	// ルートノード
	ActorNode* node = new ActorNode;
	node->actor = this;
	m_rootNode = node;

	// 太陽
	node = new ActorNode;
	StarActor* star = new StarActor(scene, Renderer::Shader3DClampLinearLit,
		Renderer::WhiteTexture, true, 0.0f, 0.0f, 0.0f, 2.0f * Ones3d);
	star->setOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 0.6f, 0.2f),
		0.01f * Ones3d, 1.0f, XMFLOAT3(0.5f, 0.3f, 0.1f)));
	node->actor = star;
	m_rootNode->addNode(node);

	// 水星
	node = new ActorNode;
	star = new StarActor(scene, Renderer::Shader3DClampLinearLit,
		Renderer::WhiteTexture, true, 4.0f, 0.0f, 90.0f, 0.12f * Ones3d);
	star->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.8f, 0.7f, 0.2f),
		0.01f * Ones3d, 1.0f, XMFLOAT3(0.15f, 0.15f, 0.35f)));
	node->actor = star;
	m_rootNode->children[0]->addNode(node);

	// 金星
	node = new ActorNode;
	star = new StarActor(scene, Renderer::Shader3DClampLinearLit,
		Renderer::WhiteTexture, true, 5.0f, 60.0f, 20.0f, 0.4f * Ones3d);
	star->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.8f, 0.7f, 0.2f),
		0.01f * Ones3d, 1.0f, XMFLOAT3(0.4f, 0.35f, 0.1f)));
	node->actor = star;
	m_rootNode->children[0]->addNode(node);

	// 地球
	node = new ActorNode;
	star = new StarActor(scene, Renderer::Shader3DClampLinearLit,
		Renderer::WhiteTexture, true, 7.0f, 180.0f, 30.0f, 0.4f * Ones3d);
	star->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.0f, 0.3f, 0.8f),
		0.01f * Ones3d, 1.0f, XMFLOAT3(0.0f, 0.15f, 0.4f)));
	node->actor = star;
	m_rootNode->children[0]->addNode(node);

	// 月
	node = new ActorNode;
	star = new StarActor(scene, Renderer::Shader3DClampLinearLit,
		Renderer::WhiteTexture, true, 1.0f, 90.0f, 180.0f, 0.1f * Ones3d);
	star->setOriginalMaterial(MeshMaterial(XMFLOAT3(0.9f, 0.9f, 0.9f),
		0.01f * Ones3d, 1.0f, XMFLOAT3(0.45f, 0.45f, 0.45f)));
	node->actor = star;
	m_rootNode->children[0]->children[2]->addNode(node);
}

StarSystemActor::~StarSystemActor()
{
	releaseActorNode(m_rootNode);
}

void StarSystemActor::update(float deltaTime)
{
	const Keyboard& keyboard = m_scene->getGame()->getKeyboard();
	int x = 0, y = 0;
	if (keyboard.isDown('W')) x++;
	if (keyboard.isDown('S')) x--;
	if (keyboard.isDown('D')) y++;
	if (keyboard.isDown('A')) y--;
	XMFLOAT3 pos = m_pos + deltaTime * ((float)x * UnitVecX3d + (float)y * UnitVecY3d);
	setPos(pos);

	calcWorldTransform();
	updateActorNode(m_rootNode, deltaTime);
}

void StarSystemActor::draw()
{
	drawActorNode(m_rootNode);
}


