#include "test.h"
#include "Common.h"

TEST_CASE("value offsets are correct", "[archetype::root]")
{
    Archetype::Root root;
    VerifyValueOffset(root, root.scriptIndex, 0x44);
}
