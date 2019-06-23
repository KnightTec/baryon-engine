#pragma once
#include <vector>

/*
 * This file contains the base classes for an implementation of the observer pattern
 */

template <typename ...T>
class Observer
{
public:
	virtual ~Observer() = default;
	virtual void onNotify(T*... args) = 0;
};

template <typename ...T>
class Subject
{
	std::vector<int> aM;
public:
	void addObserver(Observer<T...>* observer)
	{
		observers.push_back(observer);
	}
protected:
	void notify(T... args)
	{
		int numObservers =  observers.size();
		for (int i = 0; i < numObservers; i++)
		{
			observers[i]->onNotify(args...);
		}
	}
private:
	std::vector<Observer<T...>*> observers;
};
