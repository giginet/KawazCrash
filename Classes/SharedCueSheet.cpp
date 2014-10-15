//
//  SharedCueSheet.cpp
//  KawazCrash
//
//  Created by giginet on 2014/10/15.
//
//

#include "SharedCueSheet.h"

SharedCueSheet * SharedCueSheet::_shared = nullptr;

SharedCueSheet::SharedCueSheet()
: _sheet(nullptr)
{
}

SharedCueSheet::~SharedCueSheet()
{
    _sheet->release();
}

SharedCueSheet * SharedCueSheet::getInstance()
{
    if (_shared != nullptr) {
        return _shared;
    }
    return nullptr;
}

void SharedCueSheet::initialize(const char *acf, const char *acb)
{
    if (_shared == nullptr) {
        _shared = new SharedCueSheet();
        _shared->_sheet = ADX2::CueSheet::create(acf, acb);
        _shared->_sheet->retain();
    }
}