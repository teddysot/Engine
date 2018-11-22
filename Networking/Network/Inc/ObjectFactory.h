#ifndef INCLUDED_NETWORK_OBJECTFACTORY_H
#define INCLUDED_NETWORK_OBJECTFACTORY_H

#include "Common.h"

#define CLASS_ID( ClassType, FourCC)\
	static void* CreateInstance() { return new ClassType(); }\
	static const uint32_t ClassId = FourCC;\
	virtual uint32_t GetClassId() const { return ClassId; }

namespace Network {

class ObjectFactory
{
public:

	template <class T>
	void Register()
	{
		mCreators[T::ClassId] = T::CreateInstance;
	}

	void* CreateInstance(uint32_t classId)
	{
		ASSERT(mCreators.find(classId) != mCreators.end(), "[ObjectFactory] Class id %d not registered.", classId);
		return mCreators[classId]();
	}

private:
	std::unordered_map<uint32_t, std::function<void*()>> mCreators;
};

}

#endif // INCLUDED_NETWORK_OBJECTFACTORY_H