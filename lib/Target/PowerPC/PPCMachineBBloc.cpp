#include "PPC.h"
#include "PPCInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <llvm/IR/DebugInfo.h>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include "llvm/CodeGen/MachineInstr.h"

using namespace std;
using namespace llvm;

#define PPC_MACHINEBB_LOC_PASS_NAME "Dummy PPC machineBB loc pass"

namespace {

class PPCMachineBBloc : public MachineFunctionPass {
public:
    static char ID;

    PPCMachineBBloc() : MachineFunctionPass(ID) { }

    bool runOnMachineFunction(MachineFunction &MF) override;

    StringRef getPassName() const override { return PPC_MACHINEBB_LOC_PASS_NAME; }

    // string get_tree_srcline(const llvm::DebugLoc *debugInfo);
    
};

} // end of anonymous namespace

char PPCMachineBBloc::ID = 0;

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

static bool setOperandList(MachineInstr &instr, vector<string> &operandlist){
    signed operandno = instr.getNumOperands();
    if (operandno){
        for(int i=0;i<operandno;i++){
            string operandname = instr.getOperand(i).getOperandName();
            operandlist.push_back(operandname);
        }
        return true;
    }
    return false;
}

static bool isTrueCompre(MachineInstr &instr){
    if(instr.isCompare()) return true;

    vector<string> operandlist;
    if (setOperandList(instr,operandlist)){
        for(auto operand:operandlist){
            // if operandlist contains cr 
            string opcodename = getOpcodeName(instr);
            if (opcodename.find("BC")==string::npos){
                if (operand.find("cr") != string::npos){
                    // MachineInstr *tmp = &instr;
                    // while(tmp->getNextNode()){
                    // tmp = tmp->getNextNode();
                    // if (tmp->isDebugInstr() || tmp->isCFIInstruction())
                    //     continue;
                    // }
                    return true;
                }
            }
        }
    }

    return false;
}

bool PPCMachineBBloc::runOnMachineFunction(MachineFunction &MF) {
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
            // if (instr.isDebugInstr() || instr.isCFIInstruction())
            //     continue;
            // outs() << "        " <<instr;
            string opcodename = getOpcodeName(instr);
            outs() << "        " << opcodename;
            int len = instr.getNumOperands();
            // outs() << " " << to_string(len);
            if (len){
                for(int i=0;i<len;i++){
                    outs() << ", " << instr.getOperand(i);
                }
            }
            

            if (isTrueCompre(instr)){
                outs() << " is cmp";
                cmpnub += 1;
            }
            outs() << "\n";
            
            if (instr.getDebugLoc()){
                const llvm::DebugLoc debugInfo = instr.getDebugLoc();
                string dbginfo = get_tree_srcline(&debugInfo);
                outs() << "          " << dbginfo << "\n";
            } 
            // outs() << "\n";
        }
        // 
        if (cmpnub==0 && succnub==2) outs() << "    may switch or miss cmp instr" << "\n";
        if (cmpnub > 1) outs() << "    this BB contains more than one cmp:" << cmpnub << "\n";
        if (cmpnub >=1 && succnub <2) outs() << "    this BB contains cmp but has less than 2 succ" << "\n";
        outs() << "\n";
    }

    return false;
}

INITIALIZE_PASS(PPCMachineBBloc, "PPC-machineBB-loc",
    PPC_MACHINEBB_LOC_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)
 
namespace llvm {

FunctionPass *createPPCMachineBBlocPass() { return new PPCMachineBBloc(); }

}