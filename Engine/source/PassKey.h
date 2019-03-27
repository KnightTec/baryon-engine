#pragma once

template <typename T>
class Key
{
	friend T;
private:
	Key()
	{
	}
	Key(const Key&)
	{
	}
};