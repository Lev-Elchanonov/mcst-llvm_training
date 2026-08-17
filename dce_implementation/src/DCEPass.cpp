#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/Instruction.def"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Value.h"

using namespace llvm;

namespace {
    bool CanRemoveCall(CallInst *CInst){
        // если результат используется, то удалять нелья
        if (CInst->getNumUses() != 0){
            return false;
        }
        Function *Callee = CInst->getCalledFunction();
        if (!Callee){
            return false;
        }
        // проверка что функция не может выкинуть исключения
        if (!Callee->hasFnAttribute(Attribute::NoUnwind)){
            return false;
        }
        if (Callee->hasFnAttribute(Attribute::ReadNone) ||
            Callee->hasFnAttribute(Attribute::ReadOnly)) {

            return true;
        }
        return false;
    }
    bool EliminateDeadCode(Function &Func) {
        bool Changed = false;

        std::unordered_map<Value*, size_t> UseCount;
        std::vector<Instruction*> DeadInstructions;
        for (auto &BB : Func) {
            for (auto &I : BB) {
                UseCount[&I] = I.getNumUses();
            }
        }
        for (auto &BB : Func) {
            for (auto &I : BB) {
                // пропускаем инструкции терминаторы
                if (I.isTerminator()) {
                    continue;
                }
                if (auto *CInst = dyn_cast<CallInst>(&I)) {
                    if (CanRemoveCall(CInst)){
                        DeadInstructions.push_back(CInst);
                    }
                }
                // если есть побочные эффекты
                if (I.mayHaveSideEffects()){
                    continue;
                }
                // если нет использований, то помещаем в вектор для удаления
                if (UseCount[&I] == 0) {
                    DeadInstructions.push_back(&I);
                }
            }
        }
        for (auto *I : DeadInstructions) {
            I->eraseFromParent();
            Changed = true;
        }
        return Changed;
    }


    struct DCEPass : PassInfoMixin<DCEPass>{
        PreservedAnalyses run(Function &Function, FunctionAnalysisManager &AnalysisManager){
            outs() << "Function " << Function.getName() << "():" << "\n";
            size_t StartAmount = Function.getInstructionCount();
            outs() << "Before: " << StartAmount << "\n";
            bool CodeChanged = false;

            /**
             * Алгоритм выполняется пока есть удаленные инструкции, потому что
             * удаление мертвых инструкций может сделать другие инструкции мертвыми
             */
            while (EliminateDeadCode(Function)) {
                CodeChanged = true;
            }
            size_t EndAmount = Function.getInstructionCount();
            outs() << "After: " << EndAmount << "\n";
            auto Difference = StartAmount - EndAmount;
            outs() << "Deleted instructions count: " << Difference << "\n\n";
            if (CodeChanged) {
                // Код изменен, а значит анализы нужно пересчитать
                return PreservedAnalyses::none();
            }
            return PreservedAnalyses::all();
        }
        static bool isRequired() { return (true); }
    }   ;
}

bool CallBackForPipelineParser(StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>){
    if (Name == "DCEPass"){
        FPM.addPass(DCEPass());
        return (true);
    } else {
        return (false);
    }
}
void CallBackForPassBuilder(PassBuilder &PB) {
    PB.registerPipelineParsingCallback( &CallBackForPipelineParser);
}

PassPluginLibraryInfo getDCEPassPluginInfo(){
    uint32_t     APIversion =  LLVM_PLUGIN_API_VERSION;
    const char * PluginName =  "DCEPass";
    const char * PluginVersion =  LLVM_VERSION_STRING;

    PassPluginLibraryInfo Info = {
        APIversion,
        PluginName,
        PluginVersion,
        CallBackForPassBuilder
    };

    return (Info);
}


extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return (getDCEPassPluginInfo());
}
