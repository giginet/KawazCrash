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
            if (cp.y == np.y && cp.x - 1 == np.x) { // 左方向
                this->swapEntities(_currentEntity, nextEntity);
                this->setCurrentEntity(nullptr);
            }
            if (cp.x == np.x && cp.y + 1 == np.y) { // 上方向
                this->swapEntities(_currentEntity, nextEntity);
                this->setCurrentEntity(nullptr);
            }
            if (cp.x == np.x && cp.y - 1 == np.y) { // 下方向
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
    this->scheduleUpdate();
    
    return true;
}

void MainScene::update(float dt)
{
    for (auto entity : _entities) {
        this->checkFall(entity);
    }
}

Entity* MainScene::getEntityAt(int x, int y)
{
    for (auto entity : _entities) {
        if (entity->getEntityPosition().x == x && entity->getEntityPosition().y == y) {
            return entity;
        }
    }
    return nullptr;
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
    _entities.pushBack(entity);
}

bool MainScene::moveEntity(Entity *entity, cocos2d::Vec2 entityPosition)
{
    entity->setEntityPosition(entityPosition);
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
        this->checkVanishEntities(entity);
    }), NULL));
    entity1->runAction(Sequence::create(MoveTo::create(duration, entity0->getPosition()),
                                        CallFuncN::create([=](Node *node) {
        auto entity = dynamic_cast<Entity *>(node);
        this->moveEntity(entity, currentPosition0);
        EntityVector v;
        auto entities = this->checkNeighborEntitied(entity, v);
        if (entities.size() >= 4) {
            for (auto entity : entities) {
                this->deleteEntity(entity);
            }
        }
    }),
                                        NULL));
    return true;
}

void MainScene::checkVanishEntities(Entity *entity)
{
    EntityVector v;
    auto entities = this->checkNeighborEntitied(entity, v);
    if (entities.size() >= 4) {
        for (auto entity : entities) {
            this->deleteEntity(entity);
        }
    }
}

void MainScene::deleteEntity(Entity *entity)
{
    entity->runAction(Sequence::create(FadeOut::create(0.5f),
                                       CallFuncN::create([this](Node* node) {
        auto entity = dynamic_cast<Entity *>(node);
        _entities.eraseObject(entity);
    }),
                                       RemoveSelf::create(),
                                       CallFunc::create([this] {
    }),
                                       NULL));
}

EntityVector MainScene::checkNeighborEntitied(Entity *entity, EntityVector checked) {
    if (checked.contains(entity)) {
        return checked;
    }
    checked.pushBack(entity);
    auto position = entity->getEntityPosition();
    auto up = this->getEntityAt(position.x, position.y + 1);
    auto down = this->getEntityAt(position.x, position.y - 1);
    auto left = this->getEntityAt(position.x - 1, position.y);
    auto right = this->getEntityAt(position.x + 1, position.y);
    
    if (up && up->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntitied(up, checked);
    }
    if (down && down->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntitied(down, checked);
    }
    if (left && left->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntitied(left, checked);
    }
    if (right && right->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntitied(right, checked);
    }
    return std::move(checked);
}

void MainScene::checkFall(Entity *entity)
{
    auto position = entity->getEntityPosition();
    if (position.y == 0) {
        return;
    }
    if (entity->getIsFalling()) {
        return;
    }
    auto downPosition = Vec2(position.x, position.y - 1);
    auto down = this->getEntityAt(position.x, position.y - 1);
    if (down == nullptr) {
        auto duration = 0.05;
        entity->setIsFalling(true);
        entity->runAction(Sequence::create(MoveBy::create(duration, Vec2(0, -Entity::getSize())),
                                           CallFuncN::create([this, downPosition] (Node *node) {
            auto entity = dynamic_cast<Entity *>(node);
            this->moveEntity(entity, downPosition);
            entity->setIsFalling(false);
            this->checkVanishEntities(entity);
            this->checkFall(entity);
        }),
                                           NULL));
    }
}