struct Maneuvers
{
    void Update();
};

struct HUD_Maneuvers
{
    void SetVisibility_Hook(bool hidden);
    Maneuvers* GetManeuvers() const;
};

void InitManeuverFrameFix();
