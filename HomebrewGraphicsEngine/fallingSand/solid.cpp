#include "solid.h"

namespace Hogra::FallingSand {

	void Solid::Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt)
	{
		auto start = glm::ivec2(x, y);
		if (nullptr == grid(x, y - 1) || grid(x, y - 1)->GetType() == Type::LiquidParticle) {
			velocity += 9.48f * glm::vec2(0.0f, -1.0f) * density * dt;
		}
		velocity *= 1.0f / exp(airResistance * dt);
		glm::vec2 step = unperformedStep + velocity * dt;
		float l = length(step);
		unperformedStep = { 0.0f, 0.0f };
		unsigned int xEmpty = x;
		unsigned int yEmpty = y;
		bool foundObstacle = false;
		std::function<bool(unsigned int, unsigned int, float)> func
			= [&](unsigned int _x, unsigned int _y, float _) {
			if (nullptr == grid(_x, _y)) {
				xEmpty = _x;
				yEmpty = _y;
				return true;	// Continue along path
			}
			else if (Type::LiquidParticle != grid(_x, _y)->GetType()) {
				foundObstacle = true;
				return false; // Break search for empty: found obstacle
			}
			else if (Type::LiquidParticle == grid(_x, _y)->GetType()) {
				xEmpty = _x;
				yEmpty = _y;
				return true; // Break search for empty: found obstacle
			}
			return true;
		};

		// Try to move down:
		glm::vec2 dir = glm::vec2(0.0, -1.0);
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return;
		}
		bool rand_bool = std::uniform_int_distribution<>{ 0, 1 }(RandomEngine::rng);
		bool tryLeftFirst = rand_bool;

		// Try to move down-left:
		dir = normalize((tryLeftFirst) ? glm::vec2(-1.0, -1.0) : glm::vec2(+1.0, -1.0));
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return;
		}
		// Try to move down-right:
		dir = normalize((tryLeftFirst) ? glm::vec2(+1.0, -1.0) : glm::vec2(-1.0, -1.0));
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return;
		}

		unperformedStep = step;
	}
}
