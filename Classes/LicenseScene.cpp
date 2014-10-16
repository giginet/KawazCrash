//
//  LicenseScene.cpp
//  KawazCrash
//
//  Created by giginet on 2014/10/14.
//
//

#include "LicenseScene.h"
#include "TitleScene.h"

USING_NS_CC;

Scene* LicenseScene::createScene()
{
    auto scene = Scene::create();
    auto License = LicenseScene::create();
    scene->addChild(License);
    return scene;
}

LicenseScene::LicenseScene()
: _player(nullptr)
{
}

LicenseScene::~LicenseScene()
{
    CC_SAFE_RELEASE_NULL(_player);
}

bool LicenseScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    this->scheduleUpdate();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch * touch, Event * event) {
        this->gotoTitle();
        return true;
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void LicenseScene::onEnterTransitionDidFinish()
{
    auto winSize = Director::getInstance()->getWinSize();
    
    // アニメーションの作成
    auto resourceManager = ss::ResourceManager::getInstance();
    resourceManager->addData("ss_logo640X960.ssbp");
    
    auto player = ss::Player::create();
    // ssbpファイル名（拡張子不要）
    player->setData("ss_logo640X960");
    // アニメーション名を指定
    player->play("anime 1");
    player->setScale(0.5);
    player->setPosition(winSize.width / 2.0, winSize.height / 2.0);
    this->addChild(player);
    
    this->setPlayer(player);
}

void LicenseScene::update(float dt)
{
    if (_player->getFrameNo() == _player->getMaxFrame() - 1) {
        this->gotoTitle();
    }
}

void LicenseScene::gotoTitle()
{
    auto director = Director::getInstance();
    auto scene = TitleScene::createScene();
    auto transition = TransitionCrossFade::create(0.5f, scene);
    director->replaceScene(transition);
}