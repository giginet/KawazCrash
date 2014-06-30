//
//  MainScene.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "MainScene.h"
#include "Cookie.h"
#include <algorithm>

#include "CueSheet_0.h"
#include "test_acf.h"

#include <AudioToolbox/AudioSession.h>
#include "ADX2Manager.h"

USING_NS_CC;

const int HORIZONTAL_COUNT = 6;
const int VERTICAL_COUNT = 8;
const int VANISH_COUNT = 4;

MainScene::MainScene() : _currentCookie(nullptr), _cue(nullptr)
{
    // ADX2を初期化します
    CriAtomExStandardVoicePoolConfig vp_config;
    criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vp_config);
    vp_config.num_voices = 8;
    vp_config.player_config.streaming_flag = CRI_TRUE;
    vp_config.player_config.max_sampling_rate = 48000 << 1;
    
    CriAtomExPlayerConfig pf_config;
    criAtomExPlayer_SetDefaultConfig(&pf_config);
    pf_config.max_path_strings = 1;
    pf_config.max_path = 256;
    
    ADX2::ADX2Manager::initialize(pf_config, vp_config);
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_currentCookie);
    CC_SAFE_RELEASE_NULL(_cue);
    ADX2::ADX2Manager::finalize();
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
    
    auto cue = ADX2::Cue::create("test.acf", "CueSheet_0.acb");
    this->setCue(cue);
    
    this->setStage(Node::create());
    for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
        for (int y = 0; y < VERTICAL_COUNT; ++y) {
            auto cookie = Cookie::create();
            cookie->setCookiePosition(Vec2(x, y));
            this->addCookie(cookie);
        }
    }
    auto winSize = Director::getInstance()->getWinSize();
    auto leftMargin = winSize.width - Cookie::getSize() * HORIZONTAL_COUNT;
    _stage->setPosition(Vec2(leftMargin / 2 + Cookie::getSize() / 2.0, 50));
    this->addChild(_stage);
    
    // タッチイベントの登録
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto position = touch->getLocation();
        // 現在のタッチ位置にあるクッキーを取り出す
        auto cookie = this->getCookieAt(position);
        
        // 現在移動中のクッキーとして設定
        this->setCurrentCookie(cookie);
        return true;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        // 移動先のクッキーを取得
        auto nextCookie = this->getCookieAt(touch->getLocation());
        
        // 元のクッキー、移動後のクッキーが共に存在していて、違った場合、移動させる
        if (_currentCookie != nullptr &&
            nextCookie != nullptr &&
            _currentCookie != nextCookie) {
            if (_currentCookie->isNormal() && nextCookie->isNormal()) {
                auto cp = _currentCookie->getCookiePosition();
                auto np = nextCookie->getCookiePosition();
                if (cp.y == np.y && cp.x + 1 == np.x) { // 右方向
                    this->swapCookies(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
                if (cp.y == np.y && cp.x - 1 == np.x) { // 左方向
                    this->swapCookies(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
                if (cp.x == np.x && cp.y + 1 == np.y) { // 上方向
                    this->swapCookies(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
                if (cp.x == np.x && cp.y - 1 == np.y) { // 下方向
                    this->swapCookies(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
            }
        }
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        this->setCurrentCookie(nullptr);
    };
    listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        this->setCurrentCookie(nullptr);
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    this->scheduleUpdate();
    
    return true;
}

void MainScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    
    // 音楽を再生する
    this->getCue()->playCueByID(CRI_CUESHEET_0_BGM_INT);
}

void MainScene::update(float dt)
{
    // ADX2を更新する
    ADX2::ADX2Manager::getInstance()->update();
    
    // 全クッキーに対して落下を判定する
    for (auto cookie : _cookies) {
        this->fallCookie(cookie);
    }
    
    // クッキーの生成
    this->spawnCookies();
    
    //
    this->updateField();
}

Cookie* MainScene::getCookieAt(int x, int y)
{
    for (auto cookie : _cookies) {
        if (cookie->getCookiePosition().x == x && cookie->getCookiePosition().y == y) {
            return cookie;
        }
    }
    return nullptr;
}

Cookie* MainScene::getCookieAt(cocos2d::Vec2 position)
{
    auto stagePoint = _stage->convertToNodeSpace(position);
    auto x = floor((stagePoint.x + Cookie::getSize() / 2.0) / Cookie::getSize());
    auto y = floor((stagePoint.y + Cookie::getSize() / 2.0) / Cookie::getSize());
    return this->getCookieAt(x, y);
}

void MainScene::addCookie(Cookie *cookie)
{
    _cookies.pushBack(cookie);
    _stage->addChild(cookie);
    cookie->adjustPosition();
}

void MainScene::moveCookie(Cookie *cookie, cocos2d::Vec2 cookiePosition)
{
    cookie->setCookiePosition(cookiePosition);
    cookie->adjustPosition();
}

void MainScene::swapCookies(Cookie *cookie0, Cookie *cookie1)
{
    const auto duration = 0.2;
    
    auto position0 = cookie0->getPosition();
    auto position1 = cookie1->getPosition();
    
    auto currentPosition0 = cookie0->getCookiePosition();
    auto currentPosition1 = cookie1->getCookiePosition();
    
    // 要リファクタリング
    // 予め動かしておいて
    cookie1->setCookiePosition(currentPosition0);
    cookie0->setCookiePosition(currentPosition1);
    
    CookieVector v0;
    CookieVector v1;
    auto cookies0 = this->checkNeighborCookies(cookie0, v0);
    auto cookies1 = this->checkNeighborCookies(cookie1, v1);
    
    bool isVanish = cookies0.size() >= VANISH_COUNT || cookies1.size() >= VANISH_COUNT;
    
    cookie0->setState(Cookie::State::SWAPPING);
    cookie1->setState(Cookie::State::SWAPPING);
    
    // 酷い実装だからあとで直す！！！！！
    
    if (!isVanish) { // 消えなかったら戻す
        cookie1->setCookiePosition(currentPosition1);
        cookie0->setCookiePosition(currentPosition0);
    }
    
    cookie0->runAction(Sequence::create(MoveTo::create(duration, position1),
                                        CallFuncN::create([=](Node *node) {
        auto cookie = dynamic_cast<Cookie *>(node);
        if (isVanish) {
            this->moveCookie(cookie, currentPosition1);
            cookie->setState(Cookie::State::NORMAL);
            this->updateField();
        } else {
            // 元に戻す
            cookie->runAction(Sequence::create(MoveTo::create(duration / 2.0, position0),
                                               CallFuncN::create([] (Node *node) {
                auto cookie = dynamic_cast<Cookie *>(node);
                cookie->setState(Cookie::State::NORMAL);
            }), NULL));
        }
    }), NULL));
    cookie1->runAction(Sequence::create(MoveTo::create(duration, position0),
                                        CallFuncN::create([=](Node *node) {
        auto cookie = dynamic_cast<Cookie *>(node);
        if (isVanish) {
            this->moveCookie(cookie, currentPosition0);
            cookie->setState(Cookie::State::NORMAL);
            this->updateField();
        } else {
            // 元に戻す
            cookie->runAction(Sequence::create(MoveTo::create(duration / 2.0, position1),
                                               CallFuncN::create([] (Node *node) {
                auto cookie = dynamic_cast<Cookie *>(node);
                cookie->setState(Cookie::State::NORMAL);
            }), NULL));
        }
    }),
                                        NULL));
}

bool MainScene::vanishCookies(Cookie *cookie)
{
    CookieVector v;
    auto cookies = this->checkNeighborCookies(cookie, v);
    bool canVanish = std::all_of(cookies.begin(), cookies.end(), [](Cookie *cookie) { return cookie->isNormal(); });
    if (cookies.size() >= VANISH_COUNT && canVanish) {
        for (auto cookie : cookies) {
            this->deleteCookie(cookie);
        }
        return true;
    }
    return false;
}

void MainScene::deleteCookie(Cookie *cookie)
{
    if (!cookie) return;
    cookie->setState(Cookie::State::DISAPEARING);
    cookie->runAction(Sequence::create(FadeOut::create(0.5f),
                                       CallFuncN::create([this](Node* node) {
        auto cookie = dynamic_cast<Cookie *>(node);
        _cookies.eraseObject(cookie);
    }),
                                       RemoveSelf::create(),
                                       CallFunc::create([this] {
    }),
                                       NULL));
}

CookieVector MainScene::checkNeighborCookies(Cookie *cookie, CookieVector checked) {
    if (checked.contains(cookie)) {
        return std::move(checked);
    }
    checked.pushBack(cookie);
    auto position = cookie->getCookiePosition();
    auto up = this->getCookieAt(position.x, position.y + 1);
    auto down = this->getCookieAt(position.x, position.y - 1);
    auto left = this->getCookieAt(position.x - 1, position.y);
    auto right = this->getCookieAt(position.x + 1, position.y);
    
    if (up && up->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborCookies(up, checked);
    }
    if (down && down->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborCookies(down, checked);
    }
    if (left && left->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborCookies(left, checked);
    }
    if (right && right->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborCookies(right, checked);
    }
    return std::move(checked);
}

bool MainScene::fallCookie(Cookie *cookie)
{
    auto position = cookie->getCookiePosition();
    // すでに一番下にあったとき、落ちない
    if (position.y == 0) {
        return false;
    }
    // クッキーがNORMAL状態じゃなかったとき、落ちない
    if (!cookie->isNormal()) {
        return false;
    }
    auto downPosition = Vec2(position.x, position.y - 1);
    // 1つ下のクッキーを取り出す
    auto down = this->getCookieAt(position.x, position.y - 1);
    // 1つ下がなかったとき、落ちる
    if (down == nullptr) {
        auto duration = 0.05;
        cookie->setState(Cookie::State::FALLING);
        cookie->runAction(Sequence::create(MoveBy::create(duration, Vec2(0, -Cookie::getSize())),
                                           CallFuncN::create([this, downPosition] (Node *node) {
            // 落下アニメーション終了語
            auto cookie = dynamic_cast<Cookie *>(node);
            // クッキーを動かす
            this->moveCookie(cookie, downPosition);
            cookie->setState(Cookie::State::NORMAL);
            // さらに落ちないか再度落下判定を行う
            this->fallCookie(cookie);
        }),
                                           NULL));
        return true;
    }
    return false;
}

cocos2d::Vector<Cookie *> MainScene::spawnCookies()
{
    cocos2d::Vector<Cookie *> cookies;
    // 一番上の座標を取り出す
    auto y = VERTICAL_COUNT - 1;
    // 一番上の行を全てチェック
    for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
        auto cookie = this->getCookieAt(x, y);
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
    CookieVector cookies;
    cookies = this->checkNeighborCookies(cookie, cookies);
    auto skews = std::vector<Vec2> {Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};
    auto allDirections = std::vector<Vec2> {Vec2(0, 2), Vec2(2, 0), Vec2(0, -2), Vec2(-2, 0), Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};
    auto currentVector = cookie->getCookiePosition();
    
    if (cookies.size() >= VANISH_COUNT) {
        // 4以上は存在しないはずだけどtrue
        return true;
    } else if (cookies.size() == 3) {
        for (auto vector : allDirections) {
            auto nextVector = cookie->getCookiePosition() + vector;
            auto nextCookie = this->getCookieAt(nextVector.x, nextVector.y);
            if (nextCookie && cookie && nextCookie->getCookieColor() == cookie->getCookieColor() && !cookies.contains(nextCookie)) {
                return true;
            }
        }
    } else if (cookies.size() == 2) {
        for (auto vector : allDirections) {
            auto nextVector = cookie->getCookiePosition() + vector;
            auto nextCookie = this->getCookieAt(nextVector.x, nextVector.y);
            if (nextCookie && cookie && nextCookie->getCookieColor() == cookie->getCookieColor() && !cookies.contains(nextCookie)) {
                for (auto sv0 : skews) {
                    for (auto sv1 : skews) {
                        auto e0 = this->getCookieAt((currentVector + sv0).x, (currentVector + sv0).y);
                        auto e1 = this->getCookieAt((nextVector + sv1).x, (nextVector + sv1).y);
                        if (e0 && e1 && e0->getCookieColor() == cookie->getCookieColor() && e0 == e1 && !cookies.contains(e0) && !cookies.contains(e1)) {
                            // 斜めに共通のcookieがあれば消せるはず！
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void MainScene::updateField()
{
    if (this->isAllNormal()) {
        // 既に揃ってるのを消す
        CookieVector checked;
        for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                auto cookie = this->getCookieAt(x, y);
                if (cookie && !checked.contains(cookie)) {
                    CookieVector v;
                    v = this->checkNeighborCookies(cookie, v);
                    this->vanishCookies(cookie);
                    checked.pushBack(v);
                }
            }
        }
        
        // 次にどれも消えなさそうだったらランダムに2列消す
        for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                auto cookie = this->getCookieAt(x, y);
                if (cookie && this->canVanishNext(cookie)) {
                    // どれか消えそうなら探索を打ち切る
                    return;
                }
            }
        }
        // もしどれも消えなかったとき、ランダムに2列を選んで消去する
        auto baseX = rand() % HORIZONTAL_COUNT;
        auto otherX = (rand() % (HORIZONTAL_COUNT - 1) + baseX) % HORIZONTAL_COUNT;
        for (int y = 0; y < VERTICAL_COUNT; ++y) {
            this->deleteCookie(this->getCookieAt(baseX, y));
            this->deleteCookie(this->getCookieAt(otherX, y));
        }
        
    }
}

bool MainScene::isAllNormal()
{
    return std::all_of(_cookies.begin(), _cookies.end(), [](Cookie* cookie) { return cookie->isNormal(); });
}