//
//  MainScene.h
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#ifndef __KawazCrash__MainScene__
#define __KawazCrash__MainScene__

#include "cocos2d.h"
#include "Block.h"

typedef cocos2d::Map<std::string, Block *> BlockMap;

class MainScene :public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    bool init() override;
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE(BlockMap, _blocks, Blocks);
};

#endif /* defined(__KawazCrash__MainScene__) */
