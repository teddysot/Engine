#include "Commands.h"

void SpawnCommand::Serialize(Network::StreamWriter& writer)
{
	writer.Write(position.x);
	writer.Write(position.y);
	writer.Write(position.z);
	writer.Write(direction.x);
	writer.Write(direction.y);
	writer.Write(direction.z);
}

void SpawnCommand::Deserialize(Network::StreamReader& reader)
{
	reader.Read(position.x);
	reader.Read(position.y);
	reader.Read(position.z);
	reader.Read(direction.x);
	reader.Read(direction.y);
	reader.Read(direction.z);
}

void MoveCommand::Serialize(Network::StreamWriter& writer)
{
	writer.Write(networkIds.size());
	for (auto id : networkIds)
	{
		writer.Write(id);
	}

	writer.Write(position.x);
	writer.Write(position.y);
	writer.Write(position.z);
}

void MoveCommand::Deserialize(Network::StreamReader& reader)
{
	size_t count = 0;
	reader.Read(count);

	networkIds.resize(count);
	for (auto& id : networkIds)
	{
		reader.Read(id);
	}

	reader.Read(position.x);
	reader.Read(position.y);
	reader.Read(position.z);
}

void AttackCommand::Serialize(Network::StreamWriter& writer)
{
	writer.Write(networkIds.size());
	for (auto id : networkIds)
	{
		writer.Write(id);
	}

	writer.Write(targetId);
}

void AttackCommand::Deserialize(Network::StreamReader& reader)
{
	size_t count = 0;
	reader.Read(count);

	networkIds.resize(count);
	for (auto& id : networkIds)
	{
		reader.Read(id);
	}

	reader.Read(targetId);
}