//
//  SharedCueSheet.h
//  KawazCrash
//
//  Created by giginet on 2014/10/15.
//
//

#ifndef __KawazCrash__SharedCueSheet__
#define __KawazCrash__SharedCueSheet__

#include <stdio.h>
#include "ADX2Manager.h"

class SharedCueSheet {
 public:
    SharedCueSheet();
    ~SharedCueSheet();
    static SharedCueSheet * getInstance();
    static void initialize(const char* acf, const char* acb);
    
    ADX2::CueSheet * getCueSheet()
    {
        return _sheet;
    }
    
 private:
    static SharedCueSheet * _shared;
    ADX2::CueSheet * _sheet;
};

#endif /* defined(__KawazCrash__SharedCueSheet__) */
