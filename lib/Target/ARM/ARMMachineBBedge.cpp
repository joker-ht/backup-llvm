#include "ARM.h"
#include "ARMInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>
#include <list>

using namespace llvm;

#define ARM_MACHINEBB_EDGE_PASS_NAME "Dummy ARM machineBB edge pass"

namespace {

class ARMMachineBBedge : public MachineFunctionPass {
public:
    static char ID;

    ARMMachineBBedge() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return ARM_MACHINEBB_EDGE_PASS_NAME; }

};


} // end of anonymous namespace

char ARMMachineBBedge::ID = 0;

bool ARMMachineBBedge::runOnMachineFunction(MachineFunction &MF) {

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

INITIALIZE_PASS(ARMMachineBBedge, "ARM-machineBB-edge",
    ARM_MACHINEBB_EDGE_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)

namespace llvm {

FunctionPass *createARMMachineBBedgePass() { return new ARMMachineBBedge(); }

}