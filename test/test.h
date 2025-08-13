#include "catch_amalgamated.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

template <typename ObjType, typename ValType>
DWORD GetValueOffset(ObjType& obj, ValType& value)
{
    return ((DWORD) &value) - ((DWORD) &obj);
}

#define FILL_COUNTER_VFTABLE(tensPlace) \
    virtual DWORD Vftable_x ##tensPlace## 0() { return 0x ##tensPlace## 0;} \
    virtual DWORD Vftable_x ##tensPlace## 4() { return 0x ##tensPlace## 4;} \
    virtual DWORD Vftable_x ##tensPlace## 8() { return 0x ##tensPlace## 8;} \
    virtual DWORD Vftable_x ##tensPlace## C() { return 0x ##tensPlace## C;}

template <class ObjType, typename Func>
DWORD GetVftableOffset(Func func)
{
    struct CounterVftable
    {
        FILL_COUNTER_VFTABLE(0)
        FILL_COUNTER_VFTABLE(1)
        FILL_COUNTER_VFTABLE(2)
        FILL_COUNTER_VFTABLE(3)
        FILL_COUNTER_VFTABLE(4)
        FILL_COUNTER_VFTABLE(5)
        FILL_COUNTER_VFTABLE(6)
        FILL_COUNTER_VFTABLE(7)
        FILL_COUNTER_VFTABLE(8)
        FILL_COUNTER_VFTABLE(9)
    } vftable;

    ObjType* obj = (ObjType*) &vftable;

    typedef DWORD (ObjType::*GetIndex)();
    GetIndex getIndexFunc = (GetIndex) func;
    return (obj->*getIndexFunc)();
}
