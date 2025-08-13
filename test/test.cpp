#include "test.h"
#include "Common.h"

TEST_CASE("value offsets are correct", "[archetype::root]")
{
    Archetype::Root root;
    REQUIRE(GetValueOffset(root, root.scriptIndex) == 0x44);
}

TEST_CASE("vftable offsets are correct", "[cattachedequip]")
{
    REQUIRE(GetVftableOffset<CAttachedEquip>(&CAttachedEquip::GetRootIndex) == 0x80);
}
