#pragma once
#ifndef  __CCH_SINGLETON_H__
#define __CCH_SINGLETON_H__
#include<memory>

namespace cch
{
	template<class T, class X=void, int N = 0>
	class Singleton
	{
	public:
		static T* GetInstance()
		{
			static T v;
			return &v;
		}
	};

	template<class T, class X = void, int N = 0>
	class SingletonPtr
	{
	public:
		static std::shared_ptr<T> GetInstance()
		{
			static std::shared_ptr<T> v(new T); 
			return v;
		}
	};
}

#endif // ! __CCH_SINGLETON_H__



