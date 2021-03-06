//
//  MainScene.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "MainScene.h"
#include "Cookie.h"
#include "SS5Player.h"
#include <algorithm>
#include "Cocostudio/cocostudio.h"

#include "cookie_main.h"
#include "cookie_crush_acf.h"

#include "TitleScene.h"

#include "ADX2Manager.h"

#include "SharedCueSheet.h"

enum ZOrder  {
    ZOrderStage,
    ZOrderEffect,
    ZOrderResult,
    ZOrderUI
};

USING_NS_CC;

CriAtomExPlaybackId MainScene::_musicId = 0;

/// ステージの横のクッキーの数
const int HORIZONTAL_COUNT = 6;
/// ステージの縦のクッキーの数
const int VERTICAL_COUNT = 8;
/// クッキーが消える個数
const int VANISH_COUNT = 4;
/// Stage用のNodeのタグ
const int FRAME_TAG = 1000;
/// 初期残り時間
const int LIMIT_TIME = 60;

MainScene::MainScene()
: _state(State::Ready)
,_second(LIMIT_TIME)
,_score(0)
,_chainCount(0)
,_stage(nullptr)
, _currentCookie(nullptr)
,_scoreLabel(nullptr)
,_secondLabel(nullptr)
{

}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_stage);
    CC_SAFE_RELEASE_NULL(_currentCookie);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
}

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto main = MainScene::create();
    scene->addChild(main);
    return scene;
}

bool MainScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    return true;
}

void MainScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    
    // なぜかinitではなく、遷移後に画面を作らないと_scoreLabelの表示位置が変わる謎バグ
    
    // 画面サイズの取得
    auto winSize = Director::getInstance()->getWinSize();
    
    // シーンファイルを読み込み
    auto node = cocostudio::SceneReader::getInstance()->createNodeWithSceneFile("MainScene.json");
    // シーンファイルの高さ
    const auto sceneHeight = 568;
    // 3.5インチ環境で下の部分を隠すために画面の高さによって位置を変えている
    node->setPosition(Vec2(0, -(sceneHeight - winSize.height)));
    this->addChild(node);
    
    this->setStage(Node::create());
    
    auto frame = node->getChildByTag(FRAME_TAG);
    _stage->setPosition(-Vec2(Cookie::getSize() * HORIZONTAL_COUNT / 2,
                              Cookie::getSize() * VERTICAL_COUNT / 2));
    frame->addChild(_stage, ZOrderStage);
    
    // タッチイベントの登録
    auto listener = EventListenerTouchOneByOne::create();
    // タッチしたとき
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto position = touch->getLocation();
        // 現在のタッチ位置にあるクッキーを取り出す
        auto cookie = this->getCookieAtByWorld(position);
        
        // 現在移動中のクッキーとして設定
        this->setCurrentCookie(cookie);
        return true;
    };
    // タッチ中に動かしたとき
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        // 移動先のクッキーを取得
        auto nextCookie = this->getCookieAtByWorld(touch->getLocation());
        
        // Main以外では何もしない
        if (_state != State::Main) return;
        
        // 元のクッキー、移動後のクッキーが共に存在していて、違った場合、移動させる
        if (_currentCookie != nullptr &&
            nextCookie != nullptr &&
            _currentCookie != nextCookie) {
            
            // 現在の選択しているクッキーと、移動先のクッキーが共に動いていない状態であるとき
            if (_currentCookie->isStatic() && nextCookie->isStatic()) {
                auto cp = _currentCookie->getCookiePosition();
                auto np = nextCookie->getCookiePosition();
                
                // 移動先のクッキーが上下左右いずれかにあるとき
                // すなわち、距離の2乗が1のとき
                if (cp.distanceSquared(np) == 1) {
                    // 2枚のクッキーを入れ替える
                    this->swapCookies(_currentCookie, nextCookie);
                    
                    // 現在選択中のクッキーを外す
                    this->setCurrentCookie(nullptr);
                }
            }
        }
    };
    // タッチを離したとき、キャンセルされた時
    listener->onTouchEnded = listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        // 現在選択中のクッキーを外す
        this->setCurrentCookie(nullptr);
    };
    // イベントリスナーを追加
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdate();
    
    auto secondLabel = node->getChildByTag(3000)->getChildren().at(0)->getChildByName<ui::TextAtlas *>("AtlasLabel_3");
    this->setSecondLabel(dynamic_cast<ui::TextAtlas *>(secondLabel));
    secondLabel->setString(StringUtils::toString(LIMIT_TIME));
    
    auto scoreLabel = node->getChildByTag(2000)->getChildren().at(0)->getChildByName<ui::TextAtlas *>("AtlasLabel_5");
    this->setScoreLabel(dynamic_cast<ui::TextAtlas *>(scoreLabel));
    
    // アニメーションの作成
    auto kawaztan = ss::ResourceManager::getInstance();
    kawaztan->addData("kawaz.ssbp");
    
    auto player = ss::Player::create();
    // ssbpファイル名（拡張子不要）
    player->setData("kawaz");
    // アニメーション名を指定
    player->play("walking");
    auto baseScale = 0.1;
    auto duration = 10;
    auto positionY = winSize.height - 70;
    player->setScale(baseScale);
    auto left = Vec2(0, positionY);
    auto right = Vec2(winSize.width, positionY);
    player->setPosition(right);
    auto walking = Sequence::create(MoveTo::create(duration, left),
                                    ScaleTo::create(0, -baseScale, baseScale),
                                    MoveTo::create(duration, right),
                                    ScaleTo::create(0, baseScale), NULL);
    player->runAction(RepeatForever::create(walking));
    player->setName("kawaztan");
    this->addChild(player, ZOrderEffect);
    
    if (_musicId == 0) {
        _musicId = SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_BGM);
    }
    
    this->runAction(Sequence::create(DelayTime::create(2.0f),
                                     CallFunc::create([this]() {
        auto gamestart = Sprite::create("gamestart.png");
        auto winSize = Director::getInstance()->getWinSize();
        gamestart->setPosition(Vec2(winSize.width / 2.0, winSize.height / 1.5));
        gamestart->setScale(0);
        gamestart->runAction(Sequence::create(EaseElasticIn::create(ScaleTo::create(0.5, 1.0)),
                                              DelayTime::create(1.5),
                                              MoveBy::create(0.5, Vec2(0, winSize.height / 2.0)),
                                              RemoveSelf::create(),
                                              NULL));
        SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_START);
        this->addChild(gamestart, ZOrderUI);
        setState(State::Main);
    }),
                                     NULL));
}

void MainScene::update(float dt)
{
    // ADX2を更新する
    ADX2::Manager::getInstance()->update();
    
    // 全クッキーに対して落下を判定する
    for (auto cookie : _cookies) {
        this->fallCookie(cookie);
    }
    
    if (_state == State::Main) {
        
        // クッキーの生成
        this->checkSpawn();
        
        // 全てのクッキーが停止しているとき
        if (this->isAllStatic()) {
            // 既に揃ってるのを消す
            auto vanished = this->checkVanish();
            // 消えた場合、音を鳴らす
            if (vanished) {
                float gameVariable = _chainCount * 0.125;
                gameVariable = MIN(1.0, gameVariable);
                criAtomEx_SetGameVariableByName("ComboCount", gameVariable);
                SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_VANISH);
            }
            
            auto canVanish = false;
            // 次にどれも消えなさそうだったらランダムに2列消す
            for (Cookie * cookie : _cookies) {
                if (cookie && this->canVanishNext(cookie)) {
                    // どれか消えそうなら探索を打ち切る
                    canVanish = true;
                    break;
                }
            }
            if (!canVanish) {
                // もしどれも消えなかったとき、ランダムに2列を選んで消去する
                auto baseX = rand() % HORIZONTAL_COUNT;
                auto otherX = (rand() % (HORIZONTAL_COUNT - 1) + baseX) % HORIZONTAL_COUNT;
                for (int y = 0; y < VERTICAL_COUNT; ++y) {
                    this->vanishCookie(this->getCookieAt(Vec2(baseX, y)));
                    this->vanishCookie(this->getCookieAt(Vec2(otherX, y)));
                }
            }
        }
        
        // 残り時間の更新
        _second -= dt;
        
        if (_second <= 0) {
            _second = 0;
            setState(State::Result);
            auto timeup = Sprite::create("timeup.png");
            auto winSize = Director::getInstance()->getWinSize();
            timeup->setPosition(Vec2(winSize.width / 2.0, winSize.height / 1.5));
            timeup->setScale(0);
            timeup->runAction(Sequence::create(EaseElasticIn::create(ScaleTo::create(0.5, 1.0)),
                                                  DelayTime::create(1.5),
                                                  ScaleTo::create(0.5, 0),
                                                  RemoveSelf::create(),
                                                  NULL));
            this->addChild(timeup, ZOrderUI);
            
            auto layer = LayerColor::create(Color4B(0, 0, 0, 127));
            layer->setScale(0);
            layer->runAction(ScaleTo::create(0.1, 1.0));
            this->addChild(layer, ZOrderResult);
            
            _secondLabel->setString("0");
            SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_END);
        
            
            // 終了時にメニューを表示する
            this->runAction(Sequence::create(DelayTime::create(2.0),
                                             CallFunc::create([this, layer]() {
                
                auto winSize = Director::getInstance()->getWinSize();
                
                auto result = Sprite::create("result.png");
                result->setPosition(winSize.width / 2.0, winSize.height - 100);
                layer->addChild(result);
                
                auto resultScore = Label::createWithCharMap("result_number.png", 20, 27, '0');
                resultScore->setString(StringUtils::toString(_score));
                layer->addChild(resultScore);
                resultScore->setPosition(winSize.width / 2.0, winSize.height - 130);
                
                auto title = MenuItemImage::create("return.png", "return_pressed.png", [this, layer](Ref* ref) {
                    // パーティクルの表示
                    auto button = dynamic_cast<Node *>(ref);
                    auto position = button->getParent()->convertToWorldSpace(button->getPosition());
                    auto particle = ParticleSystemQuad::create("particles/button-effect.plist");
                    particle->setPosition(position);
                    layer->addChild(particle);
                    
                    SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_CHOICE);
                    auto scene = TitleScene::createScene();
                    auto transition = TransitionFade::create(1.0, scene);
                    Director::getInstance()->replaceScene(transition);
                    SharedCueSheet::getInstance()->getCueSheet()->stop(_musicId);
                    _musicId = 0;
                });
                auto replay = MenuItemImage::create("retry.png", "retry_pressed.png", [this, layer](Ref* ref) {
                    // パーティクルの表示
                    auto button = dynamic_cast<Node *>(ref);
                    auto position = button->getParent()->convertToWorldSpace(button->getPosition());
                    auto particle = ParticleSystemQuad::create("particles/button-effect.plist");
                    particle->setPosition(position);
                    layer->addChild(particle);
                    
                    SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_CHOICE);
                    auto scene = MainScene::createScene();
                    auto transition = TransitionFade::create(1.0, scene);
                    Director::getInstance()->replaceScene(transition);
                });
                auto menu = Menu::create(replay, title, NULL);
                menu->setPosition(winSize.width / 2.0, winSize.height / 2.0);
                menu->alignItemsVerticallyWithPadding(20);
                
                layer->addChild(menu);
                
            }),
                                             NULL));
            
        }
    }
    _secondLabel->setString(StringUtils::toString(floor(_second)));
    
    // スコアの更新
    _scoreLabel->setString(StringUtils::toString(_score));
}

Cookie* MainScene::getCookieAt(const cocos2d::Vec2& position)
{
    for (auto& cookie : _cookies) {
        if (position.equals(cookie->getCookiePosition())) {
            return cookie;
        }
    }
    return nullptr;
}

Cookie* MainScene::getCookieAtByWorld(const cocos2d::Vec2& worldPosition)
{
    // 絶対座標をステージ中の位置に変換
    auto stagePosition = _stage->convertToNodeSpace(worldPosition);
    // ステージ中の位置をグリッド座標に変換
    auto gridPosition = Cookie::convertToGridSpace(stagePosition);
    return this->getCookieAt(gridPosition);
}

void MainScene::addCookie(Cookie *cookie)
{
    // クッキー一覧にクッキーを追加
    _cookies.pushBack(cookie);
    // _stageノードにクッキーを追加
    _stage->addChild(cookie);
    // クッキーの位置を調整
    cookie->adjustPosition();
}

void MainScene::moveCookie(Cookie *cookie, const cocos2d::Vec2& cookiePosition)
{
    cookie->setCookiePosition(cookiePosition);
    cookie->adjustPosition();
}

bool MainScene::swapCookies(Cookie *cookie0, Cookie *cookie1)
{
    // 連鎖数を0にする
    _chainCount = 0;
    
    // 効果音を鳴らす
    SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_SWIPE);
    
    // 画面上の位置を取得しておく
    auto position0 = cookie0->getPosition();
    auto position1 = cookie1->getPosition();
    
    // フィールド上の位置も取得しておく
    auto cookiePosition0 = cookie0->getCookiePosition();
    auto cookiePosition1 = cookie1->getCookiePosition();
    
    // 消去判定のため、予め動かす
    cookie1->setCookiePosition(cookiePosition0);
    cookie0->setCookiePosition(cookiePosition1);
    
    // それぞれ、動いた後の位置で隣接しているクッキーを取り出す
    auto cookies0 = this->checkNeighborCookies(cookie0);
    auto cookies1 = this->checkNeighborCookies(cookie1);
    
    // いずれかが4つ以上繋がってる場合、移動可能
    bool canMove = cookies0.size() >= VANISH_COUNT || cookies1.size() >= VANISH_COUNT;
    
    // 移動中状態にする
    cookie0->setState(Cookie::State::SWAPPING);
    cookie1->setState(Cookie::State::SWAPPING);
    
    // 元の位置に戻しておく
    cookie1->setCookiePosition(cookiePosition1);
    cookie0->setCookiePosition(cookiePosition0);
    
    /** 移動アニメーションの追加関数
     *  @param cookie 動かすクッキー
     *  @param toPosition 移動先のステージ座標
     *  @param toCookiePosition 移動先のグリッド座標
     */
    auto addMoveAnimation = [canMove, this](Cookie *cookie, Vec2 toPosition, Vec2 toCookiePosition)
    {
        // 移動アニメーションの実行時間
        const auto duration = 0.08;
        auto fromPosition = cookie->getPosition();
        
        cookie->runAction(Sequence::create(MoveTo::create(duration, toPosition),
                                           CallFuncN::create([=](Node *node) {
            auto cookie = dynamic_cast<Cookie *>(node);
            // もし、クッキーが動かせるとき
            if (canMove) {
                // 位置を入れ替える
                this->moveCookie(cookie, toCookiePosition);
                // 状態を静止中にする
                cookie->setState(Cookie::State::STATIC);
            } else { // もし、動かせないとき
                // 元に戻すアニメーション
                cookie->runAction(Sequence::create(MoveTo::create(duration / 2.0, fromPosition),
                                                   CallFuncN::create([] (Node *node) {
                    auto cookie = dynamic_cast<Cookie *>(node);
                    // 状態を停止中にする
                    cookie->setState(Cookie::State::STATIC);
                }), NULL));
            }
        }), NULL));
    };
    
    // 移動アニメーションを追加する
    addMoveAnimation(cookie0, position1, cookiePosition1);
    addMoveAnimation(cookie1, position0, cookiePosition0);
    return canMove;
}

bool MainScene::checkVanish()
{
    // チェック済みのクッキー
    CookieVector checked;
    for (Cookie * cookie : _cookies) {
        // クッキーがチェック済みじゃないとき
        if (!checked.contains(cookie)) {
            // そのクッキーと隣接してるクッキーを全て取り出す
            CookieVector neighbors = this->checkNeighborCookies(cookie);
            // 隣接していた物はチェック済みにして重複して判定しない
            checked.pushBack(neighbors);
            // 隣接してるクッキーが一定個数以上なら
            if (neighbors.size() >= VANISH_COUNT) {
                // コンボ回数の追加
                _chainCount += 1;
                // スコアの追加
                _score += 1000 * pow(3, _chainCount);
                // コンボカウンターを表示
                this->showChainCount(neighbors.getRandomObject(), _chainCount);
                // 隣接してるクッキー全てを消去にする
                for (auto neighborCookie : neighbors) {
                    this->vanishCookie(neighborCookie);
                }
                return true;
            }
        }
    }
    return false;
}

void MainScene::vanishCookie(Cookie *cookie)
{
    // クッキーがnullptrだったら何もしない
    if (!cookie) return;
    // 状態を消去中にする
    cookie->setState(Cookie::State::DISAPEARING);
    // アニメーションの時間
    const auto duration = 0.2f;
    
    // 消去パーティクルを追加する
    auto particle = ParticleSystemQuad::create("particles/vanish.plist");
    particle->setPosition(cookie->getPosition());
    _stage->addChild(particle, ZOrderEffect);
    
    // 削除アニメーションを追加する
    cookie->runAction(Sequence::create(FadeOut::create(duration),
                                       CallFuncN::create([this](Node* node) {
        // クッキー一覧から削除する
        auto cookie = dynamic_cast<Cookie *>(node);
        _cookies.eraseObject(cookie);
    }),
                                       RemoveSelf::create(),
                                       NULL));
}

CookieVector MainScene::checkNeighborCookies(Cookie *cookie) {
    // 空のベクターを作って渡している
    CookieVector v;
    return this->checkNeighborCookies(cookie, std::move(v));
}

CookieVector MainScene::checkNeighborCookies(Cookie *cookie, CookieVector checked) {
    // すでにチェック済みだった場合は何もせずにそのままチェック済みのベクターを返す
    if (checked.contains(cookie)) {
        return std::move(checked);
    }
    // クッキーをチェック済み一覧に追加する
    checked.pushBack(cookie);
    
    // チェックするクッキーのグリッド上の位置を取り出す
    auto position = cookie->getCookiePosition();
    
    // グリッド上の上下左右にあるクッキーを取得する
    auto up = this->getCookieAt(Vec2(position.x, position.y + 1));
    auto down = this->getCookieAt(Vec2(position.x, position.y - 1));
    auto left = this->getCookieAt(Vec2(position.x - 1, position.y));
    auto right = this->getCookieAt(Vec2(position.x + 1, position.y));
    
    // 上にクッキーがあって同じ形なら、上にあるクッキーも再帰的にチェックする
    if (up && up->getCookieShape() == cookie->getCookieShape()) {
        checked = this->checkNeighborCookies(up, std::move(checked));
    }
    // 下
    if (down && down->getCookieShape() == cookie->getCookieShape()) {
        checked = this->checkNeighborCookies(down, std::move(checked));
    }
    // 左
    if (left && left->getCookieShape() == cookie->getCookieShape()) {
        checked = this->checkNeighborCookies(left, std::move(checked));
    }
    // 右
    if (right && right->getCookieShape() == cookie->getCookieShape()) {
        checked = this->checkNeighborCookies(right, std::move(checked));
    }
    return std::move(checked);
}

bool MainScene::fallCookie(Cookie *cookie)
{
    auto position = cookie->getCookiePosition();
    // すでに一番下にあったときや、停止中じゃないとき、落ちない
    if (position.y == 0 || !cookie->isStatic()) {
        return false;
    }
    // 1つ下のグリッド座標を取り出す
    auto downPosition = Vec2(position.x, position.y - 1);
    // 1つ下のクッキーを取り出す
    auto down = this->getCookieAt(Vec2(position.x, position.y - 1));
    // 1つ下がなかったとき、落ちる
    if (down == nullptr) {
        // 落下アニメーション時間
        const auto duration = 0.05;
        // 落下距離
        auto distance = -Cookie::getSize();
        // 状態を落下中にする
        cookie->setState(Cookie::State::FALLING);
        // 落下アニメーションの実行
        cookie->runAction(Sequence::create(MoveBy::create(duration, Vec2(0, distance)),
                                           CallFuncN::create([this, downPosition] (Node *node) {
            // 落下アニメーション終了後
            auto cookie = dynamic_cast<Cookie *>(node);
            // クッキーを動かす
            this->moveCookie(cookie, downPosition);
            cookie->setState(Cookie::State::STATIC);
            // さらに落ちないか再度落下判定を行う
            this->fallCookie(cookie);
        }),
                                           NULL));
        return true;
    }
    return false;
}

cocos2d::Vector<Cookie *> MainScene::checkSpawn()
{
    // 出現したクッキーの一覧
    cocos2d::Vector<Cookie *> cookies;
    // 一番上の座標を取り出す
    auto y = VERTICAL_COUNT - 1;
    // 一番上の行を全てチェック
    for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
        auto cookie = this->getCookieAt(Vec2(x, y));
        if (!cookie) { // もしクッキーがなければ
            // クッキーを追加する
            auto cookie = Cookie::create();
            cookie->setCookiePosition(Vec2(x, y));
            this->addCookie(cookie);
        }
    }
    return std::move(cookies);
}

bool MainScene::canVanishNext(Cookie *cookie)
{
    auto cookies = this->checkNeighborCookies(cookie);
    auto skews = std::vector<Vec2> {Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};
    auto allDirections = std::vector<Vec2> {Vec2(0, 2), Vec2(2, 0), Vec2(0, -2), Vec2(-2, 0), Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};
    auto currentVector = cookie->getCookiePosition();
    
    if (cookies.size() >= VANISH_COUNT) {
        // 4以上は存在しないはずだけどtrue
        return true;
    } else if (cookies.size() == 3) {
        // もし3つ繋がっていたとき
        for (auto vector : allDirections) {
            auto nextVector = cookie->getCookiePosition() + vector;
            auto nextCookie = this->getCookieAt(nextVector);
            // 塊に含まれるクッキーそれぞれの距離2の位置に、別の同種のクッキーが1つでもあったら消せる
            if (nextCookie && cookie && nextCookie->getCookieShape() == cookie->getCookieShape() && !cookies.contains(nextCookie)) {
                return true;
            }
        }
    } else if (cookies.size() == 2) {
        // もし2つ繋がっていたとき
        for (auto vector : allDirections) {
            auto nextVector = cookie->getCookiePosition() + vector;
            auto nextCookie = this->getCookieAt(nextVector);
            // 塊に含まれるクッキーの距離2の位置に、別の同種のクッキーがある
            if (nextCookie && cookie && nextCookie->getCookieShape() == cookie->getCookieShape() && !cookies.contains(nextCookie)) {
                // かつ、そのクッキーと塊のクッキーがナナメ方向に共通の同種のクッキーを持っている
                for (auto sv0 : skews) {
                    for (auto sv1 : skews) {
                        auto e0 = this->getCookieAt(currentVector + sv0);
                        auto e1 = this->getCookieAt(nextVector + sv1);
                        if (e0 &&
                            e1 &&
                            e0->getCookieShape() == cookie->getCookieShape() &&
                            e0 == e1 &&
                            !cookies.contains(e0) &&
                            !cookies.contains(e1)) {
                            // 斜めに共通のcookieがあれば消せる
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool MainScene::isAllStatic()
{
    return std::all_of(_cookies.begin(),
                       _cookies.end(),
                       [](Cookie* cookie) { return cookie->isStatic(); });
}

void MainScene::showChainCount(Cookie * cookie, int count)
{
    std::string filename;
    if (count >= 8) {
        // コンボ数が8以上なら色を変える
        filename = "Chain1.json";
    } else {
        filename = "Chain0.json";
    }
    auto chainCount = cocostudio::GUIReader::getInstance()->widgetFromJsonFile(filename.c_str());
    chainCount->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    ui::TextAtlas * atlas = dynamic_cast<ui::TextAtlas *>(chainCount->getChildByTag(6));
    atlas->setString(StringUtils::toString(count));
    chainCount->setPosition(cookie->getParent()->convertToWorldSpace(cookie->getPosition()));
    chainCount->setScale(0);
    this->addChild(chainCount, ZOrderEffect);
    
    chainCount->runAction(Sequence::create(ScaleTo::create(0.2, 1.0),
                                           DelayTime::create(1.0),
                                           ScaleTo::create(0.2, 0),
                                           RemoveSelf::create(), NULL));
    
}
