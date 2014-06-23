//
//  MainScene.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "MainScene.h"
#include "Block.h"

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
            auto block = Block::create();
            block->setBlockPosition(Vec2(x, y));
            _stage->addChild(block);
            this->addBlock(block);
        }
    }
    auto winSize = Director::getInstance()->getWinSize();
    auto leftMargin = winSize.width - Block::size * HORIZONTAL_COUNT;
    _stage->setPosition(Vec2(leftMargin / 2 + Block::size / 2.0, 30));
    this->addChild(_stage);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto position = touch->getLocation();
        auto block = this->getBlockAt(position);
        if (block) {
            block->runAction(Sequence::create(ScaleTo::create(0.5, 0.5), ScaleTo::create(0.5, 1.0), NULL));
        }
        return true;
    };
    listener->onTouchEnded = [](Touch* touch, Event* event) {
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

Block* MainScene::getBlockAt(int x, int y)
{
    auto key = StringUtils::format("%d,%d", x, y);
    return _blocks.at(key);
}

Block* MainScene::getBlockAt(cocos2d::Vec2 position)
{
    auto stagePoint = _stage->convertToNodeSpace(position);
    auto x = floor((stagePoint.x + Block::size / 2.0) / Block::size);
    auto y = floor((stagePoint.y + Block::size / 2.0) / Block::size);
    return this->getBlockAt(x, y);
}

void MainScene::addBlock(Block *block)
{
    auto position = block->getBlockPosition();
    auto key = StringUtils::format("%d,%d", static_cast<int>(position.x), static_cast<int>(position.y));
    _blocks.insert(key, block);
}