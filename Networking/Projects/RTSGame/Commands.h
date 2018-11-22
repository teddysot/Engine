#ifndef INCLUDED_COMMANDS_H
#define INCLUDED_COMMANDS_H

#include <Network/Inc/Network.h>
#include <XEngine.h>

enum class Command
{
	Spawn,
	Move,
	Attack
};

struct ICommand
{
	virtual void Serialize(Network::StreamWriter& writer) = 0;
	virtual void Deserialize(Network::StreamReader& reader) = 0;
	virtual Command GetType() const = 0;
};

typedef std::vector<ICommand*> CommandList;

class SpawnCommand : public ICommand
{
public:
	void Serialize(Network::StreamWriter& writer) override;
	void Deserialize(Network::StreamReader& reader) override;

	Command GetType() const override { return Command::Spawn; }

	X::Math::Vector3 position;
	X::Math::Vector3 direction;
};

class MoveCommand : public ICommand
{
public:
	void Serialize(Network::StreamWriter& writer) override;
	void Deserialize(Network::StreamReader& reader) override;

	Command GetType() const override { return Command::Move; }

	std::vector<int> networkIds;
	X::Math::Vector3 position;
};

class AttackCommand : public ICommand
{
public:
	void Serialize(Network::StreamWriter& writer) override;
	void Deserialize(Network::StreamReader& reader) override;

	Command GetType() const override { return Command::Attack; }

	std::vector<int> networkIds;
	int targetId;
};

#endif // #ifndef INCLUDED_COMMANDS_H