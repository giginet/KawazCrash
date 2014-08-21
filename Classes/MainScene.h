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

#include "Cookie.h"
#include "ADX2Manager.h"

typedef cocos2d::Vector<Cookie *> CookieVector;

class MainScene :public cocos2d::Layer
{
public:
    
    MainScene();
    
    virtual ~MainScene();
    
    static cocos2d::Scene* createScene();
    
    void update(float dt) override;
    
    void onEnterTransitionDidFinish() override;
    
    enum class State {
        Ready,
        Main,
        Result
    };
    
private:
    bool init() override;

    /** 指定したクッキーをフィールドに追加します
     *  @param 追加するCookie
     */
    void addCookie(Cookie *cookie);
    
    /** グリッド上の特定位置にあるクッキーを取り出します
    *   何もなかった場合はnullptrを返します
     *  @param position グリッド上のクッキー位置
     *  @return その位置にあるCookie、またはnullptr
    */
    Cookie* getCookieAt(cocos2d::Vec2 position);

    /** 画面上の特定位置にあるクッキーを取り出します
     *  何もなかった場合はnullptrを返します
     *  @param position 画面上の絶対座標
     *  @return その位置にあるCookie、またはnullptr
     */
    Cookie* getCookieAtByWorld(cocos2d::Vec2 worldPosition);
    
    /** クッキーをグリッド上の指定した位置に動かします
     *  @param cookie0 動かすブロック
     *  @param cookiePosition 動かすグリッド上の座標
     */
    void moveCookie(Cookie* cookie0, cocos2d::Vec2 cookiePosition);
    
    /** 2つのクッキーを取り替えます
     *  @param cookie0 1つめのCookie
     *  @param cookie1 2つめのCookie
     *  @return 取り替えられたかどうか
     */
    void swapCookies(Cookie* cookie0, Cookie* cookie1);
    
    /** 渡されたクッキーをフィールド上から消去します
     *  消去時にはエフェクトも再生されます
     *  @param cookie 消すCookie
     */
    void deleteCookie(Cookie* cookie);
    
    
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
    
    /** 渡されたクッキーが消えるかどうかを判定し、消える場合は消去します
     *  @param cookie チェックするクッキーの一覧
     *  @return 消えたかどうか
     */
    bool vanishCookies(CookieVector cookies);
    
    cocos2d::Vector<Cookie*> spawnCookies();
    
    /** 渡されたブロックが次のターンに消去できる可能性があるかを判定します
     *  @param Cookie* cookie
     *  @return 消去可能かどうか
     */
    bool canVanishNext(Cookie *cookie);
    
    /** フィールド全体を更新します。具体的に以下のことをします
     *  - 落ちるはずなのに落ちてないクッキーを落とします
     *  - 次のクッキーが出現できそうなら出現させます
     *  - 既に消える状態になっているクッキーの消去を開始します
     *  - 次のターンにどこを動かしても消せなくなったときに、フィールドをリセットします
     */
    void updateField();
    
    /** 全てのクッキーがNormal状態かどうかをチェックして返します
     *  @return Normal状態かどうか
     */
    bool isAllStatic();
    
    /** 指定されたクッキーの位置にチェインカウントを表示します
     *  @param cookie クッキー
     *  @param comboCount コンボ数
     */
    void showChainCount(Cookie *cookie, int comboCount);
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE(int, _comboCount, ComboCount);
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE_PASS_BY_REF(CookieVector, _cookies, Cookies);
    CC_SYNTHESIZE(State, _state, State);
    CC_SYNTHESIZE_RETAIN(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE_RETAIN(Cookie *, _currentCookie, CurrentCookie);
    CC_SYNTHESIZE_RETAIN(ADX2::Cue *, _cue, Cue);
    CC_SYNTHESIZE_RETAIN(cocos2d::ui::TextAtlas *, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::ui::TextAtlas *, _secondLabel, SecondLabel);
};

#endif /* defined(__KawazCrash__MainScene__) */
