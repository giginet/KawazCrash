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
protected:    
    Cookie();
    virtual ~Cookie();
    bool init() override;

public:
    
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
        /// 停止中
        STATIC,
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
    
    /** ステージ上の座標をグリッド上の位置に変換します
     *  @param stagePosition ステージ上の座標
     *  @return グリッド上の位置
     */
    static cocos2d::Vec2 convertToGridSpace(const cocos2d::Vec2& stagePosition);
    
    /** グリッド上の位置をステージ上の座標に変換します
     *  @param gridPosition グリッド上の位置
     *  @return ステージ上の座標
     */
    static cocos2d::Vec2 convertToStageSpace(const cocos2d::Vec2& gridPosition);
    
    /** クッキーのグリッド上の位置を指定します
    * @param position x, y位置を含んだ二次元ベクトル
    */
    void setCookiePosition(const cocos2d::Vec2& position);
    
    /** クッキーの位置を文字列で取り出します
     *  @return x座標とy座標を,で繋いだ文字列
     */
    std::string getDescription();
    
    /** クッキーの位置を、現在のグリッド上の位置に合わせて調整します
     */
    void adjustPosition();
    
    /** クッキーが停止状態かどうかを返します
     *  @return 停止状態かどうか
     */
    bool isStatic() {
        return _state == State::STATIC;
    }
    
    CC_SYNTHESIZE(State, _state, State);
    CC_SYNTHESIZE_READONLY_PASS_BY_REF(cocos2d::Vec2, _cookiePosition, CookiePosition);
    CC_SYNTHESIZE_READONLY(Shape, _cookieShape, CookieShape);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _debugLabel, DebugLabel);
    CREATE_FUNC(Cookie);
};

#endif /* defined(__KawazCrash__Cookie__) */
