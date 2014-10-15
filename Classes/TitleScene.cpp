//
//  TitleScene.cpp
//  KawazCrash
//
//  Created by giginet on 2014/10/14.
//
//

#include "TitleScene.h"
#include "MainScene.h"
#include "cookie_title.h"

USING_NS_CC;

Scene* TitleScene::createScene()
{
    auto scene = Scene::create();
    auto title = TitleScene::create();
    scene->addChild(title);
    return scene;
}

TitleScene::TitleScene()
: _cueSheet(NULL)
{
}

TitleScene::~TitleScene()
{
    CC_SAFE_RELEASE_NULL(_cueSheet);
}

void TitleScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    
    auto cueSheet = ADX2::CueSheet::create("adx2/cookie/cookie_crush.acf", "adx2/cookie/cookie_title.acb");
    this->setCueSheet(cueSheet);
    
    _cueSheet->playCueByID(CRI_COOKIE_TITLE_TITLE);
}

bool TitleScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    auto winSize = Director::getInstance()->getWinSize();
    auto title = Sprite::create("title.png");
    
    title->setPosition(winSize.width / 2.0, winSize.height / 2.0);
    
    this->addChild(title);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch * touch, Event * event) {
        auto scene = MainScene::createScene();
        auto transition = TransitionCrossFade::create(0.5, scene);
        Director::getInstance()->replaceScene(transition);
        return true;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}