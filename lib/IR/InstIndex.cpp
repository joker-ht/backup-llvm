//===-- InstIndex.cpp - Implement InstIndex class ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// dingzhu patch

#include "llvm/IR/InstIndex.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/Debug.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// InstIndex Implementation
//===----------------------------------------------------------------------===//
InstIndex::InstIndex(const InstIndex& SrcIndex) {
  FuncName = SrcIndex.FuncName;
  BBLabel = SrcIndex.BBLabel;
  InstNum = SrcIndex.InstNum;
}

InstIndex& InstIndex::operator = (const InstIndex& SrcIndex) {
  if (this == &SrcIndex) {
    return *this;
  }

  FuncName = SrcIndex.FuncName;
  BBLabel = SrcIndex.BBLabel;
  InstNum = SrcIndex.InstNum;

  return *this;

}

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
LLVM_DUMP_METHOD void InstIndex::dump() const { print(dbgs()); }
#endif

void InstIndex::print(raw_ostream &OS) const {
  OS << " Index:" << FuncName << '%' << BBLabel << '%' << InstNum;
}

