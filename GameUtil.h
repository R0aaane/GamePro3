
#pragma once

#include "GMath.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <dxgi.h>

// 頂点データ（uv座標対応）
struct VertexUV
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

// 頂点データ（法線対応）
struct VertexNorm
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 norm;
};

// 頂点データ（法線・接線対応）
struct VertexNormTangent
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 norm;
	XMFLOAT3 tangent;
};

// 頂点データ（アニメーション対応）
#define ANIM_BONE_MAX_NUM (4)
struct VertexAnim
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 norm;
	unsigned short boneID[ANIM_BONE_MAX_NUM];
	float weight[ANIM_BONE_MAX_NUM];
};

// 画素データ（RGBA成分、8bit）
struct ColorRGBA
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	ColorRGBA() : r(0), g(0), b(0), a(255) {}
	ColorRGBA(unsigned char red, unsigned char green, unsigned char blue,
		unsigned char alpha) : r(red), g(green), b(blue), a(alpha) {}
	ColorRGBA(XMFLOAT3 color, float alpha = 1.0f);
};

// カラーベクトルの定数
extern const XMFLOAT3 ColorWhite;
extern const XMFLOAT3 ColorBlack;
extern const XMFLOAT3 ColorGray;
extern const XMFLOAT3 ColorRed;
extern const XMFLOAT3 ColorGreen;
extern const XMFLOAT3 ColorBlue;
extern const XMFLOAT3 ColorYellow;
extern const XMFLOAT3 ColorCyan;
extern const XMFLOAT3 ColorMagenta;

// 円の表現
struct Circle
{
	XMFLOAT2 pos;
	float    radius;

	Circle() : pos(ZeroVec2d), radius(1.0f) {}
	Circle(XMFLOAT2 p, float r) : pos(p), radius(r) {}
};

// 矩形表現
struct Box
{
	XMFLOAT2 pos;
	float width;
	float height;

	Box() : pos(ZeroVec2d), width(1.0f), height(1.0f) {}
	Box(XMFLOAT2 p, float w, float h) : pos(p), width(w), height(h) {}
};

// 線分表現
struct Segment
{
	XMFLOAT2 start;
	XMFLOAT2 end;

	Segment() : start(ZeroVec2d), end(UnitVecX2d) {}
	Segment(const XMFLOAT2& s, const XMFLOAT2& e) : start(s), end(e) {}
};

// 球の表現
struct Sphere
{
	XMFLOAT3 pos;
	float r;

	Sphere() : pos(ZeroVec3d), r(1.0f) {}
	Sphere(const XMFLOAT3& p, float ra) : pos(p), r(ra) {}
};

// 3次元のAABB表現
struct AABB3d
{
	XMFLOAT3 curMax;	XMFLOAT3 curMin;
	XMFLOAT3 oriMax;	XMFLOAT3 oriMin;

	AABB3d();
	AABB3d(std::vector<XMFLOAT3>& points);
	void initialize(std::vector<XMFLOAT3>& points);
	void update(const XMMATRIX& world);
};

// 3次元のOBB表現
struct OBB3d
{
	XMFLOAT3 pos;
	XMFLOAT3 dir[3];
	XMFLOAT3 scale;

	OBB3d();
	OBB3d(XMFLOAT3 p, XMFLOAT3 dx, XMFLOAT3 dy, XMFLOAT3 dz, XMFLOAT3 s);
};

// 平面の表現
struct Plane
{
	XMFLOAT3 norm;
	float dist;

	Plane() : norm(UnitVecZ3d), dist(0.0f) {}
	Plane(XMFLOAT3 n, float d) : norm(n), dist(d) {}
	Plane(XMFLOAT3 n, XMFLOAT3 p) : norm(n), dist(-dot(n, p)) {}
};

// 有限平面の表現
struct FinitePlane
{
	XMFLOAT3 pos;
	XMFLOAT3 vx;
	XMFLOAT3 vy;
	XMFLOAT3 scale;

	FinitePlane() : pos(ZeroVec3d), vx(UnitVecX3d), vy(UnitVecY3d), scale(Ones3d) {}
	FinitePlane(XMFLOAT3 p, XMFLOAT3 x, XMFLOAT3 y, XMFLOAT3 s) : pos(p),
		vx(x), vy(y), scale(s) {
	}
};

// 半直線（レイ）の表現
struct Ray
{
	XMFLOAT3 pos;
	XMFLOAT3 dir;

	Ray() : pos(ZeroVec3d), dir(UnitVecX3d) {}
	Ray(XMFLOAT3 p, XMFLOAT3 d) : pos(p), dir(d) {}
	XMFLOAT3 calcPositionOnRay(float rho);
};

// グリッドハイトマップ
struct GridHeightMap
{
	int xNum;
	int yNum;
	XMFLOAT2 minPos;
	XMFLOAT2 maxPos;
	XMFLOAT2 gridWidth;
	std::vector<XMFLOAT3> heightMap;

	GridHeightMap();
	GridHeightMap(float* height, int xnum, int ynum, XMFLOAT2 base,
		float xWidth, float yWidth);
};

// 点と円の衝突（内外）判定
bool detectPointToCircleCollision(XMFLOAT2& p, Circle& c);

// 点と矩形の衝突（内外）判定
bool detectPointToBoxCollision(XMFLOAT2& p, Box& box);

// 円と円の衝突判定
bool detectCircleCollision(Circle& c1, Circle& c2);

// 矩形と矩形の衝突判定
bool detectBoxCollision(Box& b1, Box& b2);

// 円と矩形の衝突判定
bool detectBoxToCircleCollision(Box& b, Circle& c);

// 円と線分の衝突判定
bool detectCircleToSegmentCollision(Circle& c, Segment& seg);

// 矩形を線分に分割
void separateBoxToSegments(const Box& rect, Segment& left, Segment& right,
	Segment& top, Segment& bottom);

// 点と球の衝突（内外）判定
bool detectPointToSphereCollision(XMFLOAT3& p, const Sphere& sh);

// 球と球の衝突判定
bool detectSphereCollision(Sphere& s1, Sphere& s2,
	XMFLOAT3* corr = nullptr, float* len = nullptr);

// AABB3dとAABB3dの衝突判定
bool detectAABB3dCollision(AABB3d& b1, AABB3d& b2,
	XMFLOAT3* corr = nullptr, float* len = nullptr);

// OBB3dとOBB3dの衝突判定
bool detectOBB3dCollision(OBB3d& b1, OBB3d& b2,
	XMFLOAT3* corr = nullptr, float* len = nullptr);

// 球と平面の衝突判定
bool detectSphereToPlaneCollision(Sphere& s, Plane& p,
	XMFLOAT3* corr = nullptr, float* len = nullptr);

// 球とOBBの衝突判定
bool detectSphereToOBB3dCollision(Sphere& s, OBB3d& b,
	XMFLOAT3* corr = nullptr, float* len = nullptr);

// レイと三角形の衝突判定
bool detectRayToTriangleCollision(Ray& ray, XMFLOAT3& v0, XMFLOAT3& v1,
	XMFLOAT3& v2, float* rho = nullptr, bool cull = true, bool tri = true);

// レイとAABB3dの衝突判定
bool detectRayToAABB3dCollision(Ray& ray, AABB3d& b,
	float* rho = nullptr, int* pindex = nullptr, bool cull = true);

// レイとOBB3dの衝突判定
bool detectRayToOBB3dCollision(Ray& ray, OBB3d& b,
	float* rho = nullptr, int* pindex = nullptr, bool cull = true);

// レイと平面の衝突判定
bool detectRayToPlaneCollision(Ray& ray, Plane& p,
	float* rho = nullptr, bool cull = true);

// レイと有限平面の衝突判定
bool detectRayToFinitePlaneCollision(Ray& ray, FinitePlane& p,
	float* rho = nullptr, bool cull = true);

// レイと球の衝突判定
bool detectRayToSphereCollision(Ray& ray, Sphere& s,
	float* rho = nullptr, bool inCull = true);

// レイとグリッドメッシュの衝突判定
bool detectRayToGridMeshCollision(Ray& ray, GridHeightMap& map,
	float* rho = nullptr, bool cull = true);

// 法線計算
XMFLOAT3 calcNormVector(const XMFLOAT3& v1, const XMFLOAT3& v2);

// 画像情報構造体
struct ImageData
{
	int imgIndex;
	int width;
	int height;
	std::wstring filePath;
	DXGI_FORMAT format;

	ImageData() : imgIndex(-1), width(0), height(0),
		format(DXGI_FORMAT_R8G8B8A8_UNORM){}
};

// スプライト変換データ構造体
struct SpriteTransData
{
	XMMATRIX posMat;
	XMMATRIX uvMat;
	XMFLOAT3 color;
	float    alpha;

	SpriteTransData() 
		: posMat(XMMatrixIdentity()), uvMat(XMMatrixIdentity()),
		  color(Ones3d), alpha(1.0f) {}
	SpriteTransData(XMMATRIX pos, XMMATRIX uv)
		: posMat(pos), uvMat(uv), color(Ones3d), alpha(1.0f) {}
};

// スプライト座標変換行列
XMMATRIX calcSpriteMatrix(float width, float height);

// スプライトモデル変換行列
XMMATRIX calcSpriteModelMatrix(XMFLOAT2 scale,
	XMFLOAT2 pos, XMFLOAT2 offset, float theta);

// スプライトのuv座標変換行列
XMMATRIX calcSpriteUVMatrix(const XMFLOAT2& p1, float uw, float uh,
	float imgW, float imgH);

// スプライトのuv座標変換行列
XMMATRIX calcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2,
	float imgW, float imgH);

// 数値情報を出力ウィンドウに表示
void printNum(const wchar_t* str, int num);

// サウンドデータ
struct SoundData
{
	WAVEFORMATEX format;
	std::vector<BYTE> waveData;
};

// 文字コードのインデックス対応
struct FontIndex
{
	wchar_t start;
	wchar_t end;
	int indexOffset;

	FontIndex(wchar_t s, wchar_t e, int offset) :
		start(s), end(e), indexOffset(offset) {}
	bool isInRange(wchar_t c);
};

// フォントデータ
struct FontData
{
	const wchar_t* fontName;
	std::vector<FontIndex> indices;
	UINT wNum;
	UINT hNum;
	bool ddsFlag;

	FontData() : fontName(nullptr), wNum(0), hNum(0), ddsFlag(false) {}
	FontData(const wchar_t* name, std::vector<FontIndex>& indices,
		UINT w, UINT h, bool flag) :
		fontName(name), indices(indices), wNum(w), hNum(h), ddsFlag(flag) {}
};

// メッシュ情報構造体
struct MeshData
{
	int meshIndex;
	int vertexNum;
	int indexNum;

	MeshData() : meshIndex(-1), vertexNum(0), indexNum(0) {}
	MeshData(int index, int vnum, int inum) : meshIndex(index),
		vertexNum(vnum), indexNum(inum) {}
};

// アニメーションデータ
struct AnimData
{
	unsigned short boneNum;
	unsigned short frameNum;
	float interval;

	AnimData() : boneNum(0), frameNum(0), interval(0.0f) {}
};

// メッシュ・テクスチャ情報構造体
struct BufferData
{
	MeshData mesh;
	ImageData texture;
	XMMATRIX adjust;
	AnimData animData;
	std::vector<XMMATRIX> inverseGPoseMatrices;
	std::vector<std::vector<XMMATRIX>> poseMatrices;

	BufferData(): adjust(XMMatrixIdentity()),
		inverseGPoseMatrices(0), poseMatrices(0) { }
	BufferData(const MeshData& ms, const ImageData& tex, const XMMATRIX& ad)
		: mesh(ms), texture(tex), adjust(ad),
		inverseGPoseMatrices(0), poseMatrices(0) { }
};

// マテリアル設定
struct MeshMaterial
{
	XMFLOAT3 diffuse;
	float padA;
	XMFLOAT3 specular;
	float power;
	XMFLOAT3 ambient;
	float padB;

	MeshMaterial();
	MeshMaterial(const XMFLOAT3& diff,
		const XMFLOAT3& spec, float p, const XMFLOAT3& amb);
};

// メッシュ用の定数バッファデータ
struct MeshConstData
{
	XMMATRIX world;
	XMMATRIX uvTrans;
	XMFLOAT3 scale;
	float    pad;
	MeshMaterial material;
	XMMATRIX shadow;

	MeshConstData();
	MeshConstData(const XMMATRIX& w, const XMMATRIX& uv, const XMFLOAT3& s,
		const MeshMaterial& m);
};

// アニメーションメッシュ用の定数バッファデータ
#define BONE_MAX_NUM (256)
struct AnimMeshConstData
{
	XMMATRIX     world;
	XMFLOAT3     scale;
	float        pad;
	MeshMaterial material;
	XMMATRIX inverseMatrices[BONE_MAX_NUM];
	XMMATRIX poseMatrices[BONE_MAX_NUM];
	XMMATRIX shadow;

	AnimMeshConstData();
	AnimMeshConstData(XMMATRIX w, MeshMaterial mat);
	void setWorld(XMMATRIX w) { world = w; }
	void setMaterial(MeshMaterial mat) { material = mat; }
	void setInverseMatrices(const std::vector<XMMATRIX>& matrices);
	void setPoseMatrices(const std::vector<XMMATRIX>& matrices);
};

struct ParallelLight
{
	XMFLOAT3 direction;
	float padA;
	XMFLOAT3 color;
	float padB;

	ParallelLight();
	ParallelLight(XMFLOAT3 dir, XMFLOAT3 col);
};

struct PointLight
{
	XMFLOAT3 pos;
	float pad;
	XMFLOAT3 color;
	float radius;

	PointLight();
	PointLight(const XMFLOAT3& p, const XMFLOAT3 col, float r);
};

struct SpotLight
{
	XMFLOAT3 pos;
	float radius;
	XMFLOAT3 dir;
	float angle;
	XMFLOAT3 color;
	float pad;

	SpotLight();
	SpotLight(const XMFLOAT3& p, float r, const XMFLOAT3& d,
		float a, const XMFLOAT3& col);
};

// シーン共通データ
const int MaxPointLightNum = 10;
const int MaxSpotLightNum = 3;
struct SceneData
{
	XMMATRIX viewProj;
	XMFLOAT3 eye;
	float padA;
	ParallelLight parallelLight;
	PointLight pointLight[MaxPointLightNum];
	SpotLight spotLight[MaxSpotLightNum];
	XMFLOAT3 ambientLight;
	float padB;

	SceneData();
};

XMMATRIX calcMeshUVMatrix(const XMFLOAT2& scale, const XMFLOAT2& pos);
void setContainerToMatrix(const std::vector<float>& v, XMMATRIX& mat);
void setContainerToMatrices(const std::vector<float>& v, std::vector<XMMATRIX>& mat);
void setContainerToMultiMatrices(const std::vector<float>& v,
	std::vector<std::vector<XMMATRIX>>& mat);

XMMATRIX calcBillboardMatrix(const XMMATRIX& view, XMFLOAT3 p);
XMMATRIX calcBillboardMatrixYaxis(const XMMATRIX& view, XMFLOAT3 p, XMFLOAT3 camPos);

