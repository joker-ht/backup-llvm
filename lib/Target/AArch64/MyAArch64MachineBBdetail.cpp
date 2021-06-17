#include "AArch64.h"
#include "AArch64InstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>

using namespace llvm;

#define AArch64_MACHINEBB_DETAIL_PASS_NAME "Dummy AArch64 machineBB detail pass"

namespace {

class AArch64MachineBBdetail : public MachineFunctionPass {
public:
    static char ID;

    AArch64MachineBBdetail() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return AArch64_MACHINEBB_DETAIL_PASS_NAME; }

};

} // end of anonymous namespace

char AArch64MachineBBdetail::ID = 0;

bool AArch64MachineBBdetail::runOnMachineFunction(MachineFunction &MF) {
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

INITIALIZE_PASS(AArch64MachineBBdetail, "AArch64-machineBB-detail",
    AArch64_MACHINEBB_DETAIL_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createAArch64MachineBBdetailPass() { return new AArch64MachineBBdetail(); }

}