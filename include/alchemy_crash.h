#pragma once

struct Alchemy
{
    float progress;
    void* effect;
};

struct AleLoop
{
    int startOffset;
    unsigned char maxProgressOffset;
};

void InitAlchemyCrashFix();
