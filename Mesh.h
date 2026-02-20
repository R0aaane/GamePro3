
#pragma once

#include "GameUtil.h"

class Mesh
{
public:
	Mesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>modelIndex,
		XMFLOAT2 uvScale = Ones2d, XMFLOAT2 uvPos = ZeroVec2d,
		bool translucent = false, int shaderIndexFront = -1,
		float depthOffset = 0.0f);
	virtual ~Mesh();

	virtual void draw() = 0;

	bool isEnabled() const { return m_enabled; }
	void setUVScale(XMFLOAT2 s) { m_uvScale = s; }
	XMFLOAT2 getUVScale() const { return m_uvScale; }
	void setUVPos(XMFLOAT2 p) { m_uvPos = p; }
	XMFLOAT2 getUVPos() const { return m_uvPos; }

	void setCurrentMaterial(const MeshMaterial& m);
	void setOriginalMaterial(const MeshMaterial& m);
	void resetMaterial();

	int getInstanceNum() const { return m_instanceNum; }
	void setInstanceNum(int ins) { m_instanceNum = ins; }

	void setShadowMatrix(Plane plane, XMFLOAT3 light);

protected:
	class Actor3d* m_owner;
	class Renderer* m_renderer;
	int             m_shaderIndex;
	std::vector<int>      m_modelIndex;
	std::vector<MeshData> m_meshData;
	XMFLOAT2 m_uvScale;
	XMFLOAT2 m_uvPos;
	bool m_translucent;
	int   m_shaderIndexFront;
	float m_depthOffset;
	int   m_instanceNum;

	MeshMaterial m_oriMaterial;
	MeshMaterial m_curMaterial;

	XMMATRIX m_shadow;

	bool m_enabled;

};


