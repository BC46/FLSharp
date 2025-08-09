#include "catch_amalgamated.hpp"
#include "Common.h"

// Verifies that a value lies at the correct offset within the struct/class.
// Every value is tested individually and all bits not representing that value are expecting to be 0s,
// whereas the bits that do represent that values must be 1.
void VerifyValueOffset(PVOID obj, UINT objSize, DWORD valueOffset, UINT valueSize)
{
    // Just to be sure that the given value actually fits within the object.
    REQUIRE(valueOffset + valueSize <= objSize);

    // All bytes in the object that come before the start of the value must be 0.
    for (PBYTE byte = (PBYTE) obj; byte < ((PBYTE) obj) + valueOffset; ++byte)
        REQUIRE(*byte == 0x00);

    // All bytes that are part of the value must be 0xFF (all 1 bits).
    for (PBYTE byte = (PBYTE) obj + valueOffset; byte < ((PBYTE) obj) + valueOffset + valueSize; ++byte)
        REQUIRE(*byte == 0xFF);

    // All bytes in the object that come after the end of the value must be 0.
    for (PBYTE byte = ((PBYTE) obj) + valueOffset + valueSize; byte < ((PBYTE) obj) + objSize; ++byte)
        REQUIRE(*byte == 0x00);
}

TEST_CASE("scriptIndex is at the expected offset", "[archetype::root]")
{
    Archetype::Root root;
    ZeroMemory(&root, sizeof(root));

    root.scriptIndex = 0xFFFFFFFF;
    VerifyValueOffset(&root, sizeof(root), 0x44, sizeof(Archetype::Root::scriptIndex));
}
