/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * Design Pattern Singleton.
 */

template<typename T>
class Singleton 
{
public:
	Singleton() { }
	virtual ~Singleton() { }

	static T* getInstance() 
	{
		return instance;
	}
	
	static T* newInstance() 
	{
		if(instance)
			delete instance;
		
		instance = new T();
		return instance ;
	}
	
	static void deleteInstance() 
	{
		if(instance)
			delete instance;
		instance = NULL;
	}
protected:
	static T* instance;
private:
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

};

template<class T> T* Singleton<T>::instance = 0;

#endif // SINGLETON_H

