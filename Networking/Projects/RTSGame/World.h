#ifndef INCLUDED_WORLD_H
#define INCLUDED_WORLD_H

#include "Commands.h"
#include "RTS.h"
#include "Unit.h"
#include <Network/Inc/Network.h>

class World
{
public:
	static void Initialize();
	static void Terminate();
	static World* Get();

public:
	World();
	~World();

	void Update(float deltaTime);
	void Draw(bool master);

	void SpawnUnitRandom();

	void WriteSnapshot(Network::StreamWriter& writer);
	void ApplySnapshot(Network::StreamReader& reader);

	void ProcessCommands(const CommandList& commandList);
	CommandList GetCommands();

private:
	int GetNetworkId(Unit* unit);
	void SelectUnits();
	void CommandUnits();

	Network::LinkingContext mLinkingContext;
	CommandList mCommandList;

	std::vector<Unit*> mUnits;
};

#endif // #ifndef INCLUDED_WORLD_H