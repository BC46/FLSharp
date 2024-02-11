#pragma once

#include "Common.h"
#include "RemoteServer.h"

#define CHECK_FOR_SYNC_CALL_ADDR 0x541602
#define OBJ_UPDATE_CALL_ADDR 0x54167C

void InitTimeSinceLastUpdate();

bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3);
void __fastcall SPObjUpdate_Hook(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client);
