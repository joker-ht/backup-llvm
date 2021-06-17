#include "Mips.h"
#include "MipsInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>

using namespace llvm;

#define Mips_MACHINEBB_DETAIL_PASS_NAME "Dummy Mips machineBB detail pass"

namespace {

class MipsMachineBBdetail : public MachineFunctionPass {
public:
    static char ID;

    MipsMachineBBdetail() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return Mips_MACHINEBB_DETAIL_PASS_NAME; }

};

} // end of anonymous namespace

char MipsMachineBBdetail::ID = 0;

bool MipsMachineBBdetail::runOnMachineFunction(MachineFunction &MF) {
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

INITIALIZE_PASS(MipsMachineBBdetail, "Mips-machineBB-detail",
    Mips_MACHINEBB_DETAIL_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createMipsMachineBBdetailPass() { return new MipsMachineBBdetail(); }

}