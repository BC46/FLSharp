#include "test.h"
#include "Common.h"

TEST_CASE("value offsets are correct", "[archetype::root]")
{
    REQUIRE(offsetof(Archetype::Root, scriptIndex) == 0x44);
}

TEST_CASE("vftable offsets are correct", "[cattachedequip]")
{
    REQUIRE(GetVftableOffset<CAttachedEquip>(&CAttachedEquip::GetRootIndex) == 0x84);
}
