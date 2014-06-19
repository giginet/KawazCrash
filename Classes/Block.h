//
//  Block.h
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#ifndef __KawazCrash__Block__
#define __KawazCrash__Block__

#include "cocos2d.h"

class Block :public cocos2d::Sprite
{
    bool init() override;
    CREATE_FUNC(Block);
};

#endif /* defined(__KawazCrash__Block__) */
