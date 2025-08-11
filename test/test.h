#include "catch_amalgamated.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Verifies that a value lies at the correct offset within the struct/class.
// Every value is tested individually and all bits not representing that value are expecting to be 0s,
// whereas the bits that do represent that values must be 1.
// TODO: calculate offset difference instead, much easier approach.
// TODO: Add way to verify function vftable offsets
template <typename ObjType, typename ValType>
void VerifyValueOffset(ObjType& obj, ValType& value, DWORD valueOffset)
{
    // Just to be sure that the given value actually fits within the object.
    REQUIRE(valueOffset + sizeof(value) <= sizeof(obj));

    // Check the offset.
    REQUIRE(((DWORD) &value) - ((DWORD) &obj) == valueOffset);
}
