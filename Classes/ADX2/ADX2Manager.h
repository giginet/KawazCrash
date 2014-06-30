//
//  ADX2Manager.h
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#ifndef __CcAdx2Basic__ADX2Manager__
#define __CcAdx2Basic__ADX2Manager__

#include <iostream>
#include "cocos2d.h"
#include "cri_adx2le.h"
#include "Cue.h"

namespace ADX2 {
    
    class ADX2Manager {
     private:
        static ADX2Manager* _instance;
        /// ボイスプールのハンドル
        CriAtomExVoicePoolHn _voicePool;
        /// 音声プレーヤのハンドル
	CriAtomExPlayerHn _player;
        /// D-BASのID
	CriAtomDbasId _dbasID;
      
        ADX2Manager(CriAtomExPlayerConfig playerConfig,
                    CriAtomExStandardVoicePoolConfig voicePoolConfig);
     public:
        friend Cue;
        
        virtual ~ADX2Manager();
        
        static ADX2Manager* initialize();
        static ADX2Manager* initialize(CriAtomExPlayerConfig playerConfig, CriAtomExStandardVoicePoolConfig voicePoolConfig);
        static ADX2Manager* getInstance();
        void update();
        void stopAll();
        int getVoiceNum();
        static void finalize();
    };
    
}

#endif /* defined(__CcAdx2Basic__ADX2Manager__) */
