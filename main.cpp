#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const float GRAVITY = 0.1f;
const float FRICTION = 0.90f;
const float BOUNCE_DAMPING = 0.7f;
const float EXPLOSION_FORCE = 5.0f;

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;

    Particle(float x, float y, float radius, sf::Color color, sf::Vector2f vel)
        : velocity(vel) {
        shape.setRadius(radius);
        shape.setFillColor(color);
        shape.setPosition(x, y);
        shape.setOrigin(radius, radius);
    }

    void update() {
        velocity.y += GRAVITY;
        velocity.x *= FRICTION;
        shape.move(velocity);
    }
};

void addParticle(std::vector<Particle>& particles, float x, float y) {
    float radius = 5;
    sf::Color color = sf::Color::White;
    sf::Vector2f velocity(0, 0);
    particles.emplace_back(x, y, radius, color, velocity);
}

void handleCollisions(std::vector<Particle>& particles) {
    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = i + 1; j < particles.size(); ++j) {
            sf::Vector2f pos1 = particles[i].shape.getPosition();
            sf::Vector2f pos2 = particles[j].shape.getPosition();
            float distance = std::sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) +
                                       (pos2.y - pos1.y) * (pos2.y - pos1.y));

            float radiusSum = particles[i].shape.getRadius() + particles[j].shape.getRadius();

            if (distance < radiusSum) {
                sf::Vector2f collisionDir = pos1 - pos2;
                collisionDir /= std::sqrt(collisionDir.x * collisionDir.x + collisionDir.y * collisionDir.y);

                particles[i].velocity += collisionDir;
                particles[j].velocity -= collisionDir;
            }
        }
    }
}

void applyExplosion(std::vector<Particle>& particles, sf::Vector2f position) {
    for (auto& particle : particles) {
        sf::Vector2f dir = particle.shape.getPosition() - position;
        float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (distance < 100) {
            dir /= distance;
            particle.velocity += dir * EXPLOSION_FORCE;
        }
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::VideoMode mode(1000, 800);
    sf::RenderWindow window;
    window.create(mode, "Sistema de Partículas fechados", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    std::vector<Particle> particles;

    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\franc\\CLionProjects\\game\\Arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return 1;
    }

    sf::Text particleCountText;
    particleCountText.setFont(font);
    particleCountText.setCharacterSize(20);
    particleCountText.setFillColor(sf::Color::White);
    particleCountText.setPosition(10, 10);

    for (int i = 0; i < 10; ++i) {
        addParticle(particles, 400 + std::rand() % 50 - 25, 50);
    }

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                addParticle(particles, 400 + std::rand() % 50 - 25, 50);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                applyExplosion(particles, mousePos);
            }
        }

        for (auto& particle : particles) {
            particle.update();

            if (particle.shape.getPosition().x <= 0 || particle.shape.getPosition().x + particle.shape.getRadius() * 2 >= window.getSize().x) {
                particle.velocity.x = -particle.velocity.x;
            }
            if (particle.shape.getPosition().y + particle.shape.getRadius() * 2 >= window.getSize().y) {
                particle.velocity.y = -particle.velocity.y * BOUNCE_DAMPING;
                particle.shape.setPosition(particle.shape.getPosition().x, window.getSize().y - particle.shape.getRadius() * 2);
            }
        }

        handleCollisions(particles);

        // Atualiza o texto do contador de partículas
        particleCountText.setString("Particulas: " + std::to_string(particles.size()));

        window.clear(sf::Color::Black);
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }
        window.draw(particleCountText); // Desenha o contador de partículas
        window.display();
    }

    return 0;
}
