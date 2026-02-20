#include "Scene.h"
#include "Actor.h"
#include "SpriteActor.h"
#include "BlockSpriteActor.h"
#include "BlockAnimActor.h"

class MyGameScene : public Scene
{
public:
	MyGameScene(class Game* game);
	~MyGameScene();

	void update(float deltaTime) override;
	void draw() override;

private:
	std::unique_ptr<SpriteActor> m_sprite;
	std::unique_ptr<BlockSpriteActor> m_blockSprite;
	std::unique_ptr<BlockAnimActor> m_AnimSprite;
};






















