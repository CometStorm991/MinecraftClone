#pragma once

#include <mutex>

template <typename T>
struct LockedElement
{
	T value;
	mutable std::mutex mutex;
};