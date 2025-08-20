#include "test.h"
#include "Common.h"
#include "iostream"

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
    REQUIRE(offsetof(Archetype::Ship, angularDrag) == 0x108);
    REQUIRE(offsetof(Archetype::Ship, steeringTorque) == 0x114);
}

TEST_CASE("value offsets are correct", "[archetype::eqobj]")
{
    REQUIRE(offsetof(Archetype::EqObj, idsName) == 0x14);
    REQUIRE(offsetof(Archetype::EqObj, idsInfo) == 0x18);
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

TEST_CASE("vftable offsets are correct", "[fuseaction]")
{
    REQUIRE(GetVftableOffset<FuseAction>(&FuseAction::IsTriggered) == 0x4);
}

TEST_CASE("value offsets are correct", "[iobjrw]")
{
    REQUIRE(offsetof(IObjRW, cobject) == 0x10);
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
