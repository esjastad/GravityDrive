// ©, 2021, Erik Jastad. All Rights Reserved


#include "GDRecastNavMesh.h"
// WIP Custom NavMesh
AGDRecastNavMesh::AGDRecastNavMesh() : ARecastNavMesh()
{
	RuntimeGeneration = ERuntimeGenerationType::Dynamic;
}