#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <functional>
#include <random>
#include "../MemoryManager.h"


namespace Hogra::FallingSand {

	class RandomEngine {
	public:
		static std::mt19937 rng;
	};

	class ParticleGrid;

	class Particle {
	public:

		enum class Type
		{
			SolidParticle,
			LiquidParticle,
			GasParticle,
			UndefinedParticle
		};

		virtual bool Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt);

		const glm::vec4& GetColor() const {
			return color;
		}

		void MarchAlongLine(const glm::ivec2& start, const glm::vec2& direction, std::function<bool(unsigned int x, unsigned int y, float distanceTravelled)> func, float maxDistance = 10.0f);

		Type GetType() const {
			return this->type;
		}

		float GetFriction() const {
			return friction;
		}

		void AddVelocity(const glm::vec2& _v) {
			velocity += _v;
		}

		void SetDebug(bool b) {
			isDebug = b;
		}

		float GetTemperature() const {
			return temperature;
		}

		void AddTemperature(float t) {
			temperature += t;
		}

		void SetTemperature(float t) {
			temperature = t;
		}

		void RadiateHeat(ParticleGrid& grid, unsigned int x, unsigned int y, float dt);

		void SetActualPosition(unsigned int _x, unsigned int _y) {
			actualX = _x;
			actualY = _y;
		}

	protected:
		glm::vec4 color;
		glm::vec2 unperformedStep = { 0.0f, 0.0f };	// When the delta time is too small and a frame is not enough to move between positions.
		glm::vec2 velocity = { 0.0f, 0.0f };
		unsigned int actualX = 0;
		unsigned int actualY = 0;
		float density = 0.0f;
		float airResistance = 0.001f;
		float friction = 0.1f;
		bool isDebug = false;
		float lifeTime = -1.0f;
		float age = 0.0f;
		float temperature = 26.0f;
		bool isStaticPosition = false;

		Type type = Type::UndefinedParticle;
	};

	class ParticleGrid {
	public:

		Particle* operator()(unsigned int x, unsigned int y) {
			return elements[PosToLinIdx(x, y)];
		}

		void Write(unsigned int x, unsigned int y, Particle* particle) {
			elements[PosToLinIdx(x, y)] = particle;
			if (nullptr != particle) {
				particle->SetActualPosition(x, y);
			}
		}

		const glm::ivec2& GetDimensions() {
			return dimensions;
		}

		unsigned int GetSize() {
			return elements.size();
		}

		inline unsigned int PosToLinIdx(unsigned int x, unsigned int y)
		{
			x = (x < dimensions.x) ? x : dimensions.x - 1;
			y = (y < dimensions.y) ? y : dimensions.y - 1;
			return x + dimensions.x * y;
		}

		void Update(float dt) {
			const auto elementsCopy = elements;
			std::vector<Particle*> toDelete;
			for (unsigned int y = 0; y < dimensions.y; y++) {
				for (unsigned int x = 0; x < dimensions.x; x++) {
					auto idx = PosToLinIdx(x, y);
					if (nullptr != elementsCopy[idx]) {
						if (!elementsCopy[idx]->Update(*this, x, y, dt)) {
							toDelete.push_back(elementsCopy[idx]);
						}
					}
				}
			}
			for (auto element : toDelete) {
				Allocator::Delete(element);
			}
		}

		void UpdateColorData() {
			for (unsigned int y = 0; y < dimensions.y; y++) {
				for (unsigned int x = 0; x < dimensions.x; x++) {
					auto element = this->operator()(x, y);
					if (nullptr != element) {
						colorData[PosToLinIdx(x, y)] = element->GetColor();
					}
					else {
						colorData[PosToLinIdx(x, y)] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
					}
				}
			}
		}

		const std::vector<glm::vec4>& GetColorData() const {
			return colorData;
		}

		void SwapElements(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
			auto temp = operator()(x2, y2);
			Write(x2, y2, operator()(x1, y1));
			Write(x1, y1, temp);
		}

		bool PutIfEmpty(unsigned int x, unsigned int y, Particle* particle) {
			if (nullptr == operator()(x, y)) {
				Write(x, y, particle);
				return true;
			}
			else {
				Allocator::Delete(particle);
				return false;
			}
		}

	private:
		glm::ivec2 dimensions = glm::ivec2(256, 256);
		std::vector<Particle*> elements = std::vector<Particle*>(256 * 256);
		std::vector<glm::vec4> colorData = std::vector<glm::vec4>(256 * 256);


	};

}