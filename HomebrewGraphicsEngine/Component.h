#pragma once
class Component
{
public:
	virtual ~Component() = default;

	virtual void control(float dt) = 0;

	virtual void update(float dt) = 0;
};

