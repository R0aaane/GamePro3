
#pragma once

#include "Mesh.h"

class GridMesh : public Mesh
{
public:
	GridMesh(class Actor3d* owner, class Renderer* renderer,
		int shaderIndex, const std::vector<int>& modelIndex,
		const wchar_t* meshName, const GridHeightMap& map,
		const XMFLOAT2& uvScale = Ones2d,
		const XMFLOAT2& uvPos = ZeroVec2d,
		bool translucent = false);
	virtual ~GridMesh();

	void draw() override;

};




