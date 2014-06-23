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

MainScene::MainScene() : _currentEntity(nullptr)
{
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_currentEntity);
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
            log("%f, %f", entity->getEntityPosition().x, entity->getEntityPosition().y);
        }
        this->setCurrentEntity(entity);
        return true;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        auto nextEntity = this->getEntityAt(touch->getLocation());
        if (_currentEntity != nullptr && nextEntity != nullptr && _currentEntity != nextEntity) {
            auto cp = _currentEntity->getEntityPosition();
            auto np = nextEntity->getEntityPosition();
            if (cp.y == np.y && cp.x + 1 == np.x) { // 右方向
                this->swapEntities(_currentEntity, nextEntity);
                this->setCurrentEntity(nullptr);
            }
        }
        
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        this->setCurrentEntity(nullptr);
    };
    listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        this->setCurrentEntity(nullptr);
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
    auto key = entity->getKey();
    _entitys.insert(key, entity);
}

bool MainScene::moveEntity(Entity *entity, cocos2d::Vec2 entityPosition)
{
    if (this->getEntityAt(entityPosition.x, entityPosition.y)) {
        return false;
    }
    _entitys.erase(entity->getKey());
    entity->setEntityPosition(entityPosition);
    _entitys.insert(entity->getKey(), entity);
    return true;
}

bool MainScene::swapEntities(Entity *entity0, Entity *entity1)
{
    const auto duration = 0.2;
    
    auto currentPosition0 = entity0->getEntityPosition();
    auto currentPosition1 = entity1->getEntityPosition();
    
    entity0->runAction(Sequence::create(MoveTo::create(duration, entity1->getPosition()),
                                        CallFuncN::create([=](Node *node) {
        auto entity = dynamic_cast<Entity *>(node);
        this->moveEntity(entity, currentPosition1);
    }), NULL));
    entity1->runAction(Sequence::create(MoveTo::create(duration, entity0->getPosition()),
                                        CallFuncN::create([=](Node *node) {
        auto entity = dynamic_cast<Entity *>(node);
        this->moveEntity(entity, currentPosition0);
    }),
                                        NULL));
    return true;
}