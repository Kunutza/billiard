#pragma once
#include <cstdint>
#include "physic_object.hpp"
#include <string>

class BallObject: public PhysicObject {
public:

    BallObject(const Vec2 position, const std::string name_)
        : PhysicObject{position}, name{name_} {}
    virtual ~BallObject() = default;

public:
    const std::string name;

    // const float friction = 0.2f;

    // spin
    // shape/hitbox (but I know that it is a circle, so hitbox is hit according to radius)

    // TODO should when I hit all the balls in the start, that they end up in the same place?
};



/* TODO get those into a seperate file, they will not have physics in them */

/* A regular ball can not be shot by the cue */
class RegularBall : public BallObject {

public:
    RegularBall(const Vec2 position, const std::string name)
        : BallObject{position, name} {}

};

// A white ball CAN be shot by the cue
class WhiteBall : public BallObject {

public:
    WhiteBall(const Vec2 position, const std::string name)
       : BallObject(position, name) {}

};
