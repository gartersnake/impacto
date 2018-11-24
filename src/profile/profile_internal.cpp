#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

#include <rapidjson/error/en.h>

#include "sprites.h"
#include "fonts.h"
#include "animations.h"

namespace Impacto {
namespace Profile {

Document Json;
Value GlobalNull;

static char const* kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                   "Array", "String", "Number"};

void LoadJsonString(char const* str) {
  GlobalNull.SetNull();

  Json.Parse(str);

  if (Json.HasParseError()) {
    ImpLog(LL_Fatal, LC_Profile,
           "Failed to parse JSON from profile (at %d): %s\n",
           Json.GetErrorOffset(), GetParseError_En(Json.GetParseError()));
    Window::Shutdown();
  }
}

Value const& AssertIs(Value const& val, char const* path, Type type) {
  Type actualType = val.GetType();
  if (actualType == type) return val;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to have type %s, actual type %s\n",
         path, kTypeNames[type], kTypeNames[actualType]);
  Window::Shutdown();

  return GlobalNull;
}

Value const& AssertIsOneOf(Value const& val, char const* path,
                           std::initializer_list<Type> types) {
  Type actualType = val.GetType();
  for (auto type : types) {
    if (actualType == type) return val;
  }

  char typeList[80];  // should be enough given type name lengths
  memset(typeList, 0, sizeof(typeList));
  bool first = true;
  for (auto type : types) {
    if (!first) strcat(typeList, ", ");
    first = false;
    strcat(typeList, kTypeNames[type]);
  }

  ImpLog(LL_Fatal, LC_Profile,
         "Expected %s to have type in (%s), actual type %s\n", path, typeList,
         kTypeNames[actualType]);
  Window::Shutdown();

  return GlobalNull;
}

Value const& TryGetMember(Value const& val, char const* member,
                          bool& outSuccess) {
  auto it = val.FindMember(member);
  if (it != val.MemberEnd()) {
    outSuccess = true;
    return it->value;
  } else {
    outSuccess = false;
    return GlobalNull;
  }
}

Value const& EnsureGetMember(Value const& val, char const* path,
                             char const* member) {
  bool success;
  Value const& result = TryGetMember(val, member, success);
  if (success) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to have member %s\n", path, member);
  Window::Shutdown();

  return GlobalNull;
}
Value const& EnsureGetMemberOfType(Value const& val, char const* path,
                                   char const* member, Type type) {
  Value const& result = EnsureGetMember(val, path, member);

  Type actualType = result.GetType();
  if (actualType == type) return result;

  ImpLog(LL_Fatal, LC_Profile,
         "Expected %s/%s to have type %s, actual type %s\n", path, member,
         kTypeNames[type], kTypeNames[actualType]);
  Window::Shutdown();

  return GlobalNull;
}
Value const& EnsureGetMemberOfTypes(Value const& val, char const* path,
                                    char const* member,
                                    std::initializer_list<Type> types) {
  Value const& result = EnsureGetMember(val, path, member);

  Type actualType = result.GetType();
  for (auto type : types) {
    if (actualType == type) return result;
  }

  char typeList[80];  // should be enough given type name lengths
  memset(typeList, 0, sizeof(typeList));
  bool first = true;
  for (auto type : types) {
    if (!first) strcat(typeList, ", ");
    first = false;
    strcat(typeList, kTypeNames[type]);
  }

  ImpLog(LL_Fatal, LC_Profile,
         "Expected %s/%s to have type in (%s), actual type %s\n", path, member,
         typeList, kTypeNames[actualType]);
  Window::Shutdown();

  return GlobalNull;
}

bool TryGetBool(Value const& val, bool& outBool) {
  if (val.IsBool()) {
    outBool = val.GetBool();
    return true;
  }
  // TODO conversion?

  return false;
}

bool TryGetMemberBool(Value const& val, char const* member, bool& outBool) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetBool(it->value, outBool);
}

bool EnsureGetBool(Value const& val, char const* path) {
  bool result;
  if (TryGetBool(val, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be boolean\n", path);
  Window::Shutdown();

  return 0;
}

bool EnsureGetMemberBool(Value const& val, char const* path,
                         char const* member) {
  bool result;
  if (TryGetMemberBool(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be boolean\n", path, member);
  Window::Shutdown();

  return 0;
}

bool TryGetInt(Value const& val, int& outInt) {
  if (val.IsInt()) {
    outInt = val.GetInt();
    return true;
  }
  if (val.IsString()) {
    char* endp;
    outInt = SDL_strtol(val.GetString(), &endp, 10);
    if (endp != 0) return true;
  }

  return false;
}

bool TryGetMemberInt(Value const& val, char const* member, int& outInt) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetInt(it->value, outInt);
}

int EnsureGetInt(Value const& val, char const* path) {
  int result;
  if (TryGetInt(val, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be integer convertible\n", path);
  Window::Shutdown();

  return 0;
}

int EnsureGetMemberInt(Value const& val, char const* path, char const* member) {
  int result;
  if (TryGetMemberInt(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be integer convertible\n",
         path, member);
  Window::Shutdown();

  return 0;
}

bool TryGetUint(Value const& val, uint32_t& outUint) {
  if (val.IsUint()) {
    outUint = val.GetUint();
    return true;
  }
  if (val.IsString()) {
    char* endp;
    outUint = SDL_strtoul(val.GetString(), &endp, 10);
    if (endp != 0) return true;
  }

  return false;
}

bool TryGetMemberUint(Value const& val, char const* member, uint32_t& outUint) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetUint(it->value, outUint);
}

uint32_t EnsureGetUint(Value const& val, char const* path) {
  uint32_t result;
  if (TryGetUint(val, result)) return result;

  ImpLog(LL_Fatal, LC_Profile,
         "Expected %s to be unsigned integer convertible\n", path);
  Window::Shutdown();

  return 0;
}

uint32_t EnsureGetMemberUint(Value const& val, char const* path,
                             char const* member) {
  uint32_t result;
  if (TryGetMemberUint(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile,
         "Expected %s/%s to be unsigned integer convertible\n", path, member);
  Window::Shutdown();

  return 0;
}

bool TryGetFloat(Value const& val, float& outFloat) {
  if (val.IsNumber()) {
    outFloat = val.GetFloat();
    return true;
  }
  if (val.IsString()) {
    char* endp;
    outFloat = SDL_strtod(val.GetString(), &endp);
    if (endp != 0) return true;
  }

  return false;
}

bool TryGetMemberFloat(Value const& val, char const* member, float& outFloat) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetFloat(it->value, outFloat);
}

float EnsureGetFloat(Value const& val, char const* path) {
  float result;
  if (TryGetFloat(val, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be float convertible\n", path);
  Window::Shutdown();

  return 0;
}

float EnsureGetMemberFloat(Value const& val, char const* path,
                           char const* member) {
  float result;
  if (TryGetMemberFloat(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be float convertible\n", path,
         member);
  Window::Shutdown();

  return 0;
}

bool TryGetString(Value const& val, char const*& outString) {
  if (!val.IsString()) return false;
  outString = val.GetString();
  return true;
}

bool TryGetMemberString(Value const& val, char const* member,
                        char const*& outString) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetString(it->value, outString);
}

char const* EnsureGetString(Value const& val, char const* path) {
  char const* result;
  if (TryGetString(val, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s to be string\n", path);
  Window::Shutdown();

  return 0;
}

char const* EnsureGetMemberString(Value const& val, char const* path,
                                  char const* member) {
  char const* result;
  if (TryGetMemberString(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be string\n", path, member);
  Window::Shutdown();

  return 0;
}

bool TryGetVec2(Value const& val, glm::vec2& outVec) {
  if (!val.IsObject()) return false;

  return TryGetMemberFloat(val, "X", outVec.x) &&
         TryGetMemberFloat(val, "Y", outVec.y);
}

bool TryGetMemberVec2(Value const& val, char const* member, glm::vec2& outVec) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetVec2(it->value, outVec);
}

glm::vec2 EnsureGetVec2(Value const& val, char const* path) {
  glm::vec2 result;
  if (!TryGetVec2(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be Vec2\n", path);
    Window::Shutdown();
  }
  return result;
}

glm::vec2 EnsureGetMemberVec2(Value const& val, char const* path,
                              char const* member) {
  glm::vec2 result;
  if (TryGetMemberVec2(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be Vec2\n", path, member);
  Window::Shutdown();

  return result;
}

bool TryGetVec3(Value const& val, glm::vec3& outVec) {
  if (!val.IsObject()) return false;

  return TryGetMemberFloat(val, "X", outVec.x) &&
         TryGetMemberFloat(val, "Y", outVec.y) &&
         TryGetMemberFloat(val, "Z", outVec.z);
}

bool TryGetMemberVec3(Value const& val, char const* member, glm::vec3& outVec) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetVec3(it->value, outVec);
}

glm::vec3 EnsureGetVec3(Value const& val, char const* path) {
  glm::vec3 result;
  if (!TryGetVec3(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be Vec3\n", path);
    Window::Shutdown();
  }
  return result;
}

glm::vec3 EnsureGetMemberVec3(Value const& val, char const* path,
                              char const* member) {
  glm::vec3 result;
  if (TryGetMemberVec3(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be Vec3\n", path, member);
  Window::Shutdown();

  return result;
}

bool TryGetRectF(Value const& val, RectF& outRect) {
  if (!val.IsObject()) return false;

  return TryGetMemberFloat(val, "X", outRect.X) &&
         TryGetMemberFloat(val, "Y", outRect.Y) &&
         TryGetMemberFloat(val, "Width", outRect.Width) &&
         TryGetMemberFloat(val, "Height", outRect.Height);
}

bool TryGetMemberRectF(Value const& val, char const* member, RectF& outRect) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetRectF(it->value, outRect);
}

RectF EnsureGetRectF(Value const& val, char const* path) {
  RectF result;
  if (!TryGetRectF(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be RectF\n", path);
    Window::Shutdown();
  }
  return result;
}

RectF EnsureGetMemberRectF(Value const& val, char const* path,
                           char const* member) {
  RectF result;
  if (TryGetMemberRectF(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be RectF\n", path, member);
  Window::Shutdown();

  return result;
}

bool TryGetAssetPath(Value const& val, Io::AssetPath& outPath) {
  if (val.IsString()) {
    outPath.Mount = "";
    outPath.Id = 0;
    outPath.FileName = val.GetString();
    return true;
  } else {
    if (!val.IsObject()) return false;

    if (!TryGetMemberUint(val, "Id", outPath.Id)) return false;
    char const* _mount;
    if (!TryGetMemberString(val, "Mount", _mount)) return false;
    outPath.Mount = _mount;
    return true;
  }
}

bool TryGetMemberAssetPath(Value const& val, char const* member,
                           Io::AssetPath& outPath) {
  auto const it = val.FindMember(member);
  if (it == val.MemberEnd()) return false;
  return TryGetAssetPath(it->value, outPath);
}

Io::AssetPath EnsureGetAssetPath(Value const& val, char const* path) {
  Io::AssetPath result;
  if (!TryGetAssetPath(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be AssetPath\n", path);
    Window::Shutdown();
  }
  return result;
}

Io::AssetPath EnsureGetMemberAssetPath(Value const& val, char const* path,
                                       char const* member) {
  Io::AssetPath result;
  if (TryGetMemberAssetPath(val, member, result)) return result;

  ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be AssetPath\n", path,
         member);
  Window::Shutdown();

  return result;
}

bool TryGetSprite(Value const& val, Sprite& outSprite) {
  char const* spriteName;
  if (!TryGetString(val, spriteName)) return false;

  auto const spriteRef = Sprites.find(spriteName);
  if (spriteRef == Sprites.end()) return false;

  outSprite = spriteRef->second;
  return true;
}

bool TryGetMemberSprite(Value const& val, char const* member,
                        Sprite& outSprite) {
  bool success;
  Value const& _member = TryGetMember(val, member, success);
  if (!success) return false;

  return TryGetSprite(_member, outSprite);
}

Sprite EnsureGetSprite(Value const& val, char const* path) {
  Sprite result;
  if (!TryGetSprite(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be Sprite\n", path);
    Window::Shutdown();
  }
  return result;
}

Sprite EnsureGetMemberSprite(Value const& val, char const* path,
                             char const* member) {
  Sprite result;
  if (!TryGetMemberSprite(val, member, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be Sprite\n", path, member);
    Window::Shutdown();
  }
  return result;
}

bool TryGetSpriteSheet(Value const& val, SpriteSheet& outSheet) {
  char const* sheetName;
  if (!TryGetString(val, sheetName)) return false;

  auto const sheetRef = SpriteSheets.find(sheetName);
  if (sheetRef == SpriteSheets.end()) return false;

  outSheet = sheetRef->second;
  return true;
}

bool TryGetMemberSpriteSheet(Value const& val, char const* member,
                             SpriteSheet& outSheet) {
  bool success;
  Value const& _member = TryGetMember(val, member, success);
  if (!success) return false;

  return TryGetSpriteSheet(_member, outSheet);
}

SpriteSheet EnsureGetSpriteSheet(Value const& val, char const* path) {
  SpriteSheet result;
  if (!TryGetSpriteSheet(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be SpriteSheet\n", path);
    Window::Shutdown();
  }
  return result;
}

SpriteSheet EnsureGetMemberSpriteSheet(Value const& val, char const* path,
                                       char const* member) {
  SpriteSheet result;
  if (!TryGetMemberSpriteSheet(val, member, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be SpriteSheet\n", path,
           member);
    Window::Shutdown();
  }
  return result;
}

bool TryGetFont(Value const& val, Font& outFont) {
  char const* fontName;
  if (!TryGetString(val, fontName)) return false;

  auto const fontRef = Fonts.find(fontName);
  if (fontRef == Fonts.end()) return false;

  outFont = fontRef->second;
  return true;
}

bool TryGetMemberFont(Value const& val, char const* member, Font& outFont) {
  bool success;
  Value const& _member = TryGetMember(val, member, success);
  if (!success) return false;

  return TryGetFont(_member, outFont);
}

Font EnsureGetFont(Value const& val, char const* path) {
  Font result;
  if (!TryGetFont(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be Font\n", path);
    Window::Shutdown();
  }
  return result;
}

Font EnsureGetMemberFont(Value const& val, char const* path,
                         char const* member) {
  Font result;
  if (!TryGetMemberFont(val, member, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be Font\n", path, member);
    Window::Shutdown();
  }
  return result;
}

bool TryGetAnimation(Value const& val, SpriteAnimationDef& outAnimation) {
  char const* animName;
  if (!TryGetString(val, animName)) return false;

  auto const animRef = Animations.find(animName);
  if (animRef == Animations.end()) return false;

  outAnimation = animRef->second;
  return true;
}

bool TryGetMemberAnimation(Value const& val, char const* member,
                           SpriteAnimationDef& outAnimation) {
  bool success;
  Value const& _member = TryGetMember(val, member, success);
  if (!success) return false;

  return TryGetAnimation(_member, outAnimation);
}

SpriteAnimationDef EnsureGetAnimation(Value const& val, char const* path) {
  SpriteAnimationDef result;
  if (!TryGetAnimation(val, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s to be Animation\n", path);
    Window::Shutdown();
  }
  return result;
}

SpriteAnimationDef EnsureGetMemberAnimation(Value const& val, char const* path,
                                            char const* member) {
  SpriteAnimationDef result;
  if (!TryGetMemberAnimation(val, member, result)) {
    ImpLog(LL_Fatal, LC_Profile, "Expected %s/%s to be Animation\n", path,
           member);
    Window::Shutdown();
  }
  return result;
}

}  // namespace Profile
}  // namespace Impacto