#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "engine/window_context_handler.hpp"
#include "engine/common/color_utils.hpp"

#include "physics/ball_object.hpp"
#include "physics/physic_object.hpp"
#include "physics/physics.hpp"
#include "renderer/renderer.hpp"


#if 0

https://github.com/johnBuffer/VerletSFML-Multithread/tree/single-thread, I used
the singlethreaded branch as you can see
https://www.youtube.com/watch?v=9IULfQH7E90, implementation using multithreading


TODO make the particles less
TODO remove multithreading
TODO make ballobject from physicsobject
TODO fix physics.hpp addObjectToGrid, updateObjects
TODO change map border collisions
TODO fullscreen mode
TODO add ball textures for each ball
TODO fix ball collisions with walls

TODO REMOVE GRAVITY (I did not remove. just set it to {0.0f, 0.0f}). Should 
I remove it completely?
TODO const float VELOCITY_DAMPING = 40.0f; // arbitrary, approximating air 
friction, I may need that
TODO am I using substeps? yes, 8 at physics.hpp

TODO add friction (using std::max if vel drops under thresshold then friction 
is constant)
TODO add spin and depict it

TODO what happens with the fps and the time intervals? what if fps is irregular
TODO what happens with the floating point precision?
TODO is there any error correction in here? most likely there isnt, the 
error may always be the same so I get the same results every time, and that
may mean that someone with another machine may get different results

TODO since number of balls is small and fixed I could just use an array

TODO search for all the todos

TODO 
valgrind --tool=callgrind --callgrind-out-file=callgrind.out.program ./program
callgrind_annotate callgrind.out.program
kcachegrind callgrind.out.program

#endif

#if 0

Heres how you could potentially reduce the overhead:

1. Avoid unnecessary iteration:
    Only clear cells that are actually used.
    Keep a list of "active" or "dirty" cells and reset just those.

2. Use raw pointers or indices (with caution):
    STL iterators are safe but can be heavier than raw pointers or direct indexing.
    Rewriting critical loops with index-based access can reduce overhead, especially in hot paths.

3. Parallel clear:
    If not already done, CollisionGrid::clear() could be multithreaded since each cell can likely be cleared independently.

4. Custom container:
    If you know the data layout and usage patterns well, you could replace std::vector<CollisionCell> with a custom structure optimized for clearing quickly.

#endif

int main()
{
    const uint32_t window_width  = 1920;
    const uint32_t window_height = 1080;
    WindowContextHandler app("Verlet", sf::Style::Fullscreen);
    RenderContext& render_context = app.getRenderContext();
    // Initialize solver and renderer

    const IVec2 world_size{100, 100};
    PhysicSolver solver{world_size};
    Renderer renderer(solver);

    const float margin = 20.0f;
    const auto  zoom   = static_cast<float>(window_height - margin) / static_cast<float>(world_size.y);
    render_context.setZoom(zoom);
    render_context.setFocus({world_size.x * 0.5f, world_size.y * 0.5f});

    bool emit = true;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {
        emit = !emit;
    });

    constexpr uint32_t fps_cap = 60;
    int32_t target_fps = fps_cap;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::S, [&](sfev::CstEv) {
        target_fps = target_fps ? 0 : fps_cap;
        app.setFramerateLimit(target_fps);
    });

    // Create all the balls in the scene
    for (uint32_t i{0}; i < 16; i++) {
        
        const auto id = solver.createObject(
                {
                PhysicObject::radius * 2 + PhysicObject::radius * i, 
                PhysicObject::radius * 2 + 2.1f * PhysicObject::radius * i
                },
                std::to_string(i)
                );
        // solver.objects[id].last_position.x -= 0.2f;
        solver.objects[id]->addVelocity({0.0f, 0.1f});
        solver.objects[id]->color = ColorUtils::getRainbow(id * 1.f);

        auto ptr = solver.objects[id].get();
    }

    // Main loop
    const float dt = 1.0f / static_cast<float>(fps_cap);
    while (app.run()) {

        solver.update(dt);

        render_context.clear();
        renderer.render(render_context);
        render_context.display();
    }

    return 0;
}
