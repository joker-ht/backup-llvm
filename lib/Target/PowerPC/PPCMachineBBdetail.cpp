#include "PPC.h"
#include "PPCInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>

using namespace llvm;

#define PPC_MACHINEBB_DETAIL_PASS_NAME "Dummy PPC machineBB detail pass"

namespace {

class PPCMachineBBdetail : public MachineFunctionPass {
public:
    static char ID;

    PPCMachineBBdetail() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return PPC_MACHINEBB_DETAIL_PASS_NAME; }

};

} // end of anonymous namespace

char PPCMachineBBdetail::ID = 0;

bool PPCMachineBBdetail::runOnMachineFunction(MachineFunction &MF) {
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

INITIALIZE_PASS(PPCMachineBBdetail, "PPC-machineBB-detail",
    PPC_MACHINEBB_DETAIL_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createPPCMachineBBdetailPass() { return new PPCMachineBBdetail(); }

}