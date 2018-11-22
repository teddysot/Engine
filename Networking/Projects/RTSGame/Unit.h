#ifndef INCLUDED_UNIT_H
#define INCLUDED_UNIT_H

#include "RTS.h"
#include <Network\Inc\Network.h>

class Unit
{
public:
	Unit();

	void Update(float deltaTime);
	void Draw(bool master); 
	
	void Serialize(Network::StreamWriter& writer);
	void Deserialize(Network::StreamReader& reader);

	bool HasDestination() const;

	void SetPosition(const X::Math::Vector3& position) { mPosition = position; mDestination = position; }
	void SetDirection(const X::Math::Vector3& direction) { mDirection = direction; }
	void SetDestination(const X::Math::Vector3& destination) { mDestination = destination; }
	void SetSelected(bool selected) { mSelected = selected; }

	const X::Math::Vector3& GetPosition() const { return mPosition; }
	const X::Math::Vector3& GetDirection() const { return mDirection; }
	bool IsSelected() const { return mSelected; }

private:
	X::Math::Vector3 mPosition;
	X::Math::Vector3 mDirection;
	X::Math::Vector3 mDestination;
	bool mSelected;
};

#endif // #ifndef INCLUDED_UNIT_H