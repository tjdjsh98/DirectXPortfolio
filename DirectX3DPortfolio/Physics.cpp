#include "pch.h"
#include "Physics.h"
#include "Utils.h"

vector<Hit> Physics::Raycast(Vector3 origin, Vector3 direction, float distance)
{

	const PxU32 bufferSize = 256;       
	PxRaycastHit hitBuffer[bufferSize]; 
	PxRaycastBuffer buf(hitBuffer, bufferSize);

	direction.Normalize();
	bool isHit = PHYSICS->GetScene()->raycast(
		Utils::ToPxVec3(origin),
		Utils::ToPxVec3(direction),
		distance,
		buf
	);
	
	vector<Hit> result(buf.nbTouches);
	for (int t = 0;t < buf.nbTouches; t++)
	{

		result[t].collider = PHYSICS->GetGameObjectMatchingRigidBody(buf.touches[t].actor);
		result[t].distance = buf.touches[t].distance;
		result[t].point = Utils::ToVector3(buf.touches[t].position);
		result[t].normal = Utils::ToVector3(buf.touches[t].normal);
	}


	return result;
}
