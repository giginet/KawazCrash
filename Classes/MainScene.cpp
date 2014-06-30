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

USING_NS_CC;

const int HORIZONTAL_COUNT = 6;
const int VERTICAL_COUNT = 8;
const int VANISH_COUNT = 4;

MainScene::MainScene() : _currentCookie(nullptr)
{
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_currentCookie);
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
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto position = touch->getLocation();
        auto cookie = this->getCookieAt(position);
        this->setCurrentCookie(cookie);
        return true;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        auto nextCookie = this->getCookieAt(touch->getLocation());
        if (_currentCookie != nullptr && nextCookie != nullptr && _currentCookie != nextCookie) {
            if (_currentCookie->isNormal() && nextCookie->isNormal()) {
                auto cp = _currentCookie->getCookiePosition();
                auto np = nextCookie->getCookiePosition();
                if (cp.y == np.y && cp.x + 1 == np.x) { // 右方向
                    this->swapEntities(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
                if (cp.y == np.y && cp.x - 1 == np.x) { // 左方向
                    this->swapEntities(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
                if (cp.x == np.x && cp.y + 1 == np.y) { // 上方向
                    this->swapEntities(_currentCookie, nextCookie);
                    this->setCurrentCookie(nullptr);
                }
                if (cp.x == np.x && cp.y - 1 == np.y) { // 下方向
                    this->swapEntities(_currentCookie, nextCookie);
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

void MainScene::update(float dt)
{
    for (auto cookie : _cookies) {
        this->checkFall(cookie);
    }
    this->spawnEntities();
    this->checkField();
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

bool MainScene::moveCookie(Cookie *cookie, cocos2d::Vec2 cookiePosition)
{
    cookie->setCookiePosition(cookiePosition);
    cookie->adjustPosition();
    return true;
}

bool MainScene::swapEntities(Cookie *cookie0, Cookie *cookie1)
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
    auto cookies0 = this->checkNeighborEntities(cookie0, v0);
    auto cookies1 = this->checkNeighborEntities(cookie1, v1);
    
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
            this->checkField();
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
            this->checkField();
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
    return true;
}

bool MainScene::checkVanishEntities(Cookie *cookie)
{
    CookieVector v;
    auto cookies = this->checkNeighborEntities(cookie, v);
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

CookieVector MainScene::checkNeighborEntities(Cookie *cookie, CookieVector checked) {
    if (checked.contains(cookie)) {
        return checked;
    }
    checked.pushBack(cookie);
    auto position = cookie->getCookiePosition();
    auto up = this->getCookieAt(position.x, position.y + 1);
    auto down = this->getCookieAt(position.x, position.y - 1);
    auto left = this->getCookieAt(position.x - 1, position.y);
    auto right = this->getCookieAt(position.x + 1, position.y);
    
    if (up && up->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborEntities(up, checked);
    }
    if (down && down->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborEntities(down, checked);
    }
    if (left && left->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborEntities(left, checked);
    }
    if (right && right->getCookieColor() == cookie->getCookieColor()) {
        checked = this->checkNeighborEntities(right, checked);
    }
    return std::move(checked);
}

void MainScene::checkFall(Cookie *cookie)
{
    auto position = cookie->getCookiePosition();
    if (position.y == 0) {
        return;
    }
    if (!cookie->isNormal()) {
        return;
    }
    auto downPosition = Vec2(position.x, position.y - 1);
    auto down = this->getCookieAt(position.x, position.y - 1);
    if (down == nullptr) {
        auto duration = 0.05;
        cookie->setState(Cookie::State::FALLING);
        cookie->runAction(Sequence::create(MoveBy::create(duration, Vec2(0, -Cookie::getSize())),
                                           CallFuncN::create([this, downPosition] (Node *node) {
            auto cookie = dynamic_cast<Cookie *>(node);
            this->moveCookie(cookie, downPosition);
            cookie->setState(Cookie::State::NORMAL);
            this->checkField();
            this->checkFall(cookie);
        }),
                                           NULL));
    }
}

cocos2d::Vector<Cookie *> MainScene::spawnEntities()
{
    cocos2d::Vector<Cookie *> cookies;
    auto y = VERTICAL_COUNT - 1;
    for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
        auto cookie = this->getCookieAt(x, y);
        if (!cookie) {
            auto cookie = Cookie::create();
            cookie->setCookiePosition(Vec2(x, y));
            this->addCookie(cookie);
            cookies.pushBack(cookie);
            this->checkVanishEntities(cookie);
        }
    }
    return std::move(cookies);
}

bool MainScene::canVanishNext(Cookie *cookie)
{
    CookieVector cookies;
    cookies = this->checkNeighborEntities(cookie, cookies);
    auto vectors = std::vector<Vec2> {Vec2(0, 2), Vec2(2, 0), Vec2(0, -2), Vec2(-2, 0)};
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
        for (auto vector : vectors) {
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

void MainScene::checkField()
{
    if (this->isAllNormal()) {
        // 既に揃ってるのを消す
        CookieVector checked;
        for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                auto cookie = this->getCookieAt(x, y);
                if (cookie && !checked.contains(cookie)) {
                    CookieVector v;
                    v = this->checkNeighborEntities(cookie, v);
                    this->checkVanishEntities(cookie);
                    checked.pushBack(v);
                }
            }
        }
        
        // 次にどれも消えなさそうだったらランダムに2列消す
        bool flag = false;
        for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                auto cookie = this->getCookieAt(x, y);
                if (!flag && cookie && this->canVanishNext(cookie)) {
                    // どれか消えそうなら探索を打ち切る
                    flag = true;
                }
            }
        }
        if (!flag && _cookies.size() == HORIZONTAL_COUNT * VERTICAL_COUNT) {
            auto baseX = rand() % HORIZONTAL_COUNT;
            auto otherX = (rand() % (HORIZONTAL_COUNT - 1) + baseX) % HORIZONTAL_COUNT;
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                this->deleteCookie(this->getCookieAt(baseX, y));
                this->deleteCookie(this->getCookieAt(otherX, y));
            }
        }
    }
}

bool MainScene::isAllNormal()
{
    return std::all_of(_cookies.begin(), _cookies.end(), [](Cookie* cookie) { return cookie->isNormal(); });
}