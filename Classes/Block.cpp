//
//  Block.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "Block.h"

int Block::size = BLOCK_SIZE;

USING_NS_CC;

bool Block::init()
{
    
    auto color = rand() % 5;
    _blockColor = static_cast<BlockColor>(color);
    if (!Sprite::initWithFile("blocks.png", Rect(Block::size * color, 0, Block::size, Block::size))) {
        return false;
    }

    return true;
}