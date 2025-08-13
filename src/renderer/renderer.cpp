#include "renderer.hpp"
#include "physics/ball_object.hpp"
#include <iostream>

Renderer::Renderer(PhysicSolver& solver_)
    : solver{solver_}
    , world_va{sf::Quads, 4}
{
    initializeWorldVA();
    initializeBallsTextures();
}

void Renderer::render(RenderContext& context)
{
    renderHUD(context);

    // Draw background
    context.draw(world_va);

    // Update particle VA groups
    updateParticlesVA();

    // Draw each group with its associated texture
    for (const auto& [texture_name, va] : objects_va_map) {
        auto it = textures.find(texture_name);
        if (it == textures.end()) continue;

        sf::RenderStates states;
        states.texture = &it->second;
        context.draw(va, states);
    }
}

void Renderer::initializeWorldVA()
{
    world_va[0].position = {0.0f               , 0.0f};
    world_va[1].position = {solver.world_size.x, 0.0f};
    world_va[2].position = {solver.world_size.x, solver.world_size.y};
    world_va[3].position = {0.0f               , solver.world_size.y};

    const uint8_t level = 50;
    const sf::Color background_color{level, level, level};
    world_va[0].color = background_color;
    world_va[1].color = background_color;
    world_va[2].color = background_color;
    world_va[3].color = background_color;
}

void Renderer::initializeBallsTextures()
{
    for (int i = 0; i <= 15; ++i) {
        loadTexture(std::to_string(i), "res/" + std::to_string(i) + ".png");
    }
}

void Renderer::updateParticlesVA()
{
    objects_va_map.clear();

    //const float    texture_size  = 1024.0f;
    const float    texture_size  = 42.0f;
    const float    radius        = PhysicObject::radius;
    const uint64_t objects_count = solver.objects.size();

    for (uint64_t i = 0; i < objects_count; ++i) {
        const BallObject& object = *solver.objects[i];

        const std::string& tex_name = object.name;

        bool tex_exists = (textures.find(tex_name) != textures.end());

        auto& va = objects_va_map[tex_name];
        if (va.getPrimitiveType() != sf::Quads)
            va.setPrimitiveType(sf::Quads);

        const uint64_t idx = va.getVertexCount();
        va.resize(idx + 4);

        // Positions
        va[idx + 0].position = object.position + Vec2{-radius, -radius};
        va[idx + 1].position = object.position + Vec2{ radius, -radius};
        va[idx + 2].position = object.position + Vec2{ radius,  radius};
        va[idx + 3].position = object.position + Vec2{-radius,  radius};

        // Texture coords
        va[idx + 0].texCoords = {0.0f        , 0.0f};
        va[idx + 1].texCoords = {texture_size, 0.0f};
        va[idx + 2].texCoords = {texture_size, texture_size};
        va[idx + 3].texCoords = {0.0f        , texture_size};

        // Color
        /*const sf::Color color = object.color;
        va[idx + 0].color = color;
        va[idx + 1].color = color;
        va[idx + 2].color = color;
        va[idx + 3].color = color;
        */
    }
}

void Renderer::renderHUD(RenderContext&)
{
    // HUD (currently disabled)
    /*const float margin    = 20.0f;
    float       current_y = margin;
    text_time.setString("Simulation time: " + toString(phys_time.get()) + "ms");
    text_time.setPosition({margin, current_y});
    current_y += text_time.getBounds().y + 0.5f * margin;
    context.renderToHUD(text_time, RenderContext::Mode::Normal);

    text_objects.setString("Objects: " + toString(simulation.solver.objects.size()));
    text_objects.setPosition({margin, current_y});
    current_y += text_objects.getBounds().y + 0.5f * margin;
    context.renderToHUD(text_objects, RenderContext::Mode::Normal);*/
}

void Renderer::loadTexture(const std::string& name, const std::string& path)
{
    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture: " + path);
    }
    tex.generateMipmap();
    tex.setSmooth(true);
    textures[name] = std::move(tex);
}
