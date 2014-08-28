//
//  Cue.cpp
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#include "Cue.h"

namespace ADX2 {
    
    Cue * Cue::_instance = nullptr;
    
    Cue * Cue::getInstance()
    {
        if (!_instance) {
            _instance = new Cue();
        }
        return _instance;
    }
    
    void Cue::destroyInstance()
    {
        CC_SAFE_RELEASE_NULL(_instance);
    }
    
    Cue::Cue()
    : _acb(nullptr)
    {
    }
    
    Cue * Cue::loadWithCueFile(const char *acf, const char *acb)
    {
        return this->loadWithCueFile(acf, acb, "");
    }
    
    Cue * Cue::loadWithCueFile(const char *acf, const char *acb, const char *awb)
    {
        if (_acb) {
            criAtomExAcb_Release(_acb);
        }
        
        auto util = cocos2d::FileUtils::getInstance();
        
        auto acfPath = util->fullPathForFilename(acf);
        auto acbPath = util->fullPathForFilename(acb);
        
        criAtomEx_RegisterAcfFile(NULL, acfPath.c_str(), NULL, 0);
        if (awb) {
            auto awbPath = util->fullPathForFilename(awb);
            _acb = criAtomExAcb_LoadAcbFile(NULL, acbPath.c_str(), NULL, awbPath.c_str(), NULL, 0);
        } else {
            _acb = criAtomExAcb_LoadAcbFile(NULL, acbPath.c_str(), NULL, NULL, NULL, 0);
        }
        
        return this;
    }
    
    Cue::~Cue()
    {
        criAtomExAcb_Release(_acb);
    }
    
    CriAtomExPlaybackId Cue::playCueByID(CriAtomExCueId cueID)
    {
        auto player = ADX2Manager::getInstance()->getDefaultPlayer();
        criAtomExPlayer_SetCueId(player, _acb, cueID);
        
        int64_t playbackID = criAtomExPlayer_Start(player);
        
        return playbackID;
    }
    
    void Cue::stop(CriAtomExPlaybackId playbackID)
    {
        criAtomExPlayback_Stop(playbackID);
    }
    
    const char* Cue::getCueName(CriAtomExCueId cueID)
    {
        return criAtomExAcb_GetCueNameById(_acb, cueID);
    }
    
    int64_t Cue::getTime(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetTime(playbackID);
    }
    
    CriAtomExPlaybackStatus Cue::getStatus(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetStatus(playbackID);
    }
    
}