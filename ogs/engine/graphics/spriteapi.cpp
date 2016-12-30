#include "system/precompiled.h"
#include "graphics/sprite.h"

HSPRITE_ SPR_Load(const char *szPicName)
{
};

int SPR_Frames(HSPRITE_ hPic)
{
};

int SPR_Height(HSPRITE_ hPic, int frame)
{
};

int SPR_Width(HSPRITE_ hPic, int frame)
{
};

void SPR_Set(HSPRITE_ hPic, int r, int g, int b)
{
};

void SPR_Draw(int frame, int x, int y, const wrect_t *prc)
{
};

void SPR_DrawHoles(int frame, int x, int y, const wrect_t *prc)
{
};

void SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc)
{
};

void SPR_EnableScissor(int x, int y, int width, int height)
{
};

void SPR_DisableScissor()
{
};

client_sprite_t *SPR_GetList(char *psz, int *piCount)
{
};

const struct model_s *EngFunc_GetSpritePointer(HSPRITE hSprite)
{
};