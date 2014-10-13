//
//  TitleScene.h
//  KawazCrash
//
//  Created by giginet on 2014/10/14.
//
//

#ifndef __KawazCrash__TitleScene__
#define __KawazCrash__TitleScene__

#include "cocos2d.h"
#include "ADX2Manager.h"

class TitleScene : cocos2d::Layer
{
 protected:
    TitleScene();
    virtual ~TitleScene();
    bool init() override;
 public:
    static cocos2d::Scene * createScene();
    
    void onEnter() override;
    
    CC_SYNTHESIZE_RETAIN(ADX2::CueSheet *, _cueSheet, CueSheet);
    CREATE_FUNC(TitleScene);
};

#endif /* defined(__KawazCrash__TitleScene__) */
