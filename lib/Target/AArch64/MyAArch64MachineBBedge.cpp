#include "AArch64.h"
#include "AArch64InstrInfo.h"
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

#define AArch64_MACHINEBB_EDGE_PASS_NAME "Dummy AArch64 machineBB edge pass"

namespace {

class AArch64MachineBBedge : public MachineFunctionPass {
public:
    static char ID;

    AArch64MachineBBedge() : MachineFunctionPass(ID) { }
 
    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return AArch64_MACHINEBB_EDGE_PASS_NAME; }
};


} // end of anonymous namespace

char AArch64MachineBBedge::ID = 0;

bool AArch64MachineBBedge::runOnMachineFunction(MachineFunction &MF) {

    unsigned MFID = MF.getFunctionNumber();
    outs() << "---edgePass---\n";
    outs() << MF.getName() << "\n";
    for (auto &MBB : MF) {
        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        outs() << "MachineBasicBlock:";
        outs() << MBBID << ",";
        for (MachineBasicBlock *succ : MBB.successors()){
            std::string succID = std::to_string(succ->getNumber());
            outs() << "succ: " <<succID << " ";
        }
        outs() << "\n";
    }

    return false;
}

INITIALIZE_PASS(AArch64MachineBBedge, "AArch64-machineBB-edge",
    AArch64_MACHINEBB_EDGE_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)

namespace llvm {

FunctionPass *createAArch64MachineBBedgePass() { return new AArch64MachineBBedge(); }

}