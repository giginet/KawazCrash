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
    
    /** ブロックのグリッド上の位置を指定します
    * @param position x, y位置を含んだ二次元ベクトル
    */
    void setBlockPosition(cocos2d::Vec2 position);
    
    CC_SYNTHESIZE_READONLY(cocos2d::Vec2, _blockPosition, BlockPosition);
    CC_SYNTHESIZE(BlockColor, _blockColor, BlockColor);
    CREATE_FUNC(Block);
};

#endif /* defined(__KawazCrash__Block__) */
