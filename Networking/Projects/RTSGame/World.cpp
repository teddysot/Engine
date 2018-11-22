#include "World.h"

namespace
{
	World* singleton = nullptr;
}

void World::Initialize()
{
	XASSERT(singleton == nullptr, "[World] Singleton already exists!");
	singleton = new World();
}

void World::Terminate()
{
	X::SafeDelete(singleton);
}

World* World::Get()
{
	XASSERT(singleton != nullptr, "[World] Singleton not initialized!");
	return singleton;
}

World::World()
{
}

World::~World()
{
	for (auto unit : mUnits)
	{
		X::SafeDelete(unit);
	}
	mUnits.clear();
}

void World::Update(float deltaTime)
{
	SelectUnits();
	CommandUnits();

	for (auto unit : mUnits)
	{
		unit->Update(deltaTime);
	}
}

void World::Draw(bool master)
{
	for (auto unit : mUnits)
	{
		unit->Draw(master);
	}
}

void World::SpawnUnitRandom()
{
	Unit* unit = new Unit();

	X::Math::Vector3 min(-25.0f, 0.0f, -25.0f);
	X::Math::Vector3 max(+25.0f, 0.0f, +25.0f);
	X::Math::Vector3 position = X::RandomVector3(min, max);
	unit->SetPosition(position);

	float x = X::RandomFloat(-1.0f, 1.0f);
	float z = X::RandomFloat(-1.0f, 1.0f);
	unit->SetDirection(X::Math::Normalize(X::Math::Vector3(x, 0.0f, z)));

	mUnits.push_back(unit);
}

void World::WriteSnapshot(Network::StreamWriter& writer)
{
	for (auto unit : mUnits)
	{
		// Write network id
		writer.Write(GetNetworkId(unit));

		// Write unit data
		unit->Serialize(writer);
	}
}

void World::ApplySnapshot(Network::StreamReader& reader)
{
	std::vector<Unit*> replicatedUnits;

	while (reader.GetRemainingDataSize() > 0)
	{
		int networkId = Network::LinkingContext::sInvalidId;
		reader.Read(networkId);

		Unit* unit = (Unit*)mLinkingContext.GetInstance(networkId);
		if (unit == nullptr)
		{
			unit = new Unit();
			mLinkingContext.Register(unit, networkId);
		}

		unit->Deserialize(reader);
		replicatedUnits.push_back(unit);
	}

	for (auto unit : mUnits)
	{
		auto it = std::find(replicatedUnits.begin(), replicatedUnits.end(), unit);
		if (it == replicatedUnits.end())
		{
			mLinkingContext.Unregister(unit);
			X::SafeDelete(unit);
		}
	}

	mUnits = replicatedUnits;
}

void World::ProcessCommands(const CommandList& commandList)
{
	for (auto command : commandList)
	{
		switch (command->GetType())
		{
		case Command::Spawn:
		{
			SpawnCommand* spawnCommand = static_cast<SpawnCommand*>(command);

			Unit* unit = new Unit();
			mLinkingContext.Register(unit);

			unit->SetPosition(spawnCommand->position);
			unit->SetDirection(spawnCommand->direction);
			mUnits.push_back(unit);

			break;
		}
		case Command::Move:
		{
			MoveCommand* moveCommand = static_cast<MoveCommand*>(command);

			int counter = 0;
			for (auto id : moveCommand->networkIds)
			{
				Unit* unit = (Unit*)mLinkingContext.GetInstance(id);
				if (unit != nullptr)
				{
					unit->SetDestination(RTS::GetSpiralPosition(moveCommand->position, counter++));
				}
			}
			break;
		}
		case Command::Attack:
		{
			// Unexpected, ignore for now
			break;
		}
		}
	}
}

CommandList World::GetCommands()
{
	return std::move(mCommandList);
}

int World::GetNetworkId(Unit* unit)
{
	int networkId = mLinkingContext.GetNetworkId(unit);
	if (networkId == Network::LinkingContext::sInvalidId)
	{
		networkId = mLinkingContext.Register(unit);
	}
	return networkId;
}

void World::SelectUnits()
{
	float minX, maxX, minZ, maxZ;
	if (RTS::GetSelectionBox(minX, maxX, minZ, maxZ))
	{
		for (auto unit : mUnits)
		{
			bool selected =
				unit->GetPosition().x > minX &&
				unit->GetPosition().x < maxX &&
				unit->GetPosition().z > minZ &&
				unit->GetPosition().z < maxZ;
			unit->SetSelected(selected);
		}
	}
}

void World::CommandUnits()
{
	if (X::IsKeyPressed(X::Keys::SPACE))
	{
		SpawnCommand* spawnCommand = new SpawnCommand();

		X::Math::Vector3 min(-25.0f, 0.0f, -25.0f);
		X::Math::Vector3 max(+25.0f, 0.0f, +25.0f);
		spawnCommand->position = X::RandomVector3(min, max);

		float x = X::RandomFloat(-1.0f, 1.0f);
		float z = X::RandomFloat(-1.0f, 1.0f);
		spawnCommand->direction = X::Math::Normalize(X::Math::Vector3(x, 0.0f, z));

		mCommandList.push_back(spawnCommand);
	}
	if (X::IsMousePressed(X::Mouse::RBUTTON))
	{
		X::Math::Ray ray = X::GetMouseRay();
		X::Math::Vector3 groundPosition = RTS::GetGroundPosition(ray);

		MoveCommand* moveCommand = new MoveCommand();
		moveCommand->position = groundPosition;

		for (auto unit : mUnits)
		{
			if (unit->IsSelected())
			{
				moveCommand->networkIds.push_back(GetNetworkId(unit));
			}
		}

		mCommandList.push_back(moveCommand);
	}
}