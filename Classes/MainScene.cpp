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
    
    return true;
}

Block* MainScene::getBlockAt(cocos2d::Vec2 position)
{
    auto key = StringUtils::format("%d-%d", static_cast<int>(position.x), static_cast<int>(position.y));
    return _blocks.at(key);
}

void MainScene::addBlock(Block *block)
{
    auto position = block->getBlockPosition();
    auto key = StringUtils::format("%d-%d", static_cast<int>(position.x), static_cast<int>(position.y));
    _blocks.insert(key, block);
}