#include "RISCV.h"
#include "RISCVInstrInfo.h"
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

#define RISCV_MACHINEBB_EDGE_PASS_NAME "Dummy RISCV machineBB edge pass"

namespace {

class RISCVMachineBBedge : public MachineFunctionPass {
public:
    static char ID;

    RISCVMachineBBedge() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return RISCV_MACHINEBB_EDGE_PASS_NAME; }

};


} // end of anonymous namespace

char RISCVMachineBBedge::ID = 0;

bool RISCVMachineBBedge::runOnMachineFunction(MachineFunction &MF) {

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

INITIALIZE_PASS(RISCVMachineBBedge, "RISCV-machineBB-edge",
    RISCV_MACHINEBB_EDGE_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)

namespace llvm {

FunctionPass *createRISCVMachineBBedgePass() { return new RISCVMachineBBedge(); }

}