#include "test.h"
#include "alchemy_crash.h"
#include "Common.h"
#include "copy_paste.h"
#include "flash_particles.h"
#include "Freelancer.h"
#include "RemoteServer.h"
#include "rep_requirements.h"
#include "resolutions.h"
#include "test_sounds.h"
#include "trade_lane_lights.h"
#include "ui_anim.h"
#include "weapon_anim.h"
#include "dealer_fixes.h"
#include "cheat_detection.h"

// alchemy_crash.h
TEST_CASE("value offsets are correct", "[alchemy]")
{
    REQUIRE(offsetof(Alchemy, progress) == 0x0);
    REQUIRE(offsetof(Alchemy, effect) == 0x4);
}

// Common.h
TEST_CASE("vftable offsets are correct", "[cequip]")
{
    REQUIRE(GetVftableOffset<CEquip>(&CEquip::Activate) == 0x30);
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
}

TEST_CASE("vftable offsets are correct", "[iobjrw]")
{
    REQUIRE(GetVftableOffset<IObjRW>(&IObjRW::get_attitude_towards) == 0x68);
    REQUIRE(GetVftableOffset<IObjRW>(&IObjRW::is_player) == 0xBC);
}

TEST_CASE("value offsets are correct", "[physicsinfo]")
{
    REQUIRE(offsetof(PhysicsInfo, autoLevel) == 0x2F);
}

TEST_CASE("value offsets are correct", "[ibehaviormanager]")
{
    REQUIRE(offsetof(IBehaviorManager, physicsInfo) == 0x08);
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
    REQUIRE(offsetof(NN_ShipTrader, selectedShipIndex) == 0x3D0);
    REQUIRE(offsetof(NN_ShipTrader, shipRepPercentages) == 0x448);
}

TEST_CASE("value offsets are correct", "[flcursor]")
{
    REQUIRE(offsetof(FLCursor, xPos) == 0x0);
    REQUIRE(offsetof(FLCursor, yPos) == 0x4);
    REQUIRE(offsetof(FLCursor, distFromZero) == 0x8);
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
    REQUIRE(GetVftableOffset<UITextMsgButton>(&UITextMsgButton::UpdatePosition) == 0xA8);
}

// weapon_anim.h
TEST_CASE("value offsets are correct", "[modelbinary]")
{
    REQUIRE(offsetof(ModelBinary, type) == 0x00);
    REQUIRE(offsetof(ModelBinary, parent) == 0x10);
}

TEST_CASE("vftable offsets are correct", "[ianimation2]")
{
    REQUIRE(GetVftableOffset<IAnimation2>(&IAnimation2::Open, true) == 0x24);
}

// dealer_fixes.h
TEST_CASE("value offsets are correct", "[maneuverframe]")
{
    REQUIRE(offsetof(ManeuverFrame, flags) == 0x6C);
}

TEST_CASE("value offsets are correct", "[navbar]")
{
    REQUIRE(offsetof(NavBar, maneuverFrame) == 0x3D8);
    REQUIRE(offsetof(NavBar, unkUiElement) == 0x3E0);
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
