#pragma once

namespace Baryon
{
class ISystem
{
public:
	virtual ~ISystem() = default;
	virtual void initialize() = 0;
	virtual void tick() = 0;
	virtual void terminate() = 0;
};

template <typename... HandledComponents>
class System : ISystem
{
public:
	virtual void tick() override;
private:
	virtual void update(HandledComponents ...) = 0;
};

template <typename ... HandledComponents>
void System<HandledComponents...>::tick()
{
	//TODO: multiple threads
	//update()
}

}
