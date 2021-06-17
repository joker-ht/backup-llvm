#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>

using namespace llvm;

#define RISCV_MACHINEBB_DETAIL_PASS_NAME "Dummy RISCV machineBB detail pass"

namespace {

class RISCVMachineBBdetail : public MachineFunctionPass {
public:
    static char ID;

    RISCVMachineBBdetail() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return RISCV_MACHINEBB_DETAIL_PASS_NAME; }

};

} // end of anonymous namespace

char RISCVMachineBBdetail::ID = 0;

bool RISCVMachineBBdetail::runOnMachineFunction(MachineFunction &MF) {

    std::string funcname = MF.getName();
    outs() << "FuncName:" << funcname << '\n';
    unsigned MFID = MF.getFunctionNumber();
    for (auto &MBB : MF) {
        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        outs() << "FUNC ID and BB ID:" << MBBID << '\n';
        outs() << MBB << "\n";
    }

    return false;
}

INITIALIZE_PASS(RISCVMachineBBdetail, "RISCV-machineBB-detail",
    RISCV_MACHINEBB_DETAIL_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createRISCVMachineBBdetailPass() { return new RISCVMachineBBdetail(); }

}