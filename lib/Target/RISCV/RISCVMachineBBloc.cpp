#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <llvm/IR/DebugInfo.h>
#include <string>
#include "llvm/CodeGen/MachineInstr.h"

using namespace llvm;

#define RISCV_MACHINEBB_LOC_PASS_NAME "Dummy RISCV machineBB loc pass"

namespace {

class RISCVMachineBBloc : public MachineFunctionPass {
public:
    static char ID;

    RISCVMachineBBloc() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return RISCV_MACHINEBB_LOC_PASS_NAME; }

};

} // end of anonymous namespace

char RISCVMachineBBloc::ID = 0;

std::string cpx_dbginfo;

static std::string get_tree_srcline(const llvm::DebugLoc *debugInfo){
    
    auto *Scope = cast<DIScope>(debugInfo->getScope());
    std::string filename = (*Scope).getFilename();
    unsigned linenub = debugInfo->getLine();
    unsigned colnub = debugInfo->getCol();

    std::string res = filename + ":" + std::to_string(linenub) + ":" + std::to_string(colnub);

    // DebugLoc temp = debugInfo->getInlinedAt();
    if(DebugLoc temp = debugInfo->getInlinedAt()){
        res += " @[ ";
        res += get_tree_srcline(&temp);
        res += "]";
    }

    return res;
}

static const MachineFunction *getMFIfAvailable(const MachineInstr &MI) {
  if (const MachineBasicBlock *MBB = MI.getParent())
    if (const MachineFunction *MF = MBB->getParent())
      return MF;
  return nullptr;
}

bool RISCVMachineBBloc::runOnMachineFunction(MachineFunction &MF) {
    std::string funcname = MF.getName();
    outs() << "FuncName:" << funcname << "\n";
    unsigned MFID = MF.getFunctionNumber();
    for (auto &MBB : MF) {

        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        outs() << "    FUNC ID and BB ID:" << MBBID << "\n";
        outs() << "    pred:";
        for (auto *pred : MBB.predecessors()){
            outs() << std::to_string(MFID) + "_" + std::to_string(pred->getNumber()) << ",";
        }
        outs() << "\n";
        outs() << "    succ:";
        for (auto *succ : MBB.successors()){
            outs() << std::to_string(MFID) + "_" + std::to_string(succ->getNumber()) << ",";
        }
        outs() << "\n";

        for (auto &instr : MBB){
            const MachineFunction *MF = getMFIfAvailable(instr);
            const TargetInstrInfo *TII = MF->getSubtarget().getInstrInfo();
            if (instr.isDebugInstr())
                continue;
            if (TII){
                outs() << "        " <<TII->getName(instr.getOpcode());
                if (instr.isCompare())
                    outs() << " is cmp";
                outs() << "\n";
            }
            if (instr.getDebugLoc()){
                const llvm::DebugLoc debugInfo = instr.getDebugLoc();

                // // use getLine() to get leaf point srcline
                // auto *Scope = cast<DIScope>(debugInfo.getScope());
                // unsigned int linenub = debugInfo.getLine() ;
                // unsigned int columnnub = debugInfo.getCol();
                // std::string filename = (*Scope).getFilename();
                // std::string dbginfo = filename + ":" + std::to_string(linenub) + ":" + std::to_string(columnnub);

                // get nested inline srcline
                std::string dbginfo = get_tree_srcline(&debugInfo);
                outs() << "        " << dbginfo << "\n";
            } 
        }
        outs() << "\n";
    }

    return false;
}

INITIALIZE_PASS(RISCVMachineBBloc, "RISCV-machineBB-loc",
    RISCV_MACHINEBB_LOC_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createRISCVMachineBBlocPass() { return new RISCVMachineBBloc(); }

}