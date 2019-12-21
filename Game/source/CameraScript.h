#pragma once
#include "components/ScriptComponent.h"
#include "Input.h"

namespace Baryon
{
class CameraScript : public EntityScript<CameraScript>
{
public:
	CameraScript(Entity entity) : EntityScript(entity)
	{
		Input::bindAxis("MoveForward", this, &CameraScript::moveForward);
		Input::bindAxis("MoveRight", this, &CameraScript::moveRight);
		Input::bindAxis("Turn", this, &CameraScript::turn);
		Input::bindAxis("LookUp", this, &CameraScript::lookUp);
	}
	void moveForward(float inputValue)
	{
		entity.getComponent<Transform>()->translate( 0, 0, inputValue * 0.2f * speed);
	}
	void moveRight(float inputValue)
	{
		entity.getComponent<Transform>()->translate(inputValue * 0.2f * speed, 0, 0);
	}
	void turn(float inputValue)
	{
		entity.getComponent<Transform>()->rotate(0, inputValue * 0.005f, 0, SPACE::GLOBAL);
	}
	void lookUp(float f)
	{
		entity.getComponent<Transform>()->rotate(f * 0.005f, 0, 0);
	}
	void setSpeed(float value)
	{
		speed = 1 + 5 * value;
	}

	void update() override
	{
	}
private:
	float speed = 3;
};
}
