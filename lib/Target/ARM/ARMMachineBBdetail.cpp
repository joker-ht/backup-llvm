#include "ARM.h"
#include "ARMInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>

using namespace llvm;

#define ARM_MACHINEBB_DETAIL_PASS_NAME "Dummy ARM machineBB detail pass"

namespace {

class ARMMachineBBdetail : public MachineFunctionPass {
public:
    static char ID;

    ARMMachineBBdetail() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return ARM_MACHINEBB_DETAIL_PASS_NAME; }

};

} // end of anonymous namespace

char ARMMachineBBdetail::ID = 0;

bool ARMMachineBBdetail::runOnMachineFunction(MachineFunction &MF) {
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

INITIALIZE_PASS(ARMMachineBBdetail, "ARM-machineBB-detail",
    ARM_MACHINEBB_DETAIL_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createARMMachineBBdetailPass() { return new ARMMachineBBdetail(); }

}