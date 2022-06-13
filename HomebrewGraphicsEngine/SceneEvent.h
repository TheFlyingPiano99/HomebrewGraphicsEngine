#pragma once
class SceneEvent
{
public:
	virtual ~SceneEvent() = default;

	virtual void execute(float dt) = 0;

};


