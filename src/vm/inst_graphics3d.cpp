#include "inst_graphics3d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstCHAload3D) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(unk01);
  PopExpression(modelId);
  ImpLogSlow(
      LL_Warning, LC_VMStub,
      "STUB instruction CHAload3D(bufferId: %i, unk01: %i, modelId: %i)\n",
      bufferId, unk01, modelId);
}
VmInstruction(InstCHArelease3D) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHArelease3D(bufferId: %i)\n", bufferId);
}
VmInstruction(InstUnk0204) {}
VmInstruction(InstCHAswap3DMaybe) {}
VmInstruction(InstCHAplayAnim3DMaybe) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(animationId);
  PopUint8(unk01);
  ImpLogSlow(
      LL_Warning, LC_VMStub,
      "STUB instruction CHAplayAnim3DMaybe(bufferId: %i, animationId: %i, "
      "unk01: %i)\n",
      bufferId, animationId, unk01);
}
VmInstruction(InstCHAUnk02073D) {
  StartInstruction;
  PopExpression(bufferId);
  PopUint8(unk01);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAUnk02073D(bufferId: %i, unk01: %i)\n",
             bufferId, unk01);
}
VmInstruction(InstUnk0208) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopExpression(arg4);
  PopExpression(arg5);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0208(arg1: %i, arg2: %i, arg3: %i, arg4: %i, "
             "arg5: %i)\n",
             arg1, arg2, arg3, arg4, arg5);
}
VmInstruction(InstCHAsetAnim3D) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(animationId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAsetAnim3D(bufferId: %i, animationId: %i)\n",
             bufferId, animationId);
}
VmInstruction(InstUnk0210) {}
VmInstruction(InstUnk0211) {}
VmInstruction(InstUnk0212) {}
VmInstruction(InstUnk0213) {}
VmInstruction(InstUnk0214) {}
VmInstruction(InstUnk0215) {}
VmInstruction(InstUnk0216) {}
VmInstruction(InstUnk0217) {}
VmInstruction(InstUnk0218) {}
VmInstruction(InstUnk0219) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0219(bufferId: %i)\n",
             bufferId);
}
VmInstruction(InstUnk0240) {}

}  // namespace Vm

}  // namespace Impacto