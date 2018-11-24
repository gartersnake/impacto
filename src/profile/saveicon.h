#pragma once

#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

extern SpriteAnimationDef ForegroundAnimation;
extern glm::vec2 DefaultPosition;
extern Sprite BackgroundSprite;
extern glm::vec2 BackgroundOffset;
extern float BackgroundMaxAlpha;

void Configure();

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto