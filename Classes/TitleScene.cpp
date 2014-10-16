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
    tap->setPosition(winSize.width / 2.0, winSize.height - 348);
    this->addChild(tap);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    static bool isPressed = false;
    touchListener->onTouchBegan = [this, tap](Touch * touch, Event * event) {
        if (isPressed) return true;
        
        SharedCueSheet::getInstance()->getCueSheet()->playCueByID(CRI_COOKIE_MAIN_CHOICE);
        
        isPressed = true;
        
        // パーティクルの表示
        auto particle = ParticleSystemQuad::create("particles/button-effect.plist");
        particle->setPosition(tap->getPosition());
        this->addChild(particle);
        tap->setTexture(Director::getInstance()->getTextureCache()->addImage("tapicon_pressed.png"));
        
        this->runAction(Sequence::create(DelayTime::create(0.8),
                                         CallFunc::create([tap, this]() {
            auto scene = MainScene::createScene();
            auto transition = TransitionFade::create(0.5, scene);
            Director::getInstance()->replaceScene(transition);
            
            
            SharedCueSheet::getInstance()->getCueSheet()->stop(_jingleId);
            criAtomExPlayer_Stop(ADX2::Manager::getInstance()->getDefaultPlayer());
            
        }),  NULL));
        
        return true;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}