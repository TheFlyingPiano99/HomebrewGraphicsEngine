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

		virtual void Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt);

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

	protected:
		glm::vec4 color;
		glm::vec2 unperformedStep = { 0.0f, 0.0f };	// When the delta time is too small and a frame is not enough to move between positions.
		glm::vec2 velocity = { 0.0f, 0.0f };
		float density = 0.0f;
		float airResistance = 0.01f;
		float friction = 0.1f;
		bool isDebug = false;

		Type type = Type::UndefinedParticle;
	};

	class ParticleGrid {
	public:

		Particle* operator()(unsigned int x, unsigned int y) {
			return elements[PosToLinIdx(x, y)];
		}

		void Write(unsigned int x, unsigned int y, Particle* particle) {
			elements[PosToLinIdx(x, y)] = particle;
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
			auto elementsCopy = elements;
			for (unsigned int y = 0; y < dimensions.y; y++) {
				for (unsigned int x = 0; x < dimensions.x; x++) {
					auto idx = PosToLinIdx(x, y);
					if (nullptr != elementsCopy[idx]) {
						elementsCopy[idx]->Update(*this, x, y, dt);
					}
				}
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
			}
			else {
				Allocator::Delete(particle);
			}
		}

	private:
		glm::ivec2 dimensions = glm::ivec2(256, 256);
		std::vector<Particle*> elements = std::vector<Particle*>(256 * 256);
		std::vector<glm::vec4> colorData = std::vector<glm::vec4>(256 * 256);


	};

}