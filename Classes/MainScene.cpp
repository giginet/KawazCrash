//
//  MainScene.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "MainScene.h"
#include "Entity.h"

USING_NS_CC;

const int HORIZONTAL_COUNT = 9;
const int VERTICAL_COUNT = 12;

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
            auto entity = Entity::create();
            entity->setEntityPosition(Vec2(x, y));
            _stage->addChild(entity);
            this->addEntity(entity);
        }
    }
    auto winSize = Director::getInstance()->getWinSize();
    auto leftMargin = winSize.width - Entity::getSize() * HORIZONTAL_COUNT;
    _stage->setPosition(Vec2(leftMargin / 2 + Entity::getSize() / 2.0, 30));
    this->addChild(_stage);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto position = touch->getLocation();
        auto entity = this->getEntityAt(position);
        if (entity) {
            entity->runAction(Sequence::create(ScaleTo::create(0.5, 0.5), ScaleTo::create(0.5, 1.0), NULL));
        }
        return true;
    };
    listener->onTouchEnded = [](Touch* touch, Event* event) {
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

Entity* MainScene::getEntityAt(int x, int y)
{
    auto key = StringUtils::format("%d,%d", x, y);
    return _entitys.at(key);
}

Entity* MainScene::getEntityAt(cocos2d::Vec2 position)
{
    auto stagePoint = _stage->convertToNodeSpace(position);
    auto x = floor((stagePoint.x + Entity::getSize() / 2.0) / Entity::getSize());
    auto y = floor((stagePoint.y + Entity::getSize() / 2.0) / Entity::getSize());
    return this->getEntityAt(x, y);
}

void MainScene::addEntity(Entity *entity)
{
    auto position = entity->getEntityPosition();
    auto key = StringUtils::format("%d,%d", static_cast<int>(position.x), static_cast<int>(position.y));
    _entitys.insert(key, entity);
}