//
//  MainScene.h
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#ifndef __KawazCrash__MainScene__
#define __KawazCrash__MainScene__

#include "cocos2d.h"
#include "Entity.h"

typedef cocos2d::Map<std::string, Entity *> EntityMap;

class MainScene :public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    bool init() override;

    /** 指定したブロックを追加します
     *  @param 追加するEntity
     */
    void addEntity(Entity *entity);
    
    /** グリッド上の特定位置にあるブロックを取り出します
    *   何もなかった場合はnullptrを返します
    *   @param x x座標
    *   @param y y座標
    *   @return その位置にあるEntity、またはnullptr
    */
    Entity* getEntityAt(int x, int y);

    /** 画面上の特定位置にあるブロックを取り出します
     *   何もなかった場合はnullptrを返します
     *   @param position 画面上の絶対座標
     *   @return その位置にあるEntity、またはnullptr
     */
    Entity* getEntityAt(cocos2d::Vec2 position);
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE(EntityMap, _entitys, Entitys);
};

#endif /* defined(__KawazCrash__MainScene__) */
