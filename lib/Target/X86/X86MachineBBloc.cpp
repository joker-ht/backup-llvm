#include "X86.h"
#include "X86InstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <llvm/IR/DebugInfo.h>
#include "llvm/CodeGen/MachineInstr.h"
#include <string>
#include <list>
#include <tuple>

using namespace std;
using namespace llvm;

#define X86_MACHINEBB_LOC_PASS_NAME "Dummy X86 machineBB loc pass"

typedef list<tuple<string, unsigned, unsigned>> DebugInfoList;

namespace {

class X86MachineBBloc : public MachineFunctionPass {
public:
    static char ID;

    X86MachineBBloc() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return X86_MACHINEBB_LOC_PASS_NAME; }

    // string get_tree_srcline(const llvm::DebugLoc *debugInfo);
    
};

} // end of anonymous namespace

char X86MachineBBloc::ID = 0;

static string get_tree_srcline(const llvm::DebugLoc *debugInfo){
    
    auto *Scope = cast<DIScope>(debugInfo->getScope());
    string filename = (*Scope).getFilename();
    unsigned linenub = debugInfo->getLine();
    unsigned colnub = debugInfo->getCol();

    string res = filename + ":" + to_string(linenub) + ":" + to_string(colnub);

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
        outs() << to_string(MFID) + "_" + to_string(pred->getNumber()) << ",";
    }
    outs() << " pred_nub:" << count << "\n";
    return count;
}

static unsigned PrintBBSucc(MachineBasicBlock &MBB,signed MFID){
    outs() << "    succ:";
    int count = 0;
    for (auto *succ : MBB.successors()){
        count++;
        outs() << to_string(MFID) + "_" + to_string(succ->getNumber()) << ",";
    }
    outs() << " succ_nub:" << count << "\n";
    return count;
}

static string getOpcodeName(MachineInstr &instr){
    const MachineFunction *MF = getMFIfAvailable(instr);
    const TargetInstrInfo *TII = MF->getSubtarget().getInstrInfo();
    if (TII){
        string opcodename = TII->getName(instr.getOpcode());
        return opcodename;
    }
    return "";
}

static bool isTrueCompare(MachineInstr &instr){
    string opcodename = getOpcodeName(instr);
    if (opcodename != ""){
        if ((opcodename.find("CMP") != string::npos) 
            || (opcodename.find("TEST") != string::npos))
            return true;
        if ((opcodename.find("SUB") != string::npos)
            || (opcodename.find("AND") != string::npos)){
            // if [sub,and] instr followed by [jmo,mov,set],it's a cmp
            MachineInstr *tmp = &instr;
            while(tmp->getNextNode()){
                tmp = tmp->getNextNode();
                if (tmp->isDebugInstr() || tmp->isCFIInstruction())
                    continue;
                opcodename = getOpcodeName(*tmp);
                if ((opcodename.find("CMOV") != string::npos)
                    || (opcodename.find("JCC_1") != string::npos)
                    || (opcodename.find("SET") != string::npos))
                    return true;
                if (opcodename.find("MOV") == string::npos)
                    return false;
            }
        }    
    }
    
    return false;
}

bool X86MachineBBloc::runOnMachineFunction(MachineFunction &MF) {
    string funcname = MF.getName();
    outs() << "FuncName:" << funcname << "\n";
    unsigned MFID = MF.getFunctionNumber();
    for (auto &MBB : MF) {

        string MBBID = to_string(MFID) + "_" + to_string(MBB.getNumber());
        outs() << "    FUNC ID and BB ID:" << MBBID << "\n";

        PrintBBPred(MBB,MFID);
        // refr to succnub to verify if BB has cmp
        signed succnub = PrintBBSucc(MBB,MFID);
        signed cmpnub = 0;
        for (auto &instr : MBB){
            if (instr.isDebugInstr() || instr.isCFIInstruction())
                continue;
            outs() << "        " <<instr;

            const MachineFunction *MF = getMFIfAvailable(instr);
            const TargetInstrInfo *TII = MF->getSubtarget().getInstrInfo();
            string opcodename = TII->getName(instr.getOpcode());
            outs() << "        " << opcodename;

            if (isTrueCompare(instr)){
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
                // string filename = (*Scope).getFilename();
                // string dbginfo = filename + ":" + to_string(linenub) + ":" + to_string(columnnub);

                // get nested inline srcline
                string dbginfo = get_tree_srcline(&debugInfo);
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

INITIALIZE_PASS(X86MachineBBloc, "X86-machineBB-loc",
    X86_MACHINEBB_LOC_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createX86MachineBBlocPass() { return new X86MachineBBloc(); }

}