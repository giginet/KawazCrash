//
//  Cue.cpp
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#include "Cue.h"
#include "ADX2Manager.h"

namespace ADX2 {
    
    Cue::Cue() : _acb(nullptr)
    {
    }
    
    Cue* Cue::create(const char *acf, const char *acb)
    {
        return Cue::create(acf, acb, nullptr);
    }
    
    Cue* Cue::create(const char *acf, const char *acb, const char *awb)
    {
        auto cue = new Cue();
        if (cue && cue->initWithFile(acf, acb, awb)) {
            cue->autorelease();
            return cue;
        }
        CC_SAFE_DELETE(cue);
        return nullptr;
    }
    
    bool Cue::initWithFile(const char* acf, const char* acb, const char* awb)
    {
        
        auto fp = [](const char* filename)
        {
            return cocos2d::FileUtils::getInstance()->fullPathForFilename(filename);
        };
        
        auto acfPath = fp(acf);
        auto acbPath = fp(acb);
        
        criAtomEx_RegisterAcfFile(NULL, acfPath.c_str(), NULL, 0);
        if (awb) {
            auto awbPath = fp(awb);
            _acb = criAtomExAcb_LoadAcbFile(NULL, acbPath.c_str(), NULL, awbPath.c_str(), NULL, 0);
        } else {
            _acb = criAtomExAcb_LoadAcbFile(NULL, acbPath.c_str(), NULL, NULL, NULL, 0);
        }
        
        if (_acb == nullptr) {
            return false;
        }
        
        return true;
    }
    
    Cue::~Cue()
    {
        criAtomExAcb_Release(_acb);
    }
    
    CriAtomExPlaybackId Cue::playCueByID(CriAtomExCueId cueID)
    {
        auto player = ADX2Manager::getInstance()->_player;
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