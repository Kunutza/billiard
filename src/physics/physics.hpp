#pragma once
#include <memory>
#include "collision_grid.hpp"
#include "physic_object.hpp"
#include "ball_object.hpp"
#include "engine/common/utils.hpp"
#include "engine/common/index_vector.hpp"


struct PhysicSolver
{
    CIVector<std::unique_ptr<BallObject>>   objects;
    CollisionGrid          grid;
    Vec2                   world_size;
    Vec2                   gravity = {0.0f, 0.0f};

    // Simulation solving pass count
    uint32_t        sub_steps;

    PhysicSolver(IVec2 size)
        : grid{size.x, size.y}
        , world_size{to<float>(size.x), to<float>(size.y)}
        , sub_steps{8}
    {
        grid.clear();
    }

    // // Checks if two atoms are colliding and if so create a new contact
    // void solveContact(uint32_t atom_1_idx, uint32_t atom_2_idx)
    // {
    //     constexpr float response_coef = 1.0f;
    //     constexpr float eps           = 0.0001f;
    //     PhysicObject& obj_1 = *objects.data[atom_1_idx];
    //     PhysicObject& obj_2 = *objects.data[atom_2_idx];
    //     const Vec2 o2_o1  = obj_1.position - obj_2.position;
    //     const float dist2 = o2_o1.x * o2_o1.x + o2_o1.y * o2_o1.y;
    //     if (dist2 < 1.0f && dist2 > eps) {
    //         const float dist          = sqrt(dist2);
    //         // Radius are all equal to 1.0f
    //         const float delta  = response_coef * 0.5f * (1.0f - dist);
    //         const Vec2 col_vec = (o2_o1 / dist) * delta;
    //         obj_1.position += col_vec;
    //         obj_2.position -= col_vec;
    //     }
    // }

    void solveContact(uint32_t atom_1_idx, uint32_t atom_2_idx)
    {
        constexpr float response_coef = 1.0f;
        constexpr float eps = 0.0001f;

        BallObject& obj_1 = *objects.data[atom_1_idx];
        BallObject& obj_2 = *objects.data[atom_2_idx];

        const Vec2 o2_o1 = obj_1.position - obj_2.position;
        const float dist2 = o2_o1.x * o2_o1.x + o2_o1.y * o2_o1.y;

        const float radius_sum = obj_1.radius + obj_2.radius;
        const float radius_sum_squared = radius_sum * radius_sum;

        if (dist2 < radius_sum_squared && dist2 > eps)
        {
            const float dist = std::sqrt(dist2);
            const float delta = response_coef * 0.5f * (radius_sum - dist);
            const Vec2 col_vec = (o2_o1 / dist) * delta;
            obj_1.position += col_vec;
            obj_2.position -= col_vec;
        }
    }

    void checkAtomCellCollisions(uint32_t atom_idx, const CollisionCell& c)
    {
        for (uint32_t i{0}; i < c.objects_count; ++i) {
            solveContact(atom_idx, c.objects[i]);
        }
    }

void processCell(const CollisionCell& c, uint32_t index)
{
    for (uint32_t i = 0; i < c.objects_count; ++i) {
        const uint32_t atom_idx = c.objects[i];

        // Get cell position in 2D grid
        const uint32_t x = index / grid.height;
        const uint32_t y = index % grid.height;

        // Loop through neighboring cells (3x3 block)
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                const int32_t nx = static_cast<int32_t>(x) + dx;
                const int32_t ny = static_cast<int32_t>(y) + dy;

                // Skip out-of-bounds neighbors
                if (nx < 0 || nx >= static_cast<int32_t>(grid.width) ||
                    ny < 0 || ny >= static_cast<int32_t>(grid.height)) {
                    continue;
                }

                const uint32_t neighbor_index = nx * grid.height + ny;
                checkAtomCellCollisions(atom_idx, grid.data[neighbor_index]);
            }
        }
    }
}

    void solveCollisions()
    {
        uint32_t i{0};
        for (auto& cell : grid.data) {
            processCell(cell, i);
            ++i;
        }
    }

    // Add a new object to the solver
    uint64_t addObject(std::unique_ptr<BallObject> object)
    {
        return objects.push_back(std::move(object));
    }

    // Add a new object to the solver
    uint64_t createObject(Vec2 pos, const std::string& num)
    {
        return addObject(std::make_unique<BallObject>(pos, num));
        // return objects.emplace_back(pos);
    }

    void addObjectsToGrid()
    {
        grid.clear();
        uint32_t i{0};

        for (const auto& obj_ptr : objects.data) {
            const BallObject& obj = *obj_ptr;

            // Calculate the bounding box of the object based on its radius
            const int min_x = std::max(0, to<int32_t>(obj.position.x - obj.radius));
            const int max_x = std::min(grid.width  - 1, to<int32_t>(obj.position.x + obj.radius));
            const int min_y = std::max(0, to<int32_t>(obj.position.y - obj.radius));
            const int max_y = std::min(grid.height - 1, to<int32_t>(obj.position.y + obj.radius));

            // Insert the object into all overlapping grid cells
            for (int x = min_x; x <= max_x; ++x) {
                for (int y = min_y; y <= max_y; ++y) {
                    grid.addAtom(x, y, i);
                }
            }

            ++i;
        }
    }

    void updateObjects(float dt)
    {
        for (auto& obj : objects) {
            // Add gravity
            obj->acceleration += gravity;
            // Apply Verlet integration
            obj->update(dt);

            // Apply map borders collisions
            constexpr float padding = 0.f;
            constexpr float margin = PhysicObject::radius + padding;
    
            /* 
             * If ball touches both x and y border at the same time then its
             * velocity goes to 0. But we do not care about that since this is
             * pool
             */
            if (obj->position.x > world_size.x - margin) {
                const Vec2 vel = obj->getVelocity();
                obj->position.x = world_size.x - margin;
                obj->addVelocity({-vel.x, 0});
            } else if (obj->position.x < margin) {
                const Vec2 vel = obj->getVelocity();
                obj->position.x = margin;
                obj->addVelocity({-vel.x, 0});
            }
    
            if (obj->position.y > world_size.y - margin) {
                const Vec2 vel = obj->getVelocity();
                obj->position.y = world_size.y - margin;
                obj->addVelocity({0, -vel.y});
            } else if (obj->position.y < margin) {
                const Vec2 vel = obj->getVelocity();
                obj->position.y = margin;
                obj->addVelocity({0, -vel.y});
            }
        }
    }

    void update(float dt)
    {
        // Perform the sub steps
        const float sub_dt = dt / static_cast<float>(sub_steps);
        for (uint32_t i(sub_steps); i--;) {
            addObjectsToGrid();
            solveCollisions();
            updateObjects(sub_dt);
        }
    }
};
