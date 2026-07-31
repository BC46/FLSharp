#pragma once

#include "Freelancer.h"

struct HUD_TargetH : public HUD_Target
{
    void RenderTargetBaseBackground_Hook();
};

void InitTargetTradeBlinkFix();
