#pragma once

#define FILL_VFTABLE(tensPlace) \
    virtual void Vftable_x ##tensPlace## 0(); \
    virtual void Vftable_x ##tensPlace## 4(); \
    virtual void Vftable_x ##tensPlace## 8(); \
    virtual void Vftable_x ##tensPlace## C();
