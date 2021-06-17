#include "PPC.h"
#include "PPCInstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include <string>
// #include <map>
#include <list>

using namespace llvm;

#define PPC_MACHINEBB_EDGE_PASS_NAME "Dummy PPC machineBB edge pass"

namespace {

class PPCMachineBBedge : public MachineFunctionPass {
public:
    static char ID;

    PPCMachineBBedge() : MachineFunctionPass(ID) { }
    // initializePPCMachineBBPrinterPass(*PassRegistry::getPassRegistry());

    bool runOnMachineFunction(MachineFunction &MF) override;
    // bool doFinalization(Module &M) override;

    StringRef getPassName() const override { return PPC_MACHINEBB_EDGE_PASS_NAME; }

    // std::map<std::string, std::set<std::string>> BB2MBBs;
};


} // end of anonymous namespace

char PPCMachineBBedge::ID = 0;

bool PPCMachineBBedge::runOnMachineFunction(MachineFunction &MF) {

    unsigned MFID = MF.getFunctionNumber();
    outs() << "---edgePass---\n";
    outs() << MF.getName() << "\n";
    for (auto &MBB : MF) {
        std::string MBBID = std::to_string(MFID) + "_" + std::to_string(MBB.getNumber());
        outs() << "MachineBasicBlock:";
        outs() << MBBID << ",";
        for (MachineBasicBlock *succ : MBB.successors()){
            std::string succID = std::to_string(succ->getNumber());
            outs() << "succ: " <<succID << " ";
        }
        outs() << "\n";
    }

    return false;
}

// bool PPCMachineBBedge::doFinalization(Module &M) {
//     // print the result of BB-MBB mapping by MBB::getBB()
//     outs() << "Total BB size is " << BB2MBBs.size() << "\n";

//     for(auto BBiter = BB2MBBs.begin(); BBiter != BB2MBBs.end(); BBiter++) {
//         std::string BBID = BBiter->first;
//         std::set<std::string> MBBs = BBiter->second;
//         outs() << BBID << " has " << MBBs.size() << " MBBs : [";
//         for(auto MBBiter = MBBs.begin(); MBBiter != MBBs.end(); MBBiter++) {
//             std::string MBBID = *MBBiter;
//             outs() << MBBID << ",";
//         }
//         outs() << "]\n";
//     }

//     return false;
// }


INITIALIZE_PASS(PPCMachineBBedge, "PPC-machineBB-edge",
    PPC_MACHINEBB_EDGE_PASS_NAME,
    true, // is CFG only?
    true  // is analysis?
)

namespace llvm {

FunctionPass *createPPCMachineBBedgePass() { return new PPCMachineBBedge(); }

}