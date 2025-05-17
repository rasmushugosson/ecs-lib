#include "general/pch.h"

#include "Entity.h"

static ae::Entity s_NextEntityIndex = 1;

ae::Entity ae::NewEntity()
{
	ae::Entity nextEntity = s_NextEntityIndex;

	s_NextEntityIndex++;

	return nextEntity;
};
