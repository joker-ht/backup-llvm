#include "AArch64.h"
#include "AArch64InstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>
#include <map>
#include <list>
#include <set>

using namespace llvm;

#define AArch64_MACHINEBB_PRINTER_PASS_NAME "Dummy AArch64 machineBB printer pass"

namespace {

class AArch64MachineBBPrinter : public MachineFunctionPass {
public:
    static char ID;

    AArch64MachineBBPrinter() : MachineFunctionPass(ID) { }
    
    bool runOnMachineFunction(MachineFunction &MF) override;
    bool doFinalization(Module &M) override;

    StringRef getPassName() const override { return AArch64_MACHINEBB_PRINTER_PASS_NAME; }

    // (BB-> MBBs)s
    std::map<std::string, std::set<std::string>> BB2MBBs;
};

std::string getBBLabel(const llvm::BasicBlock *Node) {
    if (!Node) 
        return "NULL BasicBlock";

    if (!Node->getName().empty()) {
        return Node->getName().str();
    }

    std::string Str;
    llvm::raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}

std::string getBBLabel2(const llvm::BasicBlock *BB) {
    if (!BB) {
        return "NULL BasicBlock";
    }

    if (BB->hasName()) {
        return BB->getName().str();
    } else {
        const Function *F = BB->getParent();
        const Module *M = F->getParent();
        ModuleSlotTracker MST(M);
        MST.incorporateFunction(*F);
        int Slot = MST.getLocalSlot(BB);
        if (Slot == -1)
            return "<ir-block badref>";
        else
           return Twine(Slot).str();
    }
}

} // end of anonymous namespace

char AArch64MachineBBPrinter::ID = 0;

bool AArch64MachineBBPrinter::runOnMachineFunction(MachineFunction &MF) {

    unsigned MFID = MF.getFunctionNumber();
    outs() << MF.getName() << "\n";

    for (auto &MBB : MF) {
        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        const BasicBlock *BB = MBB.getBasicBlock();
        std::string BBID;
        if (!BB) {
            BBID = "NULL_BB_of_MF_" + std::to_string(MFID);
        } else {
            const Function *F = BB->getParent();
            BBID = F->getName().str() + "_" + getBBLabel(BB);
        }
        // traverse instr in the BB to see if it contains cmp
        // bool compare = 0;
        // for (auto &instr : MBB){
        //     if (instr.isCompare()){
        //         outs() << instr;
        //         compare = 1;
        //     }     
        // }
        // count succ of MBB to judge if it contains cmp
        int count = MBB.successors().end() - MBB.successors().begin();

        BB2MBBs[BBID].insert(MBBID);

        outs() << "MBB:" << MBBID << ",";
        // outs() << "compare:" << compare << ",";
        outs() << "SuccNub:" << count << ",";
        outs() << "Corres IR BB:" << BBID << ",";
        outs() << getBBLabel2(BB) << "\n";
    }

    return false;
}

bool AArch64MachineBBPrinter::doFinalization(Module &M) {
    // print the result of BB-MBB mapping by MBB::getBB()
    outs() << "Total BB size is " << BB2MBBs.size() << "\n";

    for(auto BBiter = BB2MBBs.begin(); BBiter != BB2MBBs.end(); BBiter++) {
        std::string BBID = BBiter->first;
        std::set<std::string> MBBs = BBiter->second;
        outs() << BBID << " has " << MBBs.size() << " MBBs : [";
        for(auto MBBiter = MBBs.begin(); MBBiter != MBBs.end(); MBBiter++) {
            std::string MBBID = *MBBiter;
            outs() << MBBID << ",";
        }
        outs() << "]\n";
    }

    return false;
}


INITIALIZE_PASS(AArch64MachineBBPrinter, "AArch64-machineBB-printer",
    AArch64_MACHINEBB_PRINTER_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)

namespace llvm {

FunctionPass *createAArch64MachineBBPrinterPass() { return new AArch64MachineBBPrinter(); }

}