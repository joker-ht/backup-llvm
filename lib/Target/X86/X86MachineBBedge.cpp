#include "X86.h"
#include "X86InstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>
// #include <map>
#include <list>

using namespace llvm;

#define X86_MACHINEBB_EDGE_PASS_NAME "Dummy X86 machineBB edge pass"

namespace {

class X86MachineBBedge : public MachineFunctionPass {
public:
    static char ID;

    X86MachineBBedge() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return X86_MACHINEBB_EDGE_PASS_NAME; }
};


} // end of anonymous namespace

char X86MachineBBedge::ID = 0;

bool X86MachineBBedge::runOnMachineFunction(MachineFunction &MF) {

    unsigned MFID = MF.getFunctionNumber();
    outs() << "---EdgePass---\n";
    outs() << MF.getName() << "\n";
    for (auto &MBB : MF) {
        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        outs() << "MachineBasicBlock:";
        outs() << MBBID << ",";
        for (MachineBasicBlock *succ : MBB.successors()){
            std::string succID = std::to_string(succ->getNumber());
            outs() << "succ: "<<succID << " ";
        }
        outs() << "\n";
    }

    return false;
}

INITIALIZE_PASS(X86MachineBBedge, "x86-machineBB-edge",
    X86_MACHINEBB_EDGE_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)

namespace llvm {

FunctionPass *createX86MachineBBedgePass() { return new X86MachineBBedge(); }

}