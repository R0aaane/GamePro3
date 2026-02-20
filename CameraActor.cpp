
#include "CameraActor.h"
#include "Game.h"
#include "Scene.h"
#include "Renderer.h"

CameraActor::CameraActor(Scene* scene, const Camera& cam,
	const XMFLOAT3& pos, const XMFLOAT3& vel, 
	const XMFLOAT3& angle, const XMFLOAT3& avel)
	: Actor3d(scene, pos, vel, angle, avel)
	, m_camera(cam)
{
}

CameraActor::~CameraActor()
{
}

void CameraActor::update(float deltaTime)
{
	setPos(m_pos + deltaTime * m_vel);
	setAngle(m_angle + deltaTime * m_angleVel);
	calcWorldTransform();
}

void CameraActor::draw()
{
}

void CameraActor::uploadParam()
{
	XMFLOAT3 pos(m_worldTransform.r[3].m128_f32[0],
		m_worldTransform.r[3].m128_f32[1],
		m_worldTransform.r[3].m128_f32[2]);
	m_camera.setExtParam(CamExtParam(pos, pos + getForward(), getUp()));
	m_scene->getGame()->getRenderer()->setCameraParam(
		m_camera.getInParam(), m_camera.getExtParam());
}

void CameraActor::uploadInParam()
{
	m_scene->getGame()->getRenderer()->setCameraInParam(
		m_camera.getInParam());
}

void CameraActor::uploadExtParam()
{
	XMFLOAT3 pos(m_worldTransform.r[3].m128_f32[0],
		m_worldTransform.r[3].m128_f32[1],
		m_worldTransform.r[3].m128_f32[2]);
	m_camera.setExtParam(CamExtParam(pos, pos + getForward(), getUp()));
	m_scene->getGame()->getRenderer()->setCameraExtParam(
		m_camera.getExtParam());
}

