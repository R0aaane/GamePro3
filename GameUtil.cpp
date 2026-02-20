
#include "GameUtil.h"

// カラーベクトルの定数
const XMFLOAT3 ColorWhite(1.0f, 1.0f, 1.0f);
const XMFLOAT3 ColorBlack(0.0f, 0.0f, 0.0f);
const XMFLOAT3 ColorGray(0.5f, 0.5f, 0.5f);
const XMFLOAT3 ColorRed(1.0f, 0.0f, 0.0f);
const XMFLOAT3 ColorGreen(0.0f, 1.0f, 0.0f);
const XMFLOAT3 ColorBlue(0.0f, 0.0f, 1.0f);
const XMFLOAT3 ColorYellow(1.0f, 1.0f, 0.0f);
const XMFLOAT3 ColorCyan(0.0f, 1.0f, 1.0f);
const XMFLOAT3 ColorMagenta(1.0f, 0.0f, 1.0f);

ColorRGBA::ColorRGBA(XMFLOAT3 color, float alpha)
	: r((unsigned char)(255.0f * color.x))
	, g((unsigned char)(255.0f * color.y))
	, b((unsigned char)(255.0f * color.z))
	, a((unsigned char)(255.0f * alpha))
{
}

// 点と円の衝突（内外）判定
bool detectPointToCircleCollision(XMFLOAT2& p, Circle& c)
{
	XMFLOAT2 d = p - c.pos;
	if (lengthSq(d) <= c.radius * c.radius) return true;
	return false;
}

// 点と矩形の衝突（内外）判定
bool detectPointToBoxCollision(XMFLOAT2& p, Box& box)
{
	XMFLOAT2 p1 = box.pos;
	XMFLOAT2 p2 = box.pos + XMFLOAT2(box.width, box.height);
	if (p.x < p1.x) return false;
	if (p.x > p2.x) return false;
	if (p.y < p1.y) return false;
	if (p.y > p2.y) return false;
	return true;
}

// 円と円の衝突判定
bool detectCircleCollision(Circle& c1, Circle& c2)
{
	XMFLOAT2 d = c2.pos - c1.pos;
	float th = c1.radius + c2.radius;
	if (lengthSq(d) < th * th) return true;
	return false;
}

// 矩形と矩形の衝突判定
bool detectBoxCollision(Box& b1, Box& b2)
{
	XMFLOAT2 p1min = b1.pos;
	XMFLOAT2 p1max = b1.pos + XMFLOAT2(b1.width, b1.height);
	XMFLOAT2 p2min = b2.pos;
	XMFLOAT2 p2max = b2.pos + XMFLOAT2(b2.width, b2.height);

	if (p1max.x < p2min.x) return false;
	if (p2max.x < p1min.x) return false;
	if (p1max.y < p2min.y) return false;
	if (p2max.y < p1min.y) return false;
	return true;
}

// 円と矩形の衝突判定
bool detectBoxToCircleCollision(Box& b, Circle& c)
{
	XMFLOAT2 bmin = b.pos;
	XMFLOAT2 bmax = b.pos + XMFLOAT2(b.width, b.height);
	XMFLOAT2 s;
	s.x = (c.pos.x > bmax.x) ? bmax.x
		: (bmin.x > c.pos.x) ? bmin.x : c.pos.x;
	s.y = (c.pos.y > bmax.y) ? bmax.y
		: (bmin.y > c.pos.y) ? bmin.y : c.pos.y;

	return lengthSq(s - c.pos) <= (c.radius * c.radius);
}

// 円と線分の衝突判定
bool detectCircleToSegmentCollision(Circle& c, Segment& seg)
{
	XMFLOAT2 p = seg.end - seg.start;
	XMFLOAT2 q = c.pos - seg.start;
	float d = std::abs(cross(p, q) / length(p));
	if (d > c.radius) return false;

	XMFLOAT2 w = c.pos - seg.end;
	float t = dot(p, q) * dot(-1.0f * p, w);
	if (t >= 0.0f) return true;

	float csq = c.radius * c.radius;
	if (lengthSq(q) <= csq) return true;
	if (lengthSq(w) <= csq) return true;
	return false;
}

// 矩形を線分に分割
void separateBoxToSegments(const Box& rect, Segment& left, Segment& right,
	Segment& top, Segment& bottom)
{
	XMFLOAT2 p1 = rect.pos;
	XMFLOAT2 p2 = rect.pos + XMFLOAT2(rect.width, 0.0f);
	XMFLOAT2 p3 = rect.pos + XMFLOAT2(0.0f, rect.height);
	XMFLOAT2 p4 = rect.pos + XMFLOAT2(rect.width, rect.height);

	left = Segment(p1, p3);
	right = Segment(p2, p4);
	top = Segment(p1, p2);
	bottom = Segment(p3, p4);
}

// 数値情報を出力ウィンドウに表示
void printNum(const wchar_t* str, int num)
{
	static wchar_t buf[1024];
	wsprintfW(buf, str, num);
	OutputDebugStringW(buf);
}

// スプライト座標変換行列
XMMATRIX calcSpriteMatrix(float width, float height)
{
	XMMATRIX mat = XMMatrixIdentity();
	mat.r[0].m128_f32[0] = 2.0f / width;
	mat.r[1].m128_f32[1] = -2.0f / height;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;

	return mat;
}

// スプライトモデル変換行列
XMMATRIX calcSpriteModelMatrix(XMFLOAT2 scale,
	XMFLOAT2 pos, XMFLOAT2 offset, float theta)
{
	float t = XMConvertToRadians(theta);
	float s = XMScalarSin(t);
	float c = XMScalarCos(t);
	XMMATRIX mat = XMMatrixIdentity();
	mat.r[0].m128_f32[0] = scale.x * c;
	mat.r[0].m128_f32[1] = scale.x * s;
	mat.r[1].m128_f32[0] = scale.y * s;
	mat.r[1].m128_f32[1] = -scale.y * c;
	mat.r[3].m128_f32[0] = pos.x - offset.x * c + offset.y * s;
	mat.r[3].m128_f32[1] = pos.y - offset.x * s - offset.y * c;

	return mat;
}

// スプライトのuv座標変換行列
XMMATRIX calcSpriteUVMatrix(const XMFLOAT2& p1, float uw, float uh,
	float imgW, float imgH)
{
	XMMATRIX mat = XMMatrixIdentity();
	float iw = 1.0f / imgW;
	float ih = 1.0f / imgH;
	mat.r[0].m128_f32[0] = uw * iw;
	mat.r[1].m128_f32[1] = uh * ih;
	mat.r[3].m128_f32[0] = p1.x * iw;
	mat.r[3].m128_f32[1] = p1.y * ih;

	return mat;
}

// スプライトのuv座標変換行列
XMMATRIX calcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2,
	float imgW, float imgH)
{
	return calcSpriteUVMatrix(p1, p2.x - p1.x, p2.y - p1.y, imgW, imgH);
}

// 文字コードのインデックス対応
bool FontIndex::isInRange(wchar_t c)
{
	return (start <= c) && (c <= end);
}

MeshMaterial::MeshMaterial()
	: diffuse(Ones3d), padA(0.0f)
	, specular(0.1f * Ones3d), power(1.0f)
	, ambient(0.1f * Ones3d), padB(0.0f)
{
}

MeshMaterial::MeshMaterial(const XMFLOAT3& diff,
	const XMFLOAT3& spec, float p, const XMFLOAT3& amb)
	: diffuse(diff), padA(0.0f)
	, specular(spec), power(p)
	, ambient(amb), padB(0.0f)
{
}

MeshConstData::MeshConstData()
	: world(XMMatrixIdentity())
	, uvTrans(XMMatrixIdentity())
	, scale(Ones3d)
	, pad(0.0f)
	, shadow(XMMatrixIdentity())
{
}

MeshConstData::MeshConstData(const XMMATRIX& w, const XMMATRIX& uv, const XMFLOAT3& s,
	const MeshMaterial& m)
	: world(w)
	, uvTrans(uv)
	, scale(s)
	, pad(0.0f)
	, material(m)
	, shadow(XMMatrixIdentity())
{
}

XMMATRIX calcMeshUVMatrix(const XMFLOAT2& scale, const XMFLOAT2& pos)
{
	XMMATRIX mat = XMMatrixIdentity();
	mat.r[0].m128_f32[0] = scale.x;
	mat.r[1].m128_f32[1] = scale.y;
	mat.r[3].m128_f32[0] = pos.x;
	mat.r[3].m128_f32[1] = pos.y;

	return mat;
}

ParallelLight::ParallelLight()
	: direction(-1.0f * UnitVecZ3d), padA(0.0f)
	, color(Ones3d), padB(0.0f)
{
}

ParallelLight::ParallelLight(XMFLOAT3 dir, XMFLOAT3 col)
	: direction(dir), padA(0.0f)
	, color(col), padB(0.0f)
{
}

PointLight::PointLight()
	: pos(ZeroVec3d), pad(0.0f)
	, color(ZeroVec3d), radius(1.0f)
{
}

PointLight::PointLight(const XMFLOAT3& p, const XMFLOAT3 col, float r)
	: pos(p), pad(0.0f)
	, color(col), radius(r)
{
}

SpotLight::SpotLight()
	: pos(ZeroVec3d)
	, radius(1.0f)
	, dir(-1.0f * UnitVecZ3d)
	, angle(30.0f)
	, color(Ones3d)
	, pad(0.0f)
{
}

SpotLight::SpotLight(const XMFLOAT3& p, float r, const XMFLOAT3& d,
	float a, const XMFLOAT3& col)
	: pos(p)
	, radius(r)
	, dir(d)
	, angle(a)
	, color(col)
	, pad(0.0f)
{
}

SceneData::SceneData()
	: viewProj(XMMatrixIdentity())
	, eye(ZeroVec3d), padA(0.0f)
	, ambientLight(Ones3d), padB(0.0f)
{
}

// 点と球の衝突（内外）判定
bool detectPointToSphereCollision(XMFLOAT3& p, const Sphere& sh)
{
	XMFLOAT3 d = p - sh.pos;

	return (lengthSq(d) <= sh.r * sh.r) ? true : false;
}

// 球と球の衝突判定
bool detectSphereCollision(Sphere& s1, Sphere& s2,
	XMFLOAT3* corr, float* len)
{
	XMFLOAT3 d = s1.pos - s2.pos;

	float sq = (s1.r + s2.r) * (s1.r + s2.r);
	float dlen = lengthSq(d);

	//return (lengthSq(d) <= sq) ? true : false;
	if (dlen > sq)
	{
		return false;
	}
	else
	{
		if (corr != nullptr) *corr = normalize(d);
		if (len != nullptr) *len = s1.r + s2.r - std::sqrt(dlen);

		return true;
	}
}

AABB3d::AABB3d()
	: curMax(0.5f * Ones3d), curMin(-0.5f * Ones3d)
	, oriMax(0.5f * Ones3d), oriMin(-0.5f * Ones3d)
{
}

AABB3d::AABB3d(std::vector<XMFLOAT3>& points)
{
	initialize(points);
}

void AABB3d::initialize(std::vector<XMFLOAT3>& points)
{
	if (points.size() == 0) return;

	oriMax = points[0];
	oriMin = points[0];
	for (size_t i = 1; i < points.size(); ++i)
	{
		if (oriMax.x < points[i].x) oriMax.x = points[i].x;
		if (oriMin.x > points[i].x) oriMin.x = points[i].x;
		if (oriMax.y < points[i].y) oriMax.y = points[i].y;
		if (oriMin.y > points[i].y) oriMin.y = points[i].y;
		if (oriMax.z < points[i].z) oriMax.z = points[i].z;
		if (oriMin.z > points[i].z) oriMin.z = points[i].z;
	}
	curMax = oriMax;
	curMin = oriMin;
}

void AABB3d::update(const XMMATRIX& world)
{
	std::vector<XMFLOAT3> points(8);
	points[0] = XMFLOAT3(oriMax.x, oriMax.y, oriMax.z);
	points[1] = XMFLOAT3(oriMin.x, oriMax.y, oriMax.z);
	points[2] = XMFLOAT3(oriMax.x, oriMin.y, oriMax.z);
	points[3] = XMFLOAT3(oriMax.x, oriMax.y, oriMin.z);
	points[4] = XMFLOAT3(oriMin.x, oriMin.y, oriMax.z);
	points[5] = XMFLOAT3(oriMin.x, oriMax.y, oriMin.z);
	points[6] = XMFLOAT3(oriMax.x, oriMin.y, oriMin.z);
	points[7] = XMFLOAT3(oriMin.x, oriMin.y, oriMin.z);

	curMax = mul(points[0], world, 1.0f);
	curMin = curMax;
	for (size_t i = 1; i < points.size(); ++i)
	{
		XMFLOAT3 p = mul(points[i], world, 1.0f);
		if (curMax.x < p.x) curMax.x = p.x;
		if (curMin.x > p.x) curMin.x = p.x;
		if (curMax.y < p.y) curMax.y = p.y;
		if (curMin.y > p.y) curMin.y = p.y;
		if (curMax.z < p.z) curMax.z = p.z;
		if (curMin.z > p.z) curMin.z = p.z;
	}
}

// AABB3dとAABB3dの衝突判定
bool detectAABB3dCollision(AABB3d& b1, AABB3d& b2,
	XMFLOAT3* corr, float* len)
{
	if (b1.curMax.x < b2.curMin.x) return false;
	if (b2.curMax.x < b1.curMin.x) return false;
	if (b1.curMax.y < b2.curMin.y) return false;
	if (b2.curMax.y < b1.curMin.y) return false;
	if (b1.curMax.z < b2.curMin.z) return false;
	if (b2.curMax.z < b1.curMin.z) return false;
	if (corr == nullptr && len == nullptr) return true;

	XMFLOAT3 diff1 = b2.curMax - b1.curMin;
	XMFLOAT3 diff2 = b2.curMin - b1.curMax;
	XMFLOAT3 d = ZeroVec3d;
	d.x = (std::fabs(diff1.x) < std::fabs(diff2.x)) ? diff1.x : diff2.x;
	d.y = (std::fabs(diff1.y) < std::fabs(diff2.y)) ? diff1.y : diff2.y;
	d.z = (std::fabs(diff1.z) < std::fabs(diff2.z)) ? diff1.z : diff2.z;

	XMFLOAT3 c = ZeroVec3d;
	float v = 0.0f;
	XMFLOAT3 a(std::fabs(d.x), std::fabs(d.y), std::fabs(d.z));
	if (a.x <= a.y)
	{
		if (a.x <= a.z)
		{
			c = UnitVecX3d;
			v = d.x;
		}
		else
		{
			c = UnitVecZ3d;
			v = d.z;
		}
	}
	else if (a.y <= a.z)
	{
		c = UnitVecY3d;
		v = d.y;
	}
	else
	{
		c = UnitVecZ3d;
		v = d.z;
	}

	if (corr != nullptr) *corr = c;
	if (len != nullptr) *len = v;
	return true;
}

OBB3d::OBB3d()
	: pos(ZeroVec3d)
	, dir{ UnitVecX3d, UnitVecY3d, UnitVecZ3d }
	, scale(Ones3d)
{
}

OBB3d::OBB3d(XMFLOAT3 p, XMFLOAT3 dx, XMFLOAT3 dy, XMFLOAT3 dz, XMFLOAT3 s)
	: pos(p)
	, dir{ dx, dy, dz }
	, scale(s)
{
}

// OBB3dとOBB3dの衝突判定
bool detectOBB3dCollision(OBB3d& b1, OBB3d& b2,
	XMFLOAT3* corr, float* len)
{
	XMFLOAT3 b[6] =
	{
		b1.dir[0] * b1.scale.x,
		b1.dir[1] * b1.scale.y,
		b1.dir[2] * b1.scale.z,
		b2.dir[0] * b2.scale.x,
		b2.dir[1] * b2.scale.y,
		b2.dir[2] * b2.scale.z
	};
	XMFLOAT3 d[6] =
	{
		b1.dir[0], b1.dir[1], b1.dir[2],
		b2.dir[0], b2.dir[1], b2.dir[2]
	};
	float s[6] =
	{
		b1.scale.x, b1.scale.y, b1.scale.z,
		b2.scale.x, b2.scale.y, b2.scale.z
	};
	XMFLOAT3 r = b1.pos - b2.pos;

	// 各面に対応する分離軸の判定
	XMFLOAT3 c = ZeroVec3d;
	float vmin = FLT_MAX;
	for (int i = 0; i < 6; ++i)
	{
		XMFLOAT3 sep = d[i];
		float d1 = s[i];
		float d2 = (i < 3) ?
			calcLengthProjAxis(sep, b[3], b[4], b[5])
			: calcLengthProjAxis(sep, b[0], b[1], b[2]);
		float w = dot(r, sep);
		float wa = std::fabs(w);
		if (wa > d1 + d2) return false;
		float v = (w < 0.0f) ? wa - (d1 + d2) : (d1 + d2) - wa;
		if (std::fabs(vmin) > std::fabs(v))
		{
			c = sep;
			vmin = v;
		}
	}

	// 外積分離軸の判定
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			XMFLOAT3 cr = cross(b1.dir[i], b2.dir[j]);
			if (length(cr) <= FLT_EPSILON) continue;
			XMFLOAT3 sep = normalize(cr);
			float d1 = calcLengthProjAxis(sep, b[0], b[1], b[2]);
			float d2 = calcLengthProjAxis(sep, b[3], b[4], b[5]);
			float w = dot(r, sep);
			float wa = std::fabs(w);
			if (wa > d1 + d2) return false;
			float v = (w < 0.0f) ? wa - (d1 + d2) : (d1 + d2) - wa;
			if (std::fabs(vmin) > std::fabs(v))
			{
				c = sep;
				vmin = v;
			}
		}
	}

	if (corr != nullptr) *corr = c;
	if (len != nullptr) *len = vmin;
	return true;
}

// 球と平面の衝突判定
bool detectSphereToPlaneCollision(Sphere& s, Plane& p,
	XMFLOAT3* corr, float* len)
{
	float d = dot(p.norm, s.pos) - p.dist;
	if (std::fabs(d) > s.r) return false;

	if (corr != nullptr) *corr = p.norm;
	if (len != nullptr) *len = s.r - d;
	return true;
}

// 球とOBBの衝突判定
bool detectSphereToOBB3dCollision(Sphere& s, OBB3d& b,
	XMFLOAT3* corr, float* len)
{
	float scale[3] = { b.scale.x, b.scale.y, b.scale.z };
	XMFLOAT3 cp = b.pos;
	XMFLOAT3 d = s.pos - b.pos;
	float w[3] = { 0.0f };
	bool inside = true;

	for (int i = 0; i < 3; ++i)
	{
		w[i] = dot(d, b.dir[i]);
		if (w[i] > scale[i])
		{
			w[i] = scale[i];
			inside = false;
		}
		if (w[i] < -scale[i])
		{
			w[i] = -scale[i];
			inside = false;
		}
		cp += w[i] * b.dir[i];
	}

	XMFLOAT3 c = ZeroVec3d;
	float v = 0.0f;

	if (inside)
	{
		// 球の中心がOBBの内部にある場合
		v = FLT_MAX;
		for (int i = 0; i < 3; ++i)
		{
			float p = scale[i] - std::fabs(w[i]);
			if (v >= p)
			{
				v = p + s.r;
				c = (w[i] >= 0.0f) ? b.dir[i] : -1.0f * b.dir[i];
			}
		}
	}
	else
	{
		// 球の中心がOBBの外部にある場合
		XMFLOAT3 diff = s.pos - cp;
		float diffLen = length(diff);
		if (diffLen > s.r) return false;
		c = normalize(diff);
		v = s.r - diffLen;
	}

	if (corr != nullptr) *corr = c;
	if (len != nullptr) *len = v;
	return true;
}

XMFLOAT3 Ray::calcPositionOnRay(float rho)
{
	return pos + rho * dir;
}

// レイと三角形の衝突判定
bool detectRayToTriangleCollision(Ray& ray, XMFLOAT3& v0, XMFLOAT3& v1,
	XMFLOAT3& v2, float* rho, bool cull, bool tri)
{
	XMFLOAT3 a = v1 - v0;
	XMFLOAT3 b = v2 - v0;
	XMFLOAT3 e = cross(ray.dir, b);

	float det = dot(a, e);
	if (std::fabs(det) < FLT_EPSILON) return false;
	if (cull == true && det <= 0.0f) return false;

	float dr = 1.0f / det;
	XMFLOAT3 d = ray.pos - v0;
	float s = dot(d, e) * dr;
	if (s < 0.0f || s > 1.0f) return false;

	XMFLOAT3 f = cross(d, a);
	float t = dot(ray.dir, f) * dr;
	if (t < 0.0f || t > 1.0f) return false;
	if (tri == true && s + t > 1.0f) return false;

	if (rho != nullptr) *rho = dot(b, f) * dr;
	return true;
}

// レイとAABB3dの衝突判定
bool detectRayToAABB3dCollision(Ray& ray, AABB3d& b,
	float* rho, int* pindex, bool cull)
{
	XMFLOAT3 points[8] = {
		XMFLOAT3(b.curMax.x, b.curMax.y, b.curMax.z),	// 0
		XMFLOAT3(b.curMin.x, b.curMax.y, b.curMax.z),	// 1
		XMFLOAT3(b.curMax.x, b.curMin.y, b.curMax.z),	// 2
		XMFLOAT3(b.curMax.x, b.curMax.y, b.curMin.z),	// 3
		XMFLOAT3(b.curMin.x, b.curMin.y, b.curMax.z),	// 4
		XMFLOAT3(b.curMin.x, b.curMax.y, b.curMin.z),	// 5
		XMFLOAT3(b.curMax.x, b.curMin.y, b.curMin.z),	// 6
		XMFLOAT3(b.curMin.x, b.curMin.y, b.curMin.z)	// 7
	};

	XMFLOAT3 planes[6][3] =
	{
		points[6], points[3], points[2], // X軸正方向
		points[3], points[5], points[0], // Y軸正方向
		points[4], points[2], points[1], // Z軸正方向
		points[5], points[7], points[1], // X軸負方向
		points[7], points[6], points[4], // Y軸負方向		
		points[3], points[7], points[5]  // Z軸負方向
	};

	float rhoMin = FLT_MAX;
	int index = -1;
	bool hit = false;
	for (int i = 0; i < 6; ++i)
	{
		float r = 0.0f;
		if (detectRayToTriangleCollision(ray, planes[i][0],
			planes[i][1], planes[i][2], &r, cull, false))
		{
			if (r > 0.0f && rhoMin > r)
			{
				rhoMin = r;
				index = i;
				hit = true;
			}
		}
	}
	if (hit == false) return false;

	if (rho != nullptr) *rho = rhoMin;
	if (pindex != nullptr) *pindex = index;
	return true;
}

// レイとOBB3dの衝突判定
bool detectRayToOBB3dCollision(Ray& ray, OBB3d& b,
	float* rho, int* pindex, bool cull)
{
	XMFLOAT3 points[8] = {
		b.pos + b.scale.x * b.dir[0] + b.scale.y * b.dir[1] + b.scale.z * b.dir[2],	// 0
		b.pos - b.scale.x * b.dir[0] + b.scale.y * b.dir[1] + b.scale.z * b.dir[2],	// 1
		b.pos + b.scale.x * b.dir[0] - b.scale.y * b.dir[1] + b.scale.z * b.dir[2],	// 2
		b.pos + b.scale.x * b.dir[0] + b.scale.y * b.dir[1] - b.scale.z * b.dir[2],	// 3
		b.pos - b.scale.x * b.dir[0] - b.scale.y * b.dir[1] + b.scale.z * b.dir[2],	// 4
		b.pos - b.scale.x * b.dir[0] + b.scale.y * b.dir[1] - b.scale.z * b.dir[2],	// 5
		b.pos + b.scale.x * b.dir[0] - b.scale.y * b.dir[1] - b.scale.z * b.dir[2],	// 6
		b.pos - b.scale.x * b.dir[0] - b.scale.y * b.dir[1] - b.scale.z * b.dir[2]	// 7
	};

	XMFLOAT3 planes[6][3] =
	{
		points[6], points[3], points[2], // X軸正方向
		points[3], points[5], points[0], // Y軸正方向
		points[4], points[2], points[1], // Z軸正方向
		points[5], points[7], points[1], // X軸負方向
		points[7], points[6], points[4], // Y軸負方向		
		points[3], points[7], points[5]  // Z軸負方向
	};

	float rhoMin = FLT_MAX;
	int index = -1;
	bool hit = false;
	for (int i = 0; i < 6; ++i)
	{
		float r = 0.0f;
		if (detectRayToTriangleCollision(ray, planes[i][0],
			planes[i][1], planes[i][2], &r, cull, false))
		{
			if (r > 0.0f && rhoMin > r)
			{
				rhoMin = r;
				index = i;
				hit = true;
			}
		}
	}
	if (hit == false) return false;

	if (rho != nullptr) *rho = rhoMin;
	if (pindex != nullptr) *pindex = index;
	return true;
}

// レイと平面の衝突判定
bool detectRayToPlaneCollision(Ray& ray, Plane& p, float* rho, bool cull)
{
	float a = dot(ray.dir, p.norm);
	if (cull == true && a > 0.0f) return false;
	if (std::fabs(a) < FLT_EPSILON) return false;

	float r = -(dot(ray.pos, p.norm) + p.dist) / a;
	if (rho != nullptr) *rho = r;
	return true;
}

// レイと有限平面の衝突判定
bool detectRayToFinitePlaneCollision(Ray& ray, FinitePlane& p,
	float* rho, bool cull)
{
	XMFLOAT3 v0 = p.pos - p.scale.x * p.vx + p.scale.y * p.vy;
	XMFLOAT3 v1 = p.pos - p.scale.x * p.vx - p.scale.y * p.vy;
	XMFLOAT3 v2 = p.pos + p.scale.x * p.vx + p.scale.y * p.vy;

	return detectRayToTriangleCollision(ray, v0, v1, v2, rho, cull, false);
}

// レイと球の衝突判定
bool detectRayToSphereCollision(Ray& ray, Sphere& s,
	float* rho, bool inCull)
{
	bool inSphere = detectPointToSphereCollision(ray.pos, s);
	if (inCull == true && inSphere == true) return false;
	if ((inCull == false && inSphere == true)
		&& rho == nullptr) return true;

	XMFLOAT3 d = ray.pos - s.pos;
	float rp = dot(ray.dir, d);
	float pp = dot(d, d);
	float b = 2.0f * rp;
	float dis = b * b - 4.0f * (pp - s.r * s.r);
	if (dis < FLT_EPSILON) return false;

	float q = std::sqrtf(dis);
	if (inSphere == true)
	{
		float r = (-b + q) * 0.5f;
		if (rho != nullptr) *rho = r;
	}
	else
	{
		float r = (-b - q) * 0.5f;
		if (r > 0.0f)
		{
			if (rho != nullptr) *rho = r;
		}
		else
		{
			r = (-b + q) * 0.5f;
			if (rho != nullptr) *rho = r;
		}
	}
	return true;
}

// レイとグリッドメッシュの衝突判定
bool detectRayToGridMeshCollision(Ray& ray, GridHeightMap& map,
	float* rho, bool cull)
{
	if (ray.pos.x < map.minPos.x || ray.pos.x > map.maxPos.x) return false;
	if (ray.pos.y < map.minPos.y || ray.pos.x > map.maxPos.y) return false;

	int x = (int)((ray.pos.x - map.minPos.x) / map.gridWidth.x);
	int y = (int)((ray.pos.y - map.minPos.y) / map.gridWidth.y);

	int index = x + y * map.xNum;
	if (detectRayToTriangleCollision(ray, map.heightMap[index],
		map.heightMap[index + 1], map.heightMap[index + 1 + map.xNum],
		rho, cull, true)) return true;
	if (detectRayToTriangleCollision(ray, map.heightMap[index],
		map.heightMap[index + 1 + map.xNum], map.heightMap[index + map.xNum],
		rho, cull, true)) return true;

	return false;
}

GridHeightMap::GridHeightMap()
	: xNum(5), yNum(5)
	, minPos(-50.0f, -50.0f)
	, maxPos(50.0f, 50.0f)
	, gridWidth(100.0f / ((float)(xNum - 1) - 1.0f), 100.0f / ((float)(yNum - 1) - 1.0f))
	, heightMap(xNum* yNum)
{
	for (int i = 0; i < yNum; ++i)
	{
		for (int j = 0; j < xNum; ++j)
		{
			heightMap[j + i * xNum] = XMFLOAT3(minPos.x + gridWidth.x * j,
				minPos.y + gridWidth.y * i, 0.0f);
		}
	}
}

GridHeightMap::GridHeightMap(float* height, int xnum, int ynum, XMFLOAT2 base,
	float xWidth, float yWidth)
	: xNum(xnum), yNum(ynum)
	, minPos(base), maxPos(base + XMFLOAT2(xWidth, yWidth))
	, gridWidth(xWidth / (float)(xNum - 1), yWidth / (float)(yNum - 1))
	, heightMap(xNum* yNum)
{
	for (int i = 0; i < yNum; ++i)
	{
		for (int j = 0; j < xNum; ++j)
		{
			int index = j + i * xNum;
			double x = (double)minPos.x + (double)(gridWidth.x * (float)j);
			double y = (double)minPos.y + (double)(gridWidth.y * (float)i);
			heightMap[index] = XMFLOAT3(minPos.x + gridWidth.x * j,
				minPos.y + gridWidth.y * i, height[index]);
		}
	}
}

// 法線計算
XMFLOAT3 calcNormVector(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	return normalize(cross(v1, v2));
}

void setContainerToMatrix(const std::vector<float>& v, XMMATRIX& mat)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.r[i].m128_f32[j] = v[j + i * 4];
		}
	}
}

void setContainerToMatrices(const std::vector<float>& v, std::vector<XMMATRIX>& mat)
{
	int index = 0;
	for (size_t i = 0; i < mat.size(); ++i)
	{
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				mat[i].r[y].m128_f32[x] = v[index];
				index += 1;
			}
		}
	}
}

void setContainerToMultiMatrices(const std::vector<float>& v,
	std::vector<std::vector<XMMATRIX>>& mat)
{
	int index = 0;
	for (size_t i = 0; i < mat.size(); ++i)
	{
		for (size_t j = 0; j < mat[i].size(); ++j)
		{
			for (int y = 0; y < 4; ++y)
			{
				for (int x = 0; x < 4; ++x)
				{
					mat[i][j].r[y].m128_f32[x] = v[index];
					index += 1;
				}
			}
		}
	}
}

XMMATRIX calcBillboardMatrix(const XMMATRIX& view, XMFLOAT3 p)
{
	XMMATRIX mat = XMMatrixIdentity();
	XMFLOAT3 r1(view.r[0].m128_f32[0], view.r[0].m128_f32[1], view.r[0].m128_f32[2]);
	XMFLOAT3 r2(view.r[1].m128_f32[0], view.r[1].m128_f32[1], view.r[1].m128_f32[2]);
	XMFLOAT3 r3(view.r[2].m128_f32[0], view.r[2].m128_f32[1], view.r[2].m128_f32[2]);

	mat.r[0].m128_f32[0] = r1.x;	mat.r[0].m128_f32[1] = r2.x;	mat.r[0].m128_f32[2] = r3.x;
	mat.r[1].m128_f32[0] = r1.y;	mat.r[1].m128_f32[1] = r2.y;	mat.r[1].m128_f32[2] = r3.y;
	mat.r[2].m128_f32[0] = r1.z;	mat.r[2].m128_f32[1] = r2.z;	mat.r[2].m128_f32[2] = r3.z;

	mat.r[3].m128_f32[0] = -dot(p, r1) + p.x;
	mat.r[3].m128_f32[1] = -dot(p, r2) + p.y;
	mat.r[3].m128_f32[2] = -dot(p, r3) + p.z;

	return mat;
}

XMMATRIX calcBillboardMatrixYaxis(const XMMATRIX& view, XMFLOAT3 p, XMFLOAT3 camPos)
{
	XMMATRIX mat = XMMatrixIdentity();
	XMFLOAT3 d = p - camPos;
	if (length(d) < FLT_EPSILON) return mat;

	d = normalize(d);
	XMFLOAT3 r1 = normalize(cross(UnitVecZ3d, d));
	XMFLOAT3 r2 = UnitVecZ3d;
	XMFLOAT3 r3 = normalize(cross(r1, r2));

	XMFLOAT3 c1(r1.x, r2.x, r3.x);
	XMFLOAT3 c2(r1.y, r2.y, r3.y);
	XMFLOAT3 c3(r1.z, r2.z, r3.z);

	mat.r[0].m128_f32[0] = r1.x;
	mat.r[0].m128_f32[1] = r1.y;
	mat.r[0].m128_f32[2] = r1.z;

	mat.r[1].m128_f32[0] = r2.x;
	mat.r[1].m128_f32[1] = r2.y;
	mat.r[1].m128_f32[2] = r2.z;

	mat.r[2].m128_f32[0] = r3.x;
	mat.r[2].m128_f32[1] = r3.y;
	mat.r[2].m128_f32[2] = r3.z;

	mat.r[3].m128_f32[0] = -dot(p, c1) + p.x;
	mat.r[3].m128_f32[1] = -dot(p, c2) + p.y;
	mat.r[3].m128_f32[2] = -dot(p, c3) + p.z;

	return mat;
}

AnimMeshConstData::AnimMeshConstData()
	: world(XMMatrixIdentity())
	, scale(Ones3d), pad(0.0f)
	, shadow(XMMatrixIdentity())
{
	for (int i = 0; i < BONE_MAX_NUM; ++i)
	{
		inverseMatrices[i] = XMMatrixIdentity();
		poseMatrices[i] = XMMatrixIdentity();
	}
}

AnimMeshConstData::AnimMeshConstData(XMMATRIX w, MeshMaterial mat)
	: world(w)
	, material(mat)
	, scale(Ones3d), pad(0.0f)
	, shadow(XMMatrixIdentity())
{
	for (int i = 0; i < BONE_MAX_NUM; ++i)
	{
		inverseMatrices[i] = XMMatrixIdentity();
		poseMatrices[i] = XMMatrixIdentity();
	}
}

void AnimMeshConstData::setInverseMatrices(const std::vector<XMMATRIX>& matrices)
{
	for (size_t i = 0; i < matrices.size(); ++i)
	{
		inverseMatrices[i] = matrices[i];
	}
}

void AnimMeshConstData::setPoseMatrices(const std::vector<XMMATRIX>& matrices)
{
	for (size_t i = 0; i < matrices.size(); ++i)
	{
		poseMatrices[i] = matrices[i];
	}
}