#include "AArch64.h"
#include "AArch64InstrInfo.h"
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

#define AArch64_MACHINEBB_LOC_PASS_NAME "Dummy AArch64 machineBB loc pass"

namespace {

class AArch64MachineBBloc : public MachineFunctionPass {
public:
    static char ID;

    AArch64MachineBBloc() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return AArch64_MACHINEBB_LOC_PASS_NAME; }

    // std::string get_tree_srcline(const llvm::DebugLoc *debugInfo);
    
};

} // end of anonymous namespace

char AArch64MachineBBloc::ID = 0;

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

static unsigned PrintBBPred(MachineBasicBlock &MBB,signed MFID){
    outs() << "    pred:";
    int count = 0;
    for (auto *pred : MBB.predecessors()){
        count++;
        outs() << std::to_string(MFID) + "_" + std::to_string(pred->getNumber()) << ",";
    }
    outs() << " pred_nub:" << count << "\n";
    return count;
}

static unsigned PrintBBSucc(MachineBasicBlock &MBB,signed MFID){
    outs() << "    succ:";
    int count = 0;
    for (auto *succ : MBB.successors()){
        count++;
        outs() << std::to_string(MFID) + "_" + std::to_string(succ->getNumber()) << ",";
    }
    outs() << " succ_nub:" << count << "\n";
    return count;
}

static bool isTrueCompre(MachineInstr &instr){
    const MachineFunction *MF = getMFIfAvailable(instr);
    const TargetInstrInfo *TII = MF->getSubtarget().getInstrInfo();
    if (instr.isCompare()) return true;
    
    if (TII){
        std::string opcodename = TII->getName(instr.getOpcode());
        if (instr.getDesc().getFlags() == 768){
            if (opcodename != "Bcc") 
                return true;
        }
        else if (opcodename.find("ANDS") != std::string::npos){
            return true;
        }
    }
    return false;
}

bool AArch64MachineBBloc::runOnMachineFunction(MachineFunction &MF) {
    std::string funcname = MF.getName();
    outs() << "FuncName:" << funcname << "\n";
    unsigned MFID = MF.getFunctionNumber();
    for (auto &MBB : MF) {

        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        outs() << "    FUNC ID and BB ID:" << MBBID << "\n";

        PrintBBPred(MBB,MFID);
        // refr to succnub to verify if BB has cmp
        signed succnub = PrintBBSucc(MBB,MFID);
        signed cmpnub = 0;
        for (auto &instr : MBB){
            if (instr.isDebugInstr() || instr.isCFIInstruction())
                continue;
            outs() << "        " << instr;
            const MachineFunction *MF = getMFIfAvailable(instr);
            const TargetInstrInfo *TII = MF->getSubtarget().getInstrInfo();
            std::string opcodename = TII->getName(instr.getOpcode());
            outs() << "        " << opcodename << " " << instr.getDesc().getFlags();

            if (isTrueCompre(instr)){
                outs() << " is cmp";
                cmpnub += 1;
            }
            outs() << "\n";

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
        // 
        if (cmpnub==0 && succnub==2) outs() << "    may switch or miss cmp instr" << "\n";
        if (cmpnub > 1) outs() << "    this BB contains more than one cmp:" << cmpnub << "\n";
        if (cmpnub >=1 && succnub <2) outs() << "    this BB contains cmp but has less than 2 succ" << "\n";
        outs() << "\n";
    }

    return false;
}

INITIALIZE_PASS(AArch64MachineBBloc, "AArch64-machineBB-loc",
    AArch64_MACHINEBB_LOC_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createAArch64MachineBBlocPass() { return new AArch64MachineBBloc(); }

}