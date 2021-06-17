//===--- DependencyDumper.cpp - Dump dependency file --------------------===//
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
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"


using namespace llvm;

static cl::opt<bool>
    ClEnableDepDump("enable-dependency-dumper", cl::Hidden, cl::init(false), cl::ZeroOrMore,
                cl::desc("Enable Dependency Dumper Pass"));

namespace {
    class DependencyDumper : public ModulePass {
    public:
        static char ID; // Pass identification, replacement for typeid
        DependencyDumper() : ModulePass(ID) {
        initializeDependencyDumperPass(*PassRegistry::getPassRegistry());
        }

        bool runOnModule(Module &M) override;
        // DependencyDump(Module &M);
    };
    
}

static void DependencyDump(Module &M) {
    // for (auto Func : M.getFunctionList()) {
        errs() << M.getSourceFileName() << '\n';
    // }
}

bool DependencyDumper::runOnModule(Module &M) {
        if (skipModule(M))
            return false;
        if (ClEnableDepDump)
            DependencyDump(M);
        return false;
        }

char DependencyDumper::ID = 0;

INITIALIZE_PASS(DependencyDumper, "dependencydump",
                "Dump Dependency during compilation", false, true)

ModulePass *llvm::createDependencyDumperPass() {
  return new DependencyDumper();
}
