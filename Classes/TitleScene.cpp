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
: _jingleId(0)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    
    _jingleId = SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_TITLE);
    
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
    
    auto logo = Sprite::create("logo.png");
    logo->setPosition(winSize.width / 2.0, winSize.height - 150);
    this->addChild(logo);
    
    auto tap = Sprite::create("tapicon.png");
    tap->setPosition(winSize.width / 2.0, 220);
    this->addChild(tap);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this, tap](Touch * touch, Event * event) {
        auto scene = MainScene::createScene();
        auto transition = TransitionCrossFade::create(0.5, scene);
        Director::getInstance()->replaceScene(transition);
        
        tap->setTexture(Director::getInstance()->getTextureCache()->addImage("tapicon_pressed.png"));
        
        SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_CHOICE);
        SharedCueSheet::getInstance()->getCueSheet()->stop(_jingleId);
        
        return true;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}