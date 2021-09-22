//===--- MarkInstIndex.cpp - Mark Inst Index --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This code dumps dependency files.
//
//===----------------------------------------------------------------------===//
// #include "llvm/IR/Function.h"
// dingzhu patch
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/Passes.h"



using namespace llvm;

// static cl::opt<bool>
//     ClEnableDepDump("enable-dependency-dumper", cl::Hidden, cl::init(false), cl::ZeroOrMore,
//                 cl::desc("Enable Dependency Dumper Pass"));

namespace {
    class MarkInstIndex : public FunctionPass {
    public:
        static char ID; // Pass identification, replacement for typeid
        MarkInstIndex() : FunctionPass(ID) {
        initializeMarkInstIndexPass(*PassRegistry::getPassRegistry());
        }

        bool runOnFunction(Function &F) override;
    };
    
}

static void MarkEveryInst(Function &F) {
    std::string funcname = F.getName();
    for (auto &BB : F) {
        std::string bblabel = BB.getName();
        int index = 0;
        for (auto &Inst : BB) {
            ++index;
            InstIndex tmpIndex;
            DebugLoc tmp;
            tmpIndex.setFuncName(funcname);
            tmpIndex.setBBLabel(bblabel);
            tmpIndex.setInstNum(index);

            Inst.setInstIndex(tmpIndex);
            // Inst.getInstIndex().dump();
        }
    }

}

bool MarkInstIndex::runOnFunction(Function &F) {
        if (skipFunction(F))
            return false;
        MarkEveryInst(F);
        return false;
        }

char MarkInstIndex::ID = 0;

INITIALIZE_PASS(MarkInstIndex, "markinstindex",
                "Mark Inst Index before backend", false, true)

FunctionPass *llvm::createMarkInstIndexPass() {
  return new MarkInstIndex();
}
