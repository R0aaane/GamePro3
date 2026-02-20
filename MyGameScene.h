#include "Scene.h"
#include "Actor.h"
#include "SpriteActor.h"
#include "BlockSpriteActor.h"
#include "BlockAnimActor.h"
#include "PlayerActor2D.h"
#include "ItemActor.h"
#include "EmitterActor.h"
#include "StringActor.h"

class MyGameScene : public Scene
{
public:
	MyGameScene(class Game* game);
	~MyGameScene();

	void update(float deltaTime) override;
	void draw() override;

	std::vector<Actor*>& getItems() { return m_items; }
	void addItem(ItemActor* item) { m_itemsTemp.push_back(item); }

private:
	std::unique_ptr<SpriteActor> m_bg;
	std::unique_ptr<SpriteActor> m_sprite;
	std::unique_ptr<BlockSpriteActor> m_blockSprite;
	std::unique_ptr<BlockAnimActor> m_AnimSprite;
	std::unique_ptr<PlayerActor2D> m_balloon;
	std::unique_ptr<PlayerActor2D> m_bomb;   
	std::unique_ptr<EmitterActor> m_emitter;

	std::vector<Actor*> m_items;

	// ˆêŽž‘Þ”ð—p‚ÌTemp
	std::vector<Actor*> m_itemsTemp;

	std::unique_ptr<StringActor> m_stringNum;
	FontData m_font;

};
