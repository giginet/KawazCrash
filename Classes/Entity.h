//
//  Entity.h
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#ifndef __KawazCrash__Entity__
#define __KawazCrash__Entity__

#include "cocos2d.h"

const int ENTITY_SIZE = 33;

class Entity :public cocos2d::Sprite
{
public:
    
    virtual ~Entity();

    enum class Color {
        RED,
        YELLOW,
        GREEN,
        LIGHTBLUE,
        BLUE
    };

    static int getSize() {
        return ENTITY_SIZE;
    }
    bool init() override;
    
    /** ブロックのグリッド上の位置を指定します
    * @param position x, y位置を含んだ二次元ベクトル
    */
    void setEntityPosition(cocos2d::Vec2 position);
    
    /** ブロックのキーを取り出します
     *  キーはグリッド上の位置を,で繋いだものです
     *  @return キー
     */
    std::string getKey();
    
    void adjustPosition();
    
    CC_SYNTHESIZE(bool, _isFalling, IsFalling);
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, _entityPosition, EntityPosition);
    CC_SYNTHESIZE(Color, _entityColor, EntityColor);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _debugLabel, DebugLabel);
    CREATE_FUNC(Entity);
};

#endif /* defined(__KawazCrash__Entity__) */
