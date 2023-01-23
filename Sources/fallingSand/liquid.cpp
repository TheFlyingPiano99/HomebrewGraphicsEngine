#include "liquid.h"
#include "smoke.h"
#include "stone.h"

namespace Hogra::FallingSand {

	bool Liquid::Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt) {
		if (lifeTime > 0.0f && age > lifeTime) {
			grid.Write(actualX, actualY, nullptr);
			return false;
		}
		age += dt;

		// Change phase of matter:
		if (temperature > boilingTemperature) {
			auto p = gasConstructor();
			p->SetTemperature(temperature);
			grid.Write(actualX, actualY, p);
			return false;
		}
		else if (temperature < freezingTemperature) {
			auto p = solidConstructor();
			p->SetTemperature(temperature);
			grid.Write(actualX, actualY, p);
			return false;
		}
		
		RadiateHeat(grid, x, y, dt);
	
		auto start = glm::ivec2(x, y);
		if (nullptr == grid(x, y - 1) || grid(x, y - 1)->GetType() == Type::GasParticle
			|| nullptr == grid(x - 1, y - 1) || grid(x - 1, y - 1)->GetType() == Type::GasParticle
			|| nullptr == grid(x + 1, y - 1) || grid(x + 1, y - 1)->GetType() == Type::GasParticle
		) {
			velocity += 9.48f * glm::vec2(0.0f, -1.0f) * density * dt;
		}
		else if (nullptr != grid(x - 1, y) && grid(x - 1, y)->GetType() != Type::GasParticle 
			&& nullptr != grid(x + 1, y) && grid(x + 1, y)->GetType() != Type::GasParticle
		) {
			return true;
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
			else if (Type::GasParticle != grid(_x, _y)->GetType()) {
				foundObstacle = true;
				return false; // Break search for empty: found obstacle
			}
			else if (Type::GasParticle == grid(_x, _y)->GetType()) {
				xEmpty = _x;
				yEmpty = _y;
				return true;
			}
			return true;
		};
		// Try to move down:
		glm::vec2 dir = glm::vec2(0.0, -1.0);
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return true;
		}
		bool rand_bool = std::uniform_int_distribution<>{ 0, 1 }(RandomEngine::rng);
		bool tryLeftFirst = rand_bool;

		// Try to move down-left:
		dir = normalize((tryLeftFirst)? glm::vec2(-1.0, -1.0) : glm::vec2(+1.0, -1.0));
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return true;
		}
		// Try to move down-right:
		dir = normalize((tryLeftFirst) ? glm::vec2(+1.0, -1.0) : glm::vec2(-1.0, -1.0));
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return true;
		}
		
		
		// Try to move left:
		dir = normalize((tryLeftFirst) ? glm::vec2(-1.0, 0.0) : glm::vec2(+1.0, 0.0));
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return true;
		}
		// Try to move right:
		dir = normalize((tryLeftFirst) ? glm::vec2(+1.0, 0.0) : glm::vec2(-1.0, 0.0));
		MarchAlongLine(start, dir, func, l);
		if (x != xEmpty || y != yEmpty) {
			grid.SwapElements(x, y, xEmpty, yEmpty);
			velocity = length(velocity) * dir;
			return true;
		}
		
		unperformedStep = step;
		return true;
	}
}