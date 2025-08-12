#pragma once
#include <SFML/Graphics.hpp>
#include "physics/physics.hpp"
#include "engine/window_context_handler.hpp"
#include <map>
#include <string>


struct Renderer
{
    PhysicSolver& solver;

    sf::VertexArray world_va;
    std::map<std::string, sf::Texture> textures;
    /* Each texture in unique for now (VA not needed) but whatever */
    std::map<std::string, sf::VertexArray> objects_va_map;

    explicit
    Renderer(PhysicSolver& solver_);

    void render(RenderContext& context);

    void initializeWorldVA();

    void initializeBallsTextures();

    void updateParticlesVA();

    void renderHUD(RenderContext& context);

    void loadTexture(const std::string& name, const std::string& path);
};
