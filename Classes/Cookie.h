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

/// クッキーの1辺の大きさ
const int COOKIE_SIZE = 36;

/// クッキー
class Cookie :public cocos2d::Sprite
{
public:
    
    Cookie();
    virtual ~Cookie();

    /// クッキーの形
    enum class Shape {
        /// 四角
        SQUARE,
        /// 多角形
        POLYGON,
        /// 星形
        STAR,
        /// 丸形
        CIRCLE,
        /// ハート型
        HEART,
        /// 総数
        COUNT
    };
    
    /// クッキーの状態
    enum class State {
        /// 通常
        NORMAL,
        /// 出現中
        APPEARING,
        /// 落下中
        FALLING,
        /// 入れ替え中
        SWAPPING,
        /// 消去中
        DISAPEARING
    };

    /** クッキーの一辺の長さを取り出します
     */
    static int getSize() {
        return COOKIE_SIZE;
    }
    
    bool init() override;
    
    /** クッキーのグリッド上の位置を指定します
    * @param position x, y位置を含んだ二次元ベクトル
    */
    void setCookiePosition(cocos2d::Vec2 position);
    
    /** クッキーの位置を文字列で取り出します
     *  @return x座標とy座標を,で繋いだ文字列
     */
    std::string getDescription();
    
    /** クッキーの位置を、現在のグリッド上の位置に合わせて調整します
     */
    void adjustPosition();
    
    /** クッキーが通常状態かどうかを返します
     *  @return 通常状態かどうか
     */
    bool isNormal() {
        return _state == State::NORMAL;
    }
    
    CC_SYNTHESIZE(State, _state, State);
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, _cookiePosition, CookiePosition);
    CC_SYNTHESIZE_READONLY(Shape, _cookieShape, CookieShape);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _debugLabel, DebugLabel);
    CREATE_FUNC(Cookie);
};

#endif /* defined(__KawazCrash__Cookie__) */
