#include "test.h"
#include "alchemy_crash.h"
#include "Common.h"
#include "copy_paste.h"
#include "flash_particles.h"
#include "Freelancer.h"
#include "pilot_names.h"
#include "RemoteServer.h"
#include "rep_requirements.h"
#include "resolutions.h"
#include "test_sounds.h"
#include "trade_lane_lights.h"
#include "ui_anim.h"
#include "weapon_anim.h"
#include "dealer_fixes.h"
#include "cheat_detection.h"
#include "cursor_colors.h"
#include "waypoint_names.h"
#include "mouse.h"
#include "fl_math.h"
#include "light_limit.h"
#include "exit.h"
#include "sub_target.h"

// alchemy_crash.h
TEST_CASE("value offsets are correct", "[alchemy]")
{
    REQUIRE(offsetof(Alchemy, progress) == 0x0);
    REQUIRE(offsetof(Alchemy, effect) == 0x4);
}

// Common.h
TEST_CASE("size is correct", "[ini_reader]")
{
    REQUIRE(sizeof(INI_Reader) == 0x1565);
}

TEST_CASE("vftable offsets are correct", "[cequip]")
{
    REQUIRE(GetVftableOffset<CEquip>(&CEquip::Activate) == 0x30);
}

TEST_CASE("size is correct", "[cequiptraverser]")
{
    REQUIRE(sizeof(CEquipTraverser) == 0x10);
}

TEST_CASE("vftable offsets are correct", "[celightequip]")
{
    REQUIRE(GetVftableOffset<CELightEquip>(&CELightEquip::Activate) == 0x30);
}

TEST_CASE("value offsets are correct", "[archetype::root]")
{
    REQUIRE(offsetof(Archetype::Root, scriptIndex) == 0x44);
}

TEST_CASE("value offsets are correct", "[archetype::ship]")
{
    REQUIRE(offsetof(Archetype::Ship, idsName) == 0x14);
    REQUIRE(offsetof(Archetype::Ship, idsInfo) == 0x18);
    REQUIRE(offsetof(Archetype::Ship, angularDrag) == 0x108);
    REQUIRE(offsetof(Archetype::Ship, steeringTorque) == 0x114);
}

TEST_CASE("value offsets are correct", "[archetype::eqobj]")
{
    REQUIRE(offsetof(Archetype::EqObj, idsName) == 0x14);
    REQUIRE(offsetof(Archetype::EqObj, idsInfo) == 0x18);
}

TEST_CASE("value offsets are correct", "[archetype::solar]")
{
    REQUIRE(offsetof(Archetype::Solar, idsName) == 0x14);
    REQUIRE(offsetof(Archetype::Solar, idsInfo) == 0x18);
}

TEST_CASE("value offsets are correct", "[archetype::shieldgenerator]")
{
    REQUIRE(offsetof(Archetype::ShieldGenerator, maxCapacity) == 0x94);
    REQUIRE(offsetof(Archetype::ShieldGenerator, offlineThreshold) == 0xA0);
}

TEST_CASE("value offsets are correct", "[engineobject]")
{
    REQUIRE(offsetof(EngineObject, engineInstance) == 0x04);
}

TEST_CASE("value offsets are correct", "[cobject]")
{
    REQUIRE(offsetof(CObject, engineInstance) == 0x04);
    REQUIRE(offsetof(CObject, classType) == 0x4C);
}

TEST_CASE("value offsets are correct", "[csimple]")
{
    REQUIRE(offsetof(CSimple, engineInstance) == 0x04);
    REQUIRE(offsetof(CSimple, classType) == 0x4C);
    REQUIRE(offsetof(CSimple, nickname) == 0xB0);
}

TEST_CASE("value offsets are correct", "[cattachedequip]")
{
    REQUIRE(offsetof(CAttachedEquip, parent) == 0x04);
}

TEST_CASE("vftable offsets are correct", "[cattachedequip]")
{
    REQUIRE(GetVftableOffset<CAttachedEquip>(&CAttachedEquip::GetRootIndex) == 0x84);
}

TEST_CASE("value offsets are correct", "[ceqobj]")
{
    REQUIRE(offsetof(CEqObj, engineInstance) == 0x04);
    REQUIRE(offsetof(CEqObj, classType) == 0x4C);
    REQUIRE(offsetof(CEqObj, nickname) == 0xB0);
    REQUIRE(offsetof(CEqObj, equipManager) == 0xE4);
    REQUIRE(offsetof(CEqObj, baseId) == 0x160);
    REQUIRE(offsetof(CEqObj, behaviorInterface) == 0x194);
}

TEST_CASE("vftable offsets are correct", "[ceqobj]")
{
    REQUIRE(GetVftableOffset<CEqObj>(&CEqObj::get_name) == 0x88);
}

TEST_CASE("value offsets are correct", "[cship]")
{
    REQUIRE(offsetof(CShip, engineInstance) == 0x04);
    REQUIRE(offsetof(CShip, classType) == 0x4C);
    REQUIRE(offsetof(CShip, nickname) == 0xB0);
    REQUIRE(offsetof(CShip, equipManager) == 0xE4);
    REQUIRE(offsetof(CShip, baseId) == 0x160);
    REQUIRE(offsetof(CShip, behaviorInterface) == 0x194);
    REQUIRE(offsetof(CShip, groupId) == 0x1B4);
}

TEST_CASE("vftable offsets are correct", "[cship]")
{
    REQUIRE(GetVftableOffset<CShip>(&CShip::get_name) == 0x88);
}

TEST_CASE("value offsets are correct", "[csolar]")
{
    REQUIRE(offsetof(CSolar, engineInstance) == 0x04);
    REQUIRE(offsetof(CSolar, classType) == 0x4C);
    REQUIRE(offsetof(CSolar, nickname) == 0xB0);
    REQUIRE(offsetof(CSolar, equipManager) == 0xE4);
    REQUIRE(offsetof(CSolar, baseId) == 0x160);
    REQUIRE(offsetof(CSolar, behaviorInterface) == 0x194);
}

TEST_CASE("vftable offsets are correct", "[csolar]")
{
    REQUIRE(GetVftableOffset<CSolar>(&CSolar::get_name) == 0x88);
}

TEST_CASE("vftable offsets are correct", "[fuseaction]")
{
    REQUIRE(GetVftableOffset<FuseAction>(&FuseAction::IsTriggered) == 0x4);
}

TEST_CASE("vftable offsets are correct", "[ceengine]")
{
    REQUIRE(GetVftableOffset<FuseAction>(&CEEngine::IsTriggered) == 0x4);
}

TEST_CASE("value offsets are correct", "[iobjrw]")
{
    REQUIRE(offsetof(IObjRW, cobject) == 0x10);
    REQUIRE(offsetof(IObjRW, unk_x1C) == 0x1C);
    REQUIRE(offsetof(IObjRW, flags) == 0x18C);
}

TEST_CASE("vftable offsets are correct", "[iobjrw]")
{
    REQUIRE(GetVftableOffset<IObjRW>(&IObjRW::get_simple_id) == 0x20);
    REQUIRE(GetVftableOffset<IObjRW>(&IObjRW::get_attitude_towards) == 0x68);
    REQUIRE(GetVftableOffset<IObjRW>(&IObjRW::get_target) == 0x74);
    REQUIRE(GetVftableOffset<IObjRW>(&IObjRW::is_player) == 0xBC);
}

TEST_CASE("value offsets are correct", "[iobjinspect]")
{
    REQUIRE(offsetof(IObjInspect, cobject) == 0x10);
    REQUIRE(offsetof(IObjInspect, unk_x1C) == 0x1C);
    REQUIRE(offsetof(IObjInspect, flags) == 0x18C);
}

TEST_CASE("vftable offsets are correct", "[iobjinspect]")
{
    REQUIRE(GetVftableOffset<IObjInspect>(&IObjInspect::get_simple_id) == 0x20);
    REQUIRE(GetVftableOffset<IObjInspect>(&IObjInspect::get_attitude_towards) == 0x68);
    REQUIRE(GetVftableOffset<IObjInspect>(&IObjInspect::get_target) == 0x74);
    REQUIRE(GetVftableOffset<IObjInspect>(&IObjInspect::is_player) == 0xBC);
}

TEST_CASE("value offsets are correct", "[physicsinfo]")
{
    REQUIRE(offsetof(PhysicsInfo, autoLevel) == 0x2F);
}

TEST_CASE("value offsets are correct", "[ibehaviormanager]")
{
    REQUIRE(offsetof(IBehaviorManager, physicsInfo) == 0x08);
    REQUIRE(offsetof(IBehaviorManager, currentManeuver) == 0xC0);
    REQUIRE(offsetof(IBehaviorManager, rotationLock) == 0xF9);
}

TEST_CASE("value offsets are correct", "[id_string]")
{
    REQUIRE(offsetof(ID_String, ids) == 0x0);
}

TEST_CASE("value offsets are correct", "[equipdesc]")
{
    REQUIRE(offsetof(EquipDesc, archId) == 0x4);
}

TEST_CASE("value offsets are correct", "[equipdesclist]")
{
    REQUIRE(offsetof(EquipDescList, list) == 0x0);
}

TEST_CASE("value offsets are correct", "[goodinfo]")
{
    REQUIRE(offsetof(GoodInfo, type) == 0x4C);
    REQUIRE(offsetof(GoodInfo, shipId) == 0x54);
    REQUIRE(offsetof(GoodInfo, shipHullId) == 0x90);
    REQUIRE(offsetof(GoodInfo, equipDescLists) == 0x94);
}

// copy_paste.h
TEST_CASE("value offsets are correct", "[inputchar]")
{
    REQUIRE(offsetof(InputChar, c) == 0x0);
    REQUIRE(offsetof(InputChar, flags) == 0x4);
    REQUIRE(offsetof(InputChar, unk) == 0x8);
}

TEST_CASE("value offsets are correct", "[keymapinfo]")
{
    REQUIRE(offsetof(KeyMapInfo, controlCharacterFlags) == 0x8);
    REQUIRE(offsetof(KeyMapInfo, enteredKey) == 0x10);
}

TEST_CASE("value offsets are correct", "[inputboxwindow]")
{
    REQUIRE(offsetof(InputBoxWindow, pos) == 0x49C);
    REQUIRE(offsetof(InputBoxWindow, chars) == 0x4C4);
    REQUIRE(offsetof(InputBoxWindow, maxCharsLength) == 0x510);
    REQUIRE(offsetof(InputBoxWindow, forbiddenChar) == 0x528);
    REQUIRE(offsetof(InputBoxWindow, noForbiddenChar) == 0x52A);
    REQUIRE(offsetof(InputBoxWindow, ime) == 0x52C);
}

TEST_CASE("vftable offsets are correct", "[inputboxwindow]")
{
    REQUIRE(GetVftableOffset<InputBoxWindow>(&InputBoxWindow::WriteTypedKey) == 0xC8);
}

// flash_particles.h
TEST_CASE("vftable offsets are correct", "[effectinstance]")
{
    REQUIRE(GetVftableOffset<EffectInstance>(&EffectInstance::FreeEngineEffect) == 0x4);
}

TEST_CASE("value offsets are correct", "[clilauncher]")
{
    REQUIRE(offsetof(CliLauncher, vftable) == 0x0);
    REQUIRE(offsetof(CliLauncher, launcher) == 0x04);
    REQUIRE(offsetof(CliLauncher, parent) == 0x08);
    REQUIRE(offsetof(CliLauncher, currentFlashParticle) == 0x28);
    REQUIRE(offsetof(CliLauncher, flashParticlesArr) == 0x28);
}

// Freelancer.h
TEST_CASE("value offsets are correct", "[waypoint]")
{
    REQUIRE(offsetof(Waypoint, pos) == 0x0);
    REQUIRE(offsetof(Waypoint, system) == 0xC);
    REQUIRE(offsetof(Waypoint, target) == 0x10);
    REQUIRE(offsetof(Waypoint, waypointNumber) == 0x14);
}

TEST_CASE("value offsets are correct", "[navmapobj]")
{
    REQUIRE(offsetof(NavMapObj, type) == 0x0);
}

TEST_CASE("value offsets are correct", "[audiooption]")
{
    REQUIRE(offsetof(AudioOption, idsName) == 0x0);
    REQUIRE(offsetof(AudioOption, idsTooltip) == 0x4);
    REQUIRE(offsetof(AudioOption, defaultVolume) == 0x8);
}

TEST_CASE("size is correct", "[audiooption]")
{
    REQUIRE(sizeof(AudioOption) == 0x18);
}

TEST_CASE("value offsets are correct", "[nn_preferences]")
{
    REQUIRE(offsetof(NN_Preferences, audioOptions) == 0x528);
    REQUIRE(offsetof(NN_Preferences, scrollElements) == 0x654);
    REQUIRE(offsetof(NN_Preferences, supportedResAmount) == 0x950);
    REQUIRE(offsetof(NN_Preferences, unk_x97C) == 0x97C);
    REQUIRE(offsetof(NN_Preferences, selectedHeight) == 0x980);
    REQUIRE(offsetof(NN_Preferences, activeHeight) == 0x984);
    REQUIRE(offsetof(NN_Preferences, resSupportedArr) == 0x988);
    REQUIRE(offsetof(NN_Preferences, newData) == NN_PREFERENCES_NEW_DATA);
}

TEST_CASE("value offsets are correct", "[nn_shiptrader]")
{
    REQUIRE(offsetof(NN_ShipTrader, shipCount) == 0x3CC);
    REQUIRE(offsetof(NN_ShipTrader, selectedShipIndex) == 0x3D0);
    REQUIRE(offsetof(NN_ShipTrader, playerReputationWithBaseOwners) == 0x3F8);
    REQUIRE(offsetof(NN_ShipTrader, shipStatuses) == 0x3FC);
    REQUIRE(offsetof(NN_ShipTrader, shipRepPercentages) == 0x448);
}

TEST_CASE("value offsets are correct", "[flcursor]")
{
    REQUIRE(offsetof(FLCursor, xPos) == 0x0);
    REQUIRE(offsetof(FLCursor, yPos) == 0x4);
    REQUIRE(offsetof(FLCursor, distFromZero) == 0x8);
}

TEST_CASE("value offsets are correct", "[transform]")
{
    REQUIRE(offsetof(Transform, rot) == 0x0);
    REQUIRE(offsetof(Transform, pos) == 0x24);
}

TEST_CASE("value offsets are correct", "[camera]")
{
    REQUIRE(offsetof(Camera, pos) == 0x28);
    REQUIRE(offsetof(Camera, watchable) == 0xC0);
    REQUIRE(offsetof(Camera, shipOffset) == 0x14C);
    REQUIRE(offsetof(Camera, angularAcceleration) == 0x1D4);
    REQUIRE(offsetof(Camera, angularSlerpMultiplier) == 0x1D8);
    REQUIRE(offsetof(Camera, horizontalTurnAngle) == 0x1DC);
    REQUIRE(offsetof(Camera, verticalTurnUpAngle) == 0x1E0);
    REQUIRE(offsetof(Camera, verticalTurnDownAngle) == 0x1E4);
    REQUIRE(offsetof(Camera, turnLookAheadSlerpAmount) == 0x1E8);
    REQUIRE(offsetof(Camera, x1EC) == 0x1EC);
    REQUIRE(offsetof(Camera, distFromObj) == 0x1F0);
    REQUIRE(offsetof(Camera, x1F4) == 0x1F4);
    REQUIRE(offsetof(Camera, x1F8) == 0x1F8);
    REQUIRE(offsetof(Camera, x1FC) == 0x1FC);
}

TEST_CASE("vftable offsets are correct", "[camera]")
{
    REQUIRE(GetVftableOffset<Camera>(&Camera::MainUpdate) == 0x6C);
}

TEST_CASE("value offsets are correct", "[fluielement]")
{
    REQUIRE(offsetof(FlUiElement, flags) == 0x6C);
}

TEST_CASE("vftable offsets are correct", "[fluielement]")
{
    REQUIRE(GetVftableOffset<FlUiElement>(&FlUiElement::Render) == 0x34);
    REQUIRE(GetVftableOffset<FlUiElement>(&FlUiElement::Transform) == 0xA8);
}

// pilot_names.h
TEST_CASE("value offsets are correct", "[strbuffer]")
{
    REQUIRE(offsetof(StrBuffer, str) == 0x0);
    REQUIRE(offsetof(StrBuffer, capacity) == 0x4);
}

// RemoteServer.h
TEST_CASE("value offsets are correct", "[sspobjupdateinfo]")
{
    REQUIRE(offsetof(SSPObjUpdateInfo, throttle) == 0x28);
}

TEST_CASE("vftable offsets are correct", "[iserverimpl]")
{
    REQUIRE(GetVftableOffset<IServerImpl>(&IServerImpl::SPObjUpdate) == 0xD0);
}

// rep_requirements.h
TEST_CASE("value offsets are correct", "[dealerstack]")
{
    REQUIRE(offsetof(DealerStack, repRequired) == 0x24);
}

// resolutions.h
TEST_CASE("value offsets are correct", "[resolutioninitinfo]")
{
    REQUIRE(offsetof(ResolutionInitInfo, resolutionInfo) == 0x8);
}

// test_sounds.h
TEST_CASE("value offsets are correct", "[soundhandle]")
{
    REQUIRE(offsetof(SoundHandle, unkBytePtr) == 0x30);
}

TEST_CASE("vftable offsets are correct", "[soundhandle]")
{
    REQUIRE(GetVftableOffset<SoundHandle>(&SoundHandle::FreeReference, true) == 0x08);
    REQUIRE(GetVftableOffset<SoundHandle>(&SoundHandle::Pause) == 0x68);
    REQUIRE(GetVftableOffset<SoundHandle>(&SoundHandle::Resume) == 0x6C);
    REQUIRE(GetVftableOffset<SoundHandle>(&SoundHandle::IsPaused) == 0x70);
}

TEST_CASE("value offsets are correct", "[flsound]")
{
    REQUIRE(offsetof(FlSound, vftable) == 0x0);
    REQUIRE(offsetof(FlSound, id) == 0x04);
    REQUIRE(offsetof(FlSound, filePath) == 0x08);
    REQUIRE(offsetof(FlSound, unk_x0C) == 0x0C);
    REQUIRE(offsetof(FlSound, unk_x10) == 0x10);
    REQUIRE(offsetof(FlSound, unk_x14) == 0x14);
}

// trade_lane_lights.h
TEST_CASE("value offsets are correct", "[cetradelaneequip]")
{
    REQUIRE(offsetof(CETradeLaneEquip, vftable) == 0x0);
    REQUIRE(offsetof(CETradeLaneEquip, solar) == 0x4);
}

TEST_CASE("value offsets are correct", "[tradelaneequipobj]")
{
    REQUIRE(offsetof(TradeLaneEquipObj, vftable) == 0x0);
    REQUIRE(offsetof(TradeLaneEquipObj, tradeLaneEquip) == 0x4);
    REQUIRE(offsetof(TradeLaneEquipObj, isDisrupted) == 0x30);
}

// ui_anim.h
TEST_CASE("vftable offsets are correct", "[bigimage]")
{
    REQUIRE(GetVftableOffset<BigImage>(&BigImage::Destroy, true) == 0x08);
}

TEST_CASE("value offsets are correct", "[uitextmsgbutton]")
{
    REQUIRE(offsetof(UITextMsgButton, textImage) == 0x3EC);
    REQUIRE(offsetof(UITextMsgButton, disableHovering) == 0x471);
}

TEST_CASE("vftable offsets are correct", "[uitextmsgbutton]")
{
    REQUIRE(GetVftableOffset<UITextMsgButton>(&UITextMsgButton::Render) == 0x34);
    REQUIRE(GetVftableOffset<UITextMsgButton>(&UITextMsgButton::Transform) == 0xA8);
}

// weapon_anim.h
TEST_CASE("value offsets are correct", "[engmodel]")
{
    REQUIRE(offsetof(EngModel, type) == 0x00);
    REQUIRE(offsetof(EngModel, parent) == 0x10);
}

TEST_CASE("vftable offsets are correct", "[ianimation2]")
{
    REQUIRE(GetVftableOffset<IAnimation2>(&IAnimation2::Open, true) == 0x24);
}

// dealer_fixes.h
TEST_CASE("vftable offsets are correct", "[objectdeformable]")
{
    REQUIRE(GetVftableOffset<ObjectDeformable>(&ObjectDeformable::Destroy) == 0x0);
}

TEST_CASE("value offsets are correct", "[navbar]")
{
    REQUIRE(offsetof(NavBar, roomTransitionInProgress) == 0x344);
    REQUIRE(offsetof(NavBar, maneuverFrame) == 0x3D8);
    REQUIRE(offsetof(NavBar, dealerCharacter) == 0x3E0);
    REQUIRE(offsetof(NavBar, shipDealerMenuOpened) == 0x3E4);
}

TEST_CASE("value offsets are correct", "[dealeropencamera]")
{
    REQUIRE(offsetof(DealerOpenCamera, animationInProgress) == 0x1338);
}

// cheat_detection.h
TEST_CASE("value offsets are correct", "[playerdata]")
{
    REQUIRE(offsetof(PlayerData, currentShipId) == 0x264);
    REQUIRE(offsetof(PlayerData, shipIdOnLand) == 0x324);
}

TEST_CASE("value offsets are correct", "[basegood]")
{
    REQUIRE(offsetof(BaseGood, goodId) == 0x8);
    REQUIRE(offsetof(BaseGood, price) == 0xC);
    REQUIRE(offsetof(BaseGood, minQuantity) == 0x10);
    REQUIRE(offsetof(BaseGood, maxQuantity) == 0x14);
    REQUIRE(offsetof(BaseGood, unk_x18) == 0x18);
}

TEST_CASE("value offsets are correct", "[basegoodit]")
{
    REQUIRE(offsetof(BaseGoodIt, good) == 0x0);
}

TEST_CASE("value offsets are correct", "[basegoodcollection]")
{
    REQUIRE(offsetof(BaseGoodCollection, baseName) == 0x0);
    REQUIRE(offsetof(BaseGoodCollection, launchpadName) == 0x4);
    REQUIRE(offsetof(BaseGoodCollection, unk_x08) == 0x8);
    REQUIRE(offsetof(BaseGoodCollection, unk_x0C) == 0xC);
    REQUIRE(offsetof(BaseGoodCollection, goods) == 0x10);
}

TEST_CASE("value offsets are correct", "[marketgood]")
{
    REQUIRE(offsetof(MarketGood, type) == 0x10);
}

TEST_CASE("value offsets are correct", "[basemarket]")
{
    REQUIRE(offsetof(BaseMarket, baseName) == 0x0);
    REQUIRE(offsetof(BaseMarket, baseGoods) == 0x4);
}

// cursor_colors.h
TEST_CASE("size is correct", "[mousecursor]")
{
    REQUIRE(sizeof(MouseCursor) == 0x90);
}

TEST_CASE("value offsets are correct", "[mousecursor]")
{
    REQUIRE(offsetof(MouseCursor, nicknameLen) == 0x0);
    REQUIRE(offsetof(MouseCursor, nickname) == 0x4);
    REQUIRE(offsetof(MouseCursor, animNameLen) == 0x28);
    REQUIRE(offsetof(MouseCursor, animName) == 0x2C);
    REQUIRE(offsetof(MouseCursor, hotspotX) == 0x44);
    REQUIRE(offsetof(MouseCursor, hotspotY) == 0x48);
    REQUIRE(offsetof(MouseCursor, color) == 0x4C);
    REQUIRE(offsetof(MouseCursor, animValue1) == 0x60);
    REQUIRE(offsetof(MouseCursor, animState) == 0x64);
}

TEST_CASE("value offsets are correct", "[targetable_objects]")
{
    REQUIRE(offsetof(Targetable_Objects, selectedSimple) == 0x3F0);
    REQUIRE(offsetof(Targetable_Objects, isAimLocking) == 0x928);
}

// waypoint_names.h
TEST_CASE("value offsets are correct", "[missionobjective]")
{
    REQUIRE(offsetof(MissionObjective, fmtStr) == 0x0);
    REQUIRE(offsetof(MissionObjective, flags) == 0x18);
}

// mouse.h
TEST_CASE("vftable offsets are correct", "[idirectinputdevice8]")
{
    REQUIRE(GetVftableOffset<IDirectInputDevice8>(&IDirectInputDevice8::Acquire, true) == 0x1C);
    REQUIRE(GetVftableOffset<IDirectInputDevice8>(&IDirectInputDevice8::Unacquire, true) == 0x20);
    REQUIRE(GetVftableOffset<IDirectInputDevice8>(&IDirectInputDevice8::SetCooperativeLevel, true) == 0x34);
}

// fl_math.h
TEST_CASE("value offsets are correct", "[vector]")
{
    REQUIRE(offsetof(Vector, x) == 0x0);
    REQUIRE(offsetof(Vector, y) == 0x4);
    REQUIRE(offsetof(Vector, z) == 0x8);
}

TEST_CASE("length works", "[vector]")
{
    Vector v;
    v.x = 2;
    v.y = 3;
    v.z = 6;

    REQUIRE(v.Length() == 7.0f);
}

TEST_CASE("normalization works", "[vector]")
{
    Vector v;
    v.x = 3;
    v.y = 4;
    v.z = 12;
    v = v.Normalize();

    REQUIRE(v.x == 3.0f / 13.0f);
    REQUIRE(v.y == 4.0f / 13.0f);
    REQUIRE(v.z == 12.0f / 13.0f);
    REQUIRE(v.Length() == 1.0f);
}


TEST_CASE("value offsets are correct", "[quaternion]")
{
    REQUIRE(offsetof(Quaternion, w) == 0x0);
    REQUIRE(offsetof(Quaternion, x) == 0x4);
    REQUIRE(offsetof(Quaternion, y) == 0x8);
    REQUIRE(offsetof(Quaternion, z) == 0xC);
}

TEST_CASE("value offsets are correct", "[matrix]")
{
    REQUIRE(offsetof(Matrix, data) == 0x0);
}

TEST_CASE("transpose works", "[matrix]")
{
    Matrix m;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            m.data[i][j] = (float) ((i * 3) + j + 1);

    m = m.Transpose();

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            REQUIRE(m.data[j][i] == (float) ((i * 3) + j + 1));
}

TEST_CASE("matrix-vector multiplication works", "[matrix]")
{
    Matrix m;
    m.data[0][0] = 9.0f;
    m.data[0][1] = 5.0f;
    m.data[0][2] = 4.0f;
    m.data[1][0] = 6.0f;
    m.data[1][1] = 1.0f;
    m.data[1][2] = 2.0f;
    m.data[2][0] = 5.0f;
    m.data[2][1] = 8.0f;
    m.data[2][2] = 9.0f;

    Vector v = { 9.0f, 7.0f, 8.0f };

    Vector mv = m * v;

    REQUIRE(mv.x == 148.0f);
    REQUIRE(mv.y == 77.0f);
    REQUIRE(mv.z == 173.0f);
}

// light_limit.h
TEST_CASE("value offsets are correct", "[d3dcaps8]")
{
    REQUIRE(offsetof(D3DCAPS8, MaxActiveLights) == 0xA0);
}

TEST_CASE("vftable offsets are correct", "[idirect3ddevice8]")
{
    REQUIRE(GetVftableOffset<IDirect3DDevice8>(&IDirect3DDevice8::GetDeviceCaps, true) == 0x1C);
}

// exit.h
TEST_CASE("vftable offsets are correct", "[idirectplay8client]")
{
    REQUIRE(GetVftableOffset<IDirectPlay8Client>(&IDirectPlay8Client::CancelAsyncOperation, true) == 0x18);
    REQUIRE(GetVftableOffset<IDirectPlay8Client>(&IDirectPlay8Client::Close, true) == 0x38);
}

// sub_target.h
TEST_CASE("value offsets are correct", "[target]")
{
    REQUIRE(offsetof(Target, hudTarget) == 0x38);
}

TEST_CASE("value offsets are correct", "[hud_target]")
{
    REQUIRE(offsetof(HUD_Target, targetBaseBackground) == 0x494);
    REQUIRE(offsetof(HUD_Target, targetScanButton) == 0x4D0);
    REQUIRE(offsetof(HUD_Target, targetTractorButton) == 0x4D4);
    REQUIRE(offsetof(HUD_Target, targetTradeButton) == 0x4D8);
    REQUIRE(offsetof(HUD_Target, targetPreviousButton) == 0x4DC);
    REQUIRE(offsetof(HUD_Target, targetNextButton) == 0x4E0);
    REQUIRE(offsetof(HUD_Target, targetCommButton) == 0x4E4);
    REQUIRE(offsetof(HUD_Target, tradeRequestGroupButton) == 0x4E8);
    REQUIRE(offsetof(HUD_Target, targetCloseButton) == 0x4EC);
    REQUIRE(offsetof(HUD_Target, formationList) == 0x60C);
    REQUIRE(offsetof(HUD_Target, isPlayerInFormation) == 0x618);
}
