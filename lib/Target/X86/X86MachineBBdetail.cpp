#include "X86.h"
#include "X86InstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>

using namespace llvm;

#define X86_MACHINEBB_DETAIL_PASS_NAME "Dummy X86 machineBB detail pass"

namespace {

class X86MachineBBdetail : public MachineFunctionPass {
public:
    static char ID;

    X86MachineBBdetail() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return X86_MACHINEBB_DETAIL_PASS_NAME; }

};

} // end of anonymous namespace

char X86MachineBBdetail::ID = 0;

bool X86MachineBBdetail::runOnMachineFunction(MachineFunction &MF) {

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

INITIALIZE_PASS(X86MachineBBdetail, "x86-machineBB-detail",
    X86_MACHINEBB_DETAIL_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createX86MachineBBdetailPass() { return new X86MachineBBdetail(); }

}