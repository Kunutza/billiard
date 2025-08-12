#pragma once
#include "physics.hpp"
#include <cstdint>
#include <string>

class BallObject: public PhysicObject {
public:

    BallObject(const Vec2 position, const std::string& number)
        : PhysicObject{position}, number{number} {}
    virtual ~BallObject() = default;

public:
    const std::string& number;

    // const float friction = 0.2f;

    // spin
    // shape/hitbox (but I know that it is a circle, so hitbox is hit according to radius)

    // TODO should when I hit all the balls in the start, that they end up in the same place?
};



/* TODO get those into a seperate file, they will not have physics in them */

/* A regular ball can not be shot by the cue */
class RegularBall : public BallObject {

public:
    RegularBall(const Vec2 position, const std::string& number)
        : BallObject{position, number} {}

};

// A white ball CAN be shot by the cue
class WhiteBall : public BallObject {

public:
    WhiteBall(const Vec2 position, const std::string& number)
       : BallObject(position, number) {}

};
