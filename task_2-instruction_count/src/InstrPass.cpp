#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.def"

using namespace llvm;

namespace {


void VisitFunction(Function &Func) {
    outs() << "Function " << Func.getName() << "():" << "\n";

    std::unordered_map<std::string, size_t> opNames;
    for (auto &BB : Func) {
        for (auto &I : BB) {
            std::string instrName = I.getOpcodeName();
            if (opNames.find(instrName) == opNames.end()) {
                opNames[instrName] = 1;
            } else {
                opNames[instrName] += 1;
            }
        }
    }
    for (auto &[name, count] : opNames) {
        outs() << "\t" << name << ": " << count << "\n";
    }
    outs() << "\n";
}


struct InstrPass : PassInfoMixin<InstrPass>{
    PreservedAnalyses run(Function &Function, FunctionAnalysisManager &AnalysisManager){
        VisitFunction( Function);
        return (PreservedAnalyses::all());
    }
    static bool isRequired() {
        return (true);
    }
};
}

bool CallBackForPipelineParser(StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>){
    if (Name == "InstrPass"){
        FPM.addPass( InstrPass());
	    return (true);
    } else {
        return (false);
    }
}
void CallBackForPassBuilder(PassBuilder &PB) {
    PB.registerPipelineParsingCallback( &CallBackForPipelineParser);
}

PassPluginLibraryInfo getRpoPassPluginInfo(){
    uint32_t     APIversion =  LLVM_PLUGIN_API_VERSION;
    const char * PluginName =  "InstrPass";
    const char * PluginVersion =  LLVM_VERSION_STRING;

    PassPluginLibraryInfo info =
    {
        APIversion,
	    PluginName,
	    PluginVersion,
	    CallBackForPassBuilder
    };

    return (info);
}


extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return (getRpoPassPluginInfo());
}
