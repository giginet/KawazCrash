//
//  Block.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "Block.h"

USING_NS_CC;

bool Block::init()
{
    
    if (!Sprite::initWithFile("blocks.png", Rect(0, 0, 66, 66))) {
    }

    return true;
}