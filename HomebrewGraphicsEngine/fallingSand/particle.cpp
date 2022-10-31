#include "particle.h"


namespace Hogra::FallingSand {
	
	std::mt19937 RandomEngine::rng = std::mt19937(std::random_device{}());

	void Particle::Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt) {
	}

	void Particle::MarchAlongLine(const glm::ivec2& start, const glm::vec2& direction, std::function<bool(unsigned int, unsigned int, float)> func, float maxDistance) {
		float x = (float)start.x;
		float y = (float)start.y;
		float divider = 1.0f;
		if (fabsf(direction.x) > fabsf(direction.y))
		{
			divider = std::fabsf(direction.x);
		}
		else {
			divider = std::fabsf(direction.y);
		}
		float xStep = direction.x / divider;
		float yStep = direction.y / divider;
		float distanceTravelled = 0.0f;
		float stepLength = std::sqrtf(xStep * xStep + yStep * yStep);
		while (true) {
			distanceTravelled += stepLength;
			if (distanceTravelled > maxDistance) {
				break;
			}
			x += xStep;
			y += yStep;
			if (!func((unsigned int)x, (unsigned int)y, distanceTravelled)) {
				break;
			}
		}
	}

}
