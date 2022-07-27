#pragma once

namespace Hogra {

	class Component
	{
	public:
		virtual ~Component() = default;

		virtual void Control(float dt) = 0;

		virtual void Update(float dt) = 0;
	};

}