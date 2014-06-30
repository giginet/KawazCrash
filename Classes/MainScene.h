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
#include "Cookie.h"

typedef cocos2d::Vector<Cookie *> CookieVector;

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
     *  @param 追加するCookie
     */
    void addCookie(Cookie *cookie);
    
    /** グリッド上の特定位置にあるブロックを取り出します
    *   何もなかった場合はnullptrを返します
    *   @param x x座標
    *   @param y y座標
    *   @return その位置にあるCookie、またはnullptr
    */
    Cookie* getCookieAt(int x, int y);

    /** 画面上の特定位置にあるブロックを取り出します
     *  何もなかった場合はnullptrを返します
     *  @param position 画面上の絶対座標
     *  @return その位置にあるCookie、またはnullptr
     */
    Cookie* getCookieAt(cocos2d::Vec2 position);
    
    /** ブロックをグリッド上の指定した位置に動かします
     *  @param cookie0 動かすブロック
     *  @param cookiePosition 動かすグリッド上の座標
     *  @return 動かせたかどうか
     */
    bool moveCookie(Cookie* cookie0, cocos2d::Vec2 cookiePosition);
    
    /** 2つのCookieを取り替えます
     *  @param cookie0 1つめのEnitiy
     *  @param cookie1 2つめのCookie
     *  @return 取り替えられたかどうか
     */
    bool swapEntities(Cookie* cookie0, Cookie* cookie1);
    
    /** 渡されたCookieを消去します
     *  @param cookie 消すCookie
     */
    void deleteCookie(Cookie* cookie);
    
    
    /** 渡されたCookieと隣接するCookieを全て取り出します
     *  @param cookie 開始するCookie
     *  @param checked すでに調査済みのCookieの一覧。ここから調査する場合は空のVectorを渡してください
     *  @return 自身を含む隣接したCookieが含まれたVector
     */
    CookieVector checkNeighborEntities(Cookie* cookie, CookieVector checked);
    
    void checkFall(Cookie *cookie);
    
    bool checkVanishEntities(Cookie* cookie);
    
    cocos2d::Vector<Cookie*> spawnEntities();
    
    /** 次のターンに渡されたブロックが消去可能かを判定します
     *  @param Cookie* cookie
     *  @return 消去可能かどうか
     */
    bool canVanishNext(Cookie *cookie);
    
    void checkField();
    
    /** 全てのCookieがNormal状態かどうか
     *  @return Normal状態かどうか
     */
    bool isAllNormal();
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE(CookieVector, _cookies, Cookies);
    CC_SYNTHESIZE_RETAIN(Cookie *, _currentCookie, CurrentCookie);
};

#endif /* defined(__KawazCrash__MainScene__) */
