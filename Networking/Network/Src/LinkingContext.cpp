#include "Precompiled.h"

#include "LinkingContext.h"

using namespace Network;

LinkingContext::LinkingContext()
	: mNextNetworkId(0)
{

}

int LinkingContext::GetNetworkId(void* instance) const
{
	auto it = mInstanceToNetworkIdMap.find(instance);
	if (it != mInstanceToNetworkIdMap.end())
	{
		return it->second;
	}
	else
	{
		return sInvalidId;
	}
}

void * LinkingContext::GetInstance(int networkId) const
{
	auto it = mNetworkIdToInstanceMap.find(networkId);
	if (it != mNetworkIdToInstanceMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

int LinkingContext::Register(void * instance)
{
	ASSERT(GetNetworkId(instance) == sInvalidId, "[LinkingContext] Instance already registered!");
	mInstanceToNetworkIdMap[instance] = mNextNetworkId;
	mNetworkIdToInstanceMap[mNextNetworkId] = instance;

	return mNextNetworkId++;
}

void LinkingContext::Register(void * instance, int networkId)
{
	ASSERT(GetNetworkId(instance) == sInvalidId, "[LinkingContext] Instance already registered!");
	ASSERT(GetInstance(networkId) == nullptr, "[LinkingContext] Instance already registered!");
	mInstanceToNetworkIdMap[instance] = networkId;
	mNetworkIdToInstanceMap[networkId] = instance;
	mNextNetworkId = std::max(mNextNetworkId, networkId + 1);
}

void LinkingContext::Unregister(void * instance)
{
	auto it = mInstanceToNetworkIdMap.find(instance);
	if (it != mInstanceToNetworkIdMap.end())
	{
		int networkId = it->second;
		mInstanceToNetworkIdMap.erase(instance);
		mNetworkIdToInstanceMap.erase(networkId);
	}
}
