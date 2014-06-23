//
//  Entity.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "Entity.h"

USING_NS_CC;

bool Entity::init()
{
    
    auto color = rand() % 5;
    _entityColor = static_cast<Entity::Color>(color);
    if (!Sprite::initWithFile("blocks.png", Rect(Entity::getSize() * color,
                                                 0,
                                                 Entity::getSize(),
                                                 Entity::getSize()))) {
        return false;
    }
    
    return true;
}

void Entity::setEntityPosition(Vec2 position)
{
    CCASSERT(floor(position.x) == position.x || floor(position.y) == position.y, "position must contains integers");
    _entityPosition = position;
    this->setPosition(Vec2(Entity::getSize() * position.x, Entity::getSize() * position.y));
}

std::string Entity::getKey()
{
    auto key = StringUtils::format("%d,%d", static_cast<int>(_entityPosition.x), static_cast<int>(_entityPosition.y));
    return key;
}
