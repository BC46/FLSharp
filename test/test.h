#include "catch_amalgamated.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FILL_COUNTER_VFTABLE(callingConvention, tensPlace) \
    virtual int callingConvention Vftable_x ##tensPlace## 0() { return 0x ##tensPlace## 0;} \
    virtual int callingConvention Vftable_x ##tensPlace## 4() { return 0x ##tensPlace## 4;} \
    virtual int callingConvention Vftable_x ##tensPlace## 8() { return 0x ##tensPlace## 8;} \
    virtual int callingConvention Vftable_x ##tensPlace## C() { return 0x ##tensPlace## C;}

#define DEFINE_COUNTER_VFTABLE(callingConvention) \
    FILL_COUNTER_VFTABLE(callingConvention, 0) \
    FILL_COUNTER_VFTABLE(callingConvention, 1) \
    FILL_COUNTER_VFTABLE(callingConvention, 2) \
    FILL_COUNTER_VFTABLE(callingConvention, 3) \
    FILL_COUNTER_VFTABLE(callingConvention, 4) \
    FILL_COUNTER_VFTABLE(callingConvention, 5) \
    FILL_COUNTER_VFTABLE(callingConvention, 6) \
    FILL_COUNTER_VFTABLE(callingConvention, 7) \
    FILL_COUNTER_VFTABLE(callingConvention, 8) \
    FILL_COUNTER_VFTABLE(callingConvention, 9) \
    FILL_COUNTER_VFTABLE(callingConvention, A) \
    FILL_COUNTER_VFTABLE(callingConvention, B) \
    FILL_COUNTER_VFTABLE(callingConvention, C) \
    FILL_COUNTER_VFTABLE(callingConvention, D) \
    FILL_COUNTER_VFTABLE(callingConvention, E) \
    FILL_COUNTER_VFTABLE(callingConvention, F)

template <class ObjType, typename Func>
int GetVftableOffset(Func func, bool stdcall = false)
{
    struct ThiscallCounterVftable
    {
        DEFINE_COUNTER_VFTABLE(__thiscall)
    } thiscallVftable;

    struct StdcallCounterVftable
    {
        DEFINE_COUNTER_VFTABLE(__stdcall)
    } stdcallVftable;

    ObjType* obj = stdcall ? (ObjType*) &stdcallVftable : (ObjType*) &thiscallVftable;

    if (stdcall)
    {
        typedef int (__stdcall ObjType::*GetIndexStdcall)();
        GetIndexStdcall getIndexFunc = (GetIndexStdcall) func;
        return (obj->*getIndexFunc)();
    }

    typedef int (__thiscall ObjType::*GetIndexThiscall)();
    GetIndexThiscall getIndexFunc = (GetIndexThiscall) func;
    return (obj->*getIndexFunc)();
}
