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

const int BLOCK_SIZE = 33;

class Block :public cocos2d::Sprite
{
public:

    enum class BlockColor {
        RED,
        YELLOW,
        GREEN,
        LIGHTBLUE,
        BLUE
    };

    static int size;
    bool init() override;
    CC_SYNTHESIZE(BlockColor, _blockColor, BlockColor);
    CREATE_FUNC(Block);
};

#endif /* defined(__KawazCrash__Block__) */
