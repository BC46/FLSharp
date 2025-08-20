#include "catch_amalgamated.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FILL_COUNTER_VFTABLE(tensPlace) \
    virtual int Vftable_x ##tensPlace## 0() { return 0x ##tensPlace## 0;} \
    virtual int Vftable_x ##tensPlace## 4() { return 0x ##tensPlace## 4;} \
    virtual int Vftable_x ##tensPlace## 8() { return 0x ##tensPlace## 8;} \
    virtual int Vftable_x ##tensPlace## C() { return 0x ##tensPlace## C;}

template <class ObjType, typename Func>
int GetVftableOffset(Func func)
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

    typedef int (ObjType::*GetIndex)();
    GetIndex getIndexFunc = (GetIndex) func;
    return (obj->*getIndexFunc)();
}
