//
//  TitleScene.cpp
//  KawazCrash
//
//  Created by giginet on 2014/10/14.
//
//

#include "TitleScene.h"
#include "MainScene.h"
#include "SharedCueSheet.h"

#include "cookie_main.h"

USING_NS_CC;

Scene* TitleScene::createScene()
{
    auto scene = Scene::create();
    auto title = TitleScene::create();
    scene->addChild(title);
    return scene;
}

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    
    auto id = SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_TITLE);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this, id](Touch * touch, Event * event) {
        auto scene = MainScene::createScene();
        auto transition = TransitionCrossFade::create(0.5, scene);
        Director::getInstance()->replaceScene(transition);
        
        SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_CHOICE);
        SharedCueSheet::getInstance()->getCueSheet()->stop(id);
        
        return true;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
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
    
    return true;
}