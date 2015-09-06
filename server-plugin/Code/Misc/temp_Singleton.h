#ifdef GNUC
#undef min
#undef max
#endif
#include "Preprocessors.h"

#ifndef SINGLETON_CLASS
#define SINGLETON_CLASS

template <typename SingletonT>
class Singleton
{
protected:
  Singleton () {};
  ~Singleton () {};

public:

	static SingletonT * GetInstance()
	{
		//m_mtx.lock();
		if (_singleton == nullptr)
		{
			_singleton = new SingletonT;
		}
		//m_mtx.unlock();
		return (static_cast<SingletonT*> (_singleton));
	};

	static void Delete()
	{
		if (_singleton != nullptr)
		{
			delete _singleton;
			_singleton = nullptr;
		}
	};

public:
	 //static std::mutex m_mtx;

private:
  static SingletonT *_singleton;
};

template <typename T>
T *Singleton<T>::_singleton = nullptr;

//template <typename T>
//std::mutex Singleton<T>::m_mtx;

#endif
