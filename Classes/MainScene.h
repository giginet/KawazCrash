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
#include "cri_adx2le.h"
#include "ui/UITextAtlas.h"
#include "cocostudio/CCActionManagerEx.h"

#include "Cookie.h"
#include "ADX2Manager.h"

typedef cocos2d::Vector<Cookie *> CookieVector;

class MainScene :public cocos2d::Layer
{
protected:
    bool init() override;
    
    MainScene();
    
    virtual ~MainScene();
public:
    
    static cocos2d::Scene* createScene();
    
    void update(float dt) override;
    
    void onEnterTransitionDidFinish() override;
    
    enum class State {
        Ready,
        Main,
        Result
    };
    
private:
    /** 指定したクッキーをフィールドに追加します
     *  @param 追加するCookie
     */
    void addCookie(Cookie *cookie);
    
    /** グリッド上の特定位置にあるクッキーを取り出します
    *   何もなかった場合はnullptrを返します
     *  @param position グリッド上のクッキー位置
     *  @return その位置にあるCookie、またはnullptr
    */
    Cookie* getCookieAt(const cocos2d::Vec2& position);

    /** 画面上の特定位置にあるクッキーを取り出します
     *  何もなかった場合はnullptrを返します
     *  @param position 画面上の絶対座標
     *  @return その位置にあるCookie、またはnullptr
     */
    Cookie* getCookieAtByWorld(const cocos2d::Vec2& worldPosition);
    
    /** クッキーをグリッド上の指定した位置に動かします
     *  @param cookie0 動かすブロック
     *  @param cookiePosition 動かすグリッド上の座標
     */
    void moveCookie(Cookie* cookie0, const cocos2d::Vec2& cookiePosition);
    
    /** 2つのクッキーを取り替えます
     *  @param cookie0 1つめのCookie
     *  @param cookie1 2つめのCookie
     *  @return 取り替えられたかどうか
     */
    bool swapCookies(Cookie* cookie0, Cookie* cookie1);
    
    /** 渡されたクッキーをフィールド上から消去します
     *  消去時にはエフェクトも再生されます
     *  @param cookie 消すクッキー
     */
    void vanishCookie(Cookie* cookie);
    
    /** 渡されたクッキーと隣接する同種のクッキーを全て取り出します
     *  @param cookie 探索開始するクッキー
     *  @param checked すでに調査済みのクッキーの一覧。ここから調査する場合は空のVectorを渡してください
     *  @return 自身を含む同種の隣接したクッキーが含まれたVector
     */
    CookieVector checkNeighborCookies(Cookie* cookie);
    CookieVector checkNeighborCookies(Cookie* cookie, CookieVector checked);
    
    /** 渡されたクッキーが落ちるかどうかを判定し、落ちる場合は落下させます
     *  @param cookie チェックするクッキー
     *  @return 落ちたかどうか
     */
    bool fallCookie(Cookie *cookie);
    
    /** ステージ上のクッキーが消えるかどうかを判定し、消える場合は消去します
     *  @return 消えたかどうか
     */
    bool checkVanish();
    
    /** ステージをチェックして出現できる場所にクッキーを出現させ、出現したクッキーの一覧を返します
     *  出現しなかった場合、空のベクターを返します
     *  @return 出現したクッキーの一覧
     */
    cocos2d::Vector<Cookie*> checkSpawn();
    
    /** 渡されたブロックが次のターンに消去できる可能性があるかを判定します
     *  @param Cookie* cookie
     *  @return 消去可能かどうか
     */
    bool canVanishNext(Cookie *cookie);
    
    /** 全てのクッキーが停止中かどうかをチェックして返します
     *  @return 停止中かどうか
     */
    bool isAllStatic();
    
    /** 指定されたクッキーの位置にチェインカウントを表示します
     *  @param cookie クッキー
     *  @param comboCount コンボ数
     */
    void showChainCount(Cookie *cookie, int chainCount);
    
    static CriAtomExPlaybackId _musicId;
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE(int, _chainCount, ChainCount);
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE_PASS_BY_REF(CookieVector, _cookies, Cookies);
    CC_SYNTHESIZE(State, _state, State);
    CC_SYNTHESIZE_RETAIN(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE_RETAIN(Cookie *, _currentCookie, CurrentCookie);
    CC_SYNTHESIZE_RETAIN(cocos2d::ui::TextAtlas *, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::ui::TextAtlas *, _secondLabel, SecondLabel);
};

#endif /* defined(__KawazCrash__MainScene__) */
