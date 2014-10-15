//
//  LicenseScene.h
//  KawazCrash
//
//  Created by giginet on 2014/10/14.
//
//

#ifndef __KawazCrash__LicenseScene__
#define __KawazCrash__LicenseScene__

#include "cocos2d.h"
#include "ADX2Manager.h"

#include "SS5Player.h"

class LicenseScene : cocos2d::Layer
{
protected:
    LicenseScene();
    virtual ~LicenseScene();
    bool init() override;
private:
    void gotoTitle();
public:
    static cocos2d::Scene * createScene();
    
    void onEnterTransitionDidFinish() override;
    
    void update(float dt);
    
    CC_SYNTHESIZE_RETAIN(ss::Player *, _player, Player);
    CREATE_FUNC(LicenseScene);
};

#endif /* defined(__KawazCrash__LicenseScene__) */
