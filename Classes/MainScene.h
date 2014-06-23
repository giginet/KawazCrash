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

    /** 指定したブロックを追加します
     *  @param 追加するblock
     */
    void addBlock(Block *block);
    
    /** グリッド上の特定位置にあるブロックを取り出します
    *   何もなかった場合はnullptrを返します
    *   @param x x座標
    *   @param y y座標
    *   @return その位置にあるBlock、またはnullptr
    */
    Block* getBlockAt(int x, int y);

    /** 画面上の特定位置にあるブロックを取り出します
     *   何もなかった場合はnullptrを返します
     *   @param position 画面上の絶対座標
     *   @return その位置にあるBlock、またはnullptr
     */
    Block* getBlockAt(cocos2d::Vec2 position);
    
    CREATE_FUNC(MainScene);
    CC_SYNTHESIZE(cocos2d::Node*, _stage, Stage);
    CC_SYNTHESIZE(BlockMap, _blocks, Blocks);
};

#endif /* defined(__KawazCrash__MainScene__) */
