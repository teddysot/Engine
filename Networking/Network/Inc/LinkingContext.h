#ifndef INCLUDED_NETWORK_LINKINGCONTEXT_H
#define INCLUDED_NETWORK_LINKINGCONTEXT_H

namespace Network {

class LinkingContext
{
public:
	static const int sInvalidId = -1;

public:
	LinkingContext();

	int GetNetworkId(void* instance) const;
	void* GetInstance(int networkId) const;

	// Register an instance to the next network id
	int Register(void* instance);

	// Register an instance to a specific network id
	void Register(void* instance, int networkId);

	// Unregister an instance
	void Unregister(void* instance);

private:
	std::unordered_map<int, void*> mNetworkIdToInstanceMap;
	std::unordered_map<void*, int> mInstanceToNetworkIdMap;
	int mNextNetworkId;
};

} // namespace Network

#endif // INCLUDED_NETWORK_LINKINGCONTEXT_H