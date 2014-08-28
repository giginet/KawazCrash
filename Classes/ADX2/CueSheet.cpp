//
//  CueSheet.cpp
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#include "CueSheet.h"
#include "ADX2Manager.h"

namespace ADX2 {
    
    CueSheet::CueSheet() : _acb(nullptr)
    {
    }
    
    CueSheet* CueSheet::create(const char *acf, const char *acb)
    {
        return CueSheet::create(acf, acb, nullptr);
    }
    
    CueSheet* CueSheet::create(const char *acf, const char *acb, const char *awb)
    {
        auto cue = new CueSheet();
        if (cue && cue->initWithFile(acf, acb, awb)) {
            cue->autorelease();
            return cue;
        }
        CC_SAFE_DELETE(cue);
        return nullptr;
    }
    
    bool CueSheet::initWithFile(const char* acf, const char* acb, const char* awb)
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
    
    CueSheet::~CueSheet()
    {
        criAtomExAcb_Release(_acb);
    }
    
    CriAtomExPlaybackId CueSheet::playCueByID(CriAtomExCueId cueID)
    {
        auto player = ADX2Manager::getInstance()->getDefaultPlayer();
        criAtomExPlayer_SetCueId(player, _acb, cueID);
        
        CriAtomExPlaybackId playbackID = criAtomExPlayer_Start(player);
        
        return playbackID;
    }
    
    void CueSheet::stop(CriAtomExPlaybackId playbackID)
    {
        criAtomExPlayback_Stop(playbackID);
    }
    
    const char* CueSheet::getCueName(CriAtomExCueId cueID)
    {
        return criAtomExAcb_GetCueNameById(_acb, cueID);
    }
    
    int64_t CueSheet::getTime(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetTime(playbackID);
    }
    
    CriAtomExPlaybackStatus CueSheet::getStatus(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetStatus(playbackID);
    }
    
}