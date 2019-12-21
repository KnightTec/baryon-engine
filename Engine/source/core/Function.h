#pragma once

namespace Baryon
{
template <typename... Args>
class Function
{
private:
	class ICallback
	{
	public:
		virtual ~ICallback() = default;
		virtual ICallback* clone() const = 0;
		virtual void call(Args ...) = 0;
	};

	class GlobalCallback : public ICallback
	{
	public:
		GlobalCallback(void (*callback)(Args ...)) : callback(callback)
		{
		}
		ICallback* clone() const override { return new GlobalCallback(callback); }
		void call(Args ... args) override { callback(args...); }
	private:
		void (*callback)(Args ...);
	};

	template <typename T>
	class ClassCallback : public ICallback
	{
	public:
		ClassCallback(T* object, void (T::*callback)(Args...)) : object(object), callback(callback)
		{
		}
		ICallback* clone() const override { return new ClassCallback<T>(object, callback); }
		void call(Args ... args) override { (object->*callback)(args...); }
	private:
		T* object;
		void (T::*callback)(Args ...);
	};

	ICallback* callback;
public:
	Function() : callback(nullptr)
	{
	}
	~Function() { delete callback; }
	Function(const Function& other)
	{
		if (other.callback)
		{
			callback = other.callback->clone();
		}
	}
	Function& operator=(const Function& other)
	{
		delete callback;
		if (other.callback)
		{
			callback = other.callback->clone();
		}
		else
		{
			callback = nullptr;
		}
		return *this;
	}
	Function(void (*callback)(Args ...))
	{
		this->callback = new GlobalCallback(callback);
	}
	template <typename T>
	Function(T* object, void (T::*callback)(Args ...))
	{
		this->callback = new ClassCallback<T>(object, callback);
	}
	void operator()(Args ... args) { callback->call(args...); }
};
}
