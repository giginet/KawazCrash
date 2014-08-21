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

Cookie::Cookie() : _state(State::STATIC), _debugLabel(nullptr)
{
}

Cookie::~Cookie()
{
    CC_SAFE_RELEASE_NULL(_debugLabel);
}

bool Cookie::init()
{
    // 乱数発生器の初期化
    std::random_device rdev;
    auto engine = std::mt19937(rdev());
    auto dist = std::uniform_int_distribution<>(0, static_cast<int>(Cookie::Shape::COUNT) - 1);
    
    // クッキーの形をランダムに1つ選ぶ
    auto shape = dist(engine);
    _cookieShape = static_cast<Cookie::Shape>(shape);
    
    // クッキーの形を元にSpriteを初期化
    if (!Sprite::initWithFile("blocks.png", Rect(Cookie::getSize() * shape,
                                                 0,
                                                 Cookie::getSize(),
                                                 Cookie::getSize()))) {
        return false;
    }
    
    
    auto label = Label::createWithSystemFont("", "Helvetica", 12);
    //this->addChild(label);
    this->setDebugLabel(label);
    label->setPosition(Vec2(Cookie::getSize() / 2.0, Cookie::getSize() / 2.0));
    label->setColor(Color3B::BLUE);
    
    return true;
}

void Cookie::setCookiePosition(Vec2 position)
{
    // もし、CookiePositionにfloatが含まれていたらassertする
    CCASSERT(floor(position.x) == position.x || floor(position.y) == position.y, "position must contains integers");
    _cookiePosition = position;
    _debugLabel->setString(this->getDescription());
}

void Cookie::adjustPosition()
{
    auto position = _cookiePosition;
    // _cookiePositionを元にpositionを設定する
    this->setPosition((position + Vec2::ONE * 0.5) * Cookie::getSize());
}

std::string Cookie::getDescription()
{
    auto key = StringUtils::format("%d,%d", static_cast<int>(_cookiePosition.x), static_cast<int>(_cookiePosition.y));
    return key;
}
