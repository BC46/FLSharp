#pragma once

#include "Common.h"
#include "RemoteServer.h"

void SetTimeSinceLastUpdate();

bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3);
void __fastcall SPObjUpdate_Hook(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client);
