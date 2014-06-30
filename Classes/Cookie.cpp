//
//  Cookie.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "Cookie.h"
#include <random>

USING_NS_CC;

Cookie::Cookie() : _state(State::NORMAL), _debugLabel(nullptr)
{
}

Cookie::~Cookie()
{
    CC_SAFE_RELEASE_NULL(_debugLabel);
}

bool Cookie::init()
{
    std::random_device rdev;
    auto engine = std::mt19937(rdev());
    auto dist = std::uniform_int_distribution<>(0, 4);
    auto color = dist(engine);
    _cookieColor = static_cast<Cookie::Color>(color);
    if (!Sprite::initWithFile("blocks.png", Rect(Cookie::getSize() * color,
                                                 0,
                                                 Cookie::getSize(),
                                                 Cookie::getSize()))) {
        return false;
    }
    auto label = Label::createWithSystemFont("", "Helvetica", 10);
    this->addChild(label);
    this->setDebugLabel(label);
    label->setPosition(Vec2(Cookie::getSize() / 2.0, Cookie::getSize() / 2.0));
    
    return true;
}

void Cookie::setCookiePosition(Vec2 position)
{
    CCASSERT(floor(position.x) == position.x || floor(position.y) == position.y, "position must contains integers");
    _cookiePosition = position;
    _debugLabel->setString(this->getKey());
}

void Cookie::adjustPosition()
{
    auto position = _cookiePosition;
    this->setPosition(Vec2(Cookie::getSize() * position.x, Cookie::getSize() * position.y));
}

std::string Cookie::getKey()
{
    auto key = StringUtils::format("%d,%d", static_cast<int>(_cookiePosition.x), static_cast<int>(_cookiePosition.y));
    return key;
}
