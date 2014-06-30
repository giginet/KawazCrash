//
//  Cookie.h
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#ifndef __KawazCrash__Cookie__
#define __KawazCrash__Cookie__

#include "cocos2d.h"

const int ENTITY_SIZE = 36;

class Cookie :public cocos2d::Sprite
{
public:
    
    Cookie();
    virtual ~Cookie();

    enum class Color {
        BROWN,
        RED,
        YELLOW,
        GREEN,
        LIGHTBLUE,
        BLUE,
        PINK
    };
    
    enum class State {
        NORMAL,
        APPEARING,
        FALLING,
        SWAPPING,
        DISAPEARING
    };

    static int getSize() {
        return ENTITY_SIZE;
    }
    bool init() override;
    
    /** ブロックのグリッド上の位置を指定します
    * @param position x, y位置を含んだ二次元ベクトル
    */
    void setCookiePosition(cocos2d::Vec2 position);
    
    /** ブロックのキーを取り出します
     *  キーはグリッド上の位置を,で繋いだものです
     *  @return キー
     */
    std::string getKey();
    
    void adjustPosition();
    
    bool isNormal() {
        return _state == State::NORMAL;
    }
    
    
    CC_SYNTHESIZE(State, _state, State);
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, _cookiePosition, CookiePosition);
    CC_SYNTHESIZE_READONLY(Color, _cookieColor, CookieColor);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _debugLabel, DebugLabel);
    CREATE_FUNC(Cookie);
};

#endif /* defined(__KawazCrash__Cookie__) */
