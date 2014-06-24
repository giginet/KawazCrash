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

typedef cocos2d::Vector<Entity *> EntityVector;

class MainScene :public cocos2d::Layer
{
public:
    
    MainScene();
    
    virtual ~MainScene();
    
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
     *  何もなかった場合はnullptrを返します
     *  @param position 画面上の絶対座標
     *  @return その位置にあるEntity、またはnullptr
     */
    Entity* getEntityAt(cocos2d::Vec2 position);
    
    /** ブロックをグリッド上の指定した位置に動かします
     *  @param entity0 動かすブロック
     *  @param entityPosition 動かすグリッド上の座標
     *  @return 動かせたかどうか
     */
    bool moveEntity(Entity* entity0, cocos2d::Vec2 entityPosition);
    
    /** 2つのEntityを取り替えます
     *  @param entity0 1つめのEnitiy
     *  @param entity1 2つめのEntity
     *  @return 取り替えられたかどうか
     */
    bool swapEntities(Entity* entity0, Entity* entity1);
    
    EntityVector checkNeighborEntitied(Entity* entity, EntityVector checked);
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE(EntityVector, _entitys, Entitys);
    CC_SYNTHESIZE_RETAIN(Entity *, _currentEntity, CurrentEntity);
};

#endif /* defined(__KawazCrash__MainScene__) */
