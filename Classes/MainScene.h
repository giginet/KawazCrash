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
    
    void update(float dt) override;
    
private:
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
    
    /** 渡されたEntityを消去します
     *  @param entity 消すEntity
     */
    void deleteEntity(Entity* entity);
    
    
    /** 渡されたEntityと隣接するEntityを全て取り出します
     *  @param entity 開始するEntity
     *  @param checked すでに調査済みのEntityの一覧。ここから調査する場合は空のVectorを渡してください
     *  @return 自身を含む隣接したEntityが含まれたVector
     */
    EntityVector checkNeighborEntities(Entity* entity, EntityVector checked);
    
    void checkFall(Entity *entity);
    
    bool checkVanishEntities(Entity* entity);
    
    cocos2d::Vector<Entity*> spawnEntities();
    
    /** 次のターンに渡されたブロックが消去可能かを判定します
     *  @param Entity* entity
     *  @return 消去可能かどうか
     */
    bool canVanishNext(Entity *entity);
    
    void checkField();
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE(EntityVector, _entities, Entitys);
    CC_SYNTHESIZE_RETAIN(Entity *, _currentEntity, CurrentEntity);
};

#endif /* defined(__KawazCrash__MainScene__) */
