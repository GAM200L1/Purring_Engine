#pragma once

namespace PE
{
	template <typename T>
	class Singleton
	{
	public: 
		static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		// Disable copy and assignment
		Singleton(const Singleton&) = delete;
		Singleton & operator= (const Singleton&) = delete;

	protected:
		Singleton() = default;
		~Singleton() = default;
	};
}