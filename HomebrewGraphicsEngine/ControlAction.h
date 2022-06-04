#pragma once

#include "Scene.h"
#include <queue>
#include <GLFW/glfw3.h>

class ControlAction
{
	int key;
	bool enableRepeat;

public:
	ControlAction(int _key, bool _enableRepeat = true) : key(_key), enableRepeat(_enableRepeat) {
	}

	bool isThisAction(int key, int scancode, int action);

	virtual void execute(Scene* scene, float dt) = 0;
};

class MoveCameraForward : public ControlAction {
public:
	MoveCameraForward() : ControlAction(GLFW_KEY_W) {

	}

	void execute(Scene* scene, float dt) override;
};

class MoveCameraBackward : public ControlAction {
public:
	MoveCameraBackward() : ControlAction(GLFW_KEY_S) {
	}

	void execute(Scene* scene, float dt) override;
};

class ToggleGUI : public ControlAction {
public:
	ToggleGUI() : ControlAction(GLFW_KEY_O, false) {
	}

	void execute(Scene* scene, float dt) override;
};

class FastForward : public ControlAction {
public:
	FastForward() : ControlAction(GLFW_KEY_F) {
	}

	void execute(Scene* scene, float dt) override;
};

class Rewind : public ControlAction {
public:
	Rewind() : ControlAction(GLFW_KEY_R) {
	}

	void execute(Scene* scene, float dt) override;
};

class TogglePause : public ControlAction {
public:
	TogglePause() : ControlAction(GLFW_KEY_P, false) {
	}

	void execute(Scene* scene, float dt) override;
};

class ToggleFullScreenMode : public ControlAction {
public:
	ToggleFullScreenMode() : ControlAction(GLFW_KEY_TAB, true) {
	}

	void execute(Scene* scene, float dt) override;
};
