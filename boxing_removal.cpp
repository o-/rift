#include <iostream>
#include <ciso646>



#include "boxing_removal.h"
#include "compiler.h"


using namespace llvm;

namespace rift {
    char BoxingRemoval::ID = 0;

    bool BoxingRemoval::runOnFunction(llvm::Function & f) {
        //std::cout << "running boxing removal optimization..." << std::endl;
        bool changed = false;
        while (true) {
            bool c = false;
            for (auto & b : f) {
                auto i = b.begin();
                while (i != b.end()) {
                    bool erase = false;
                    if (CallInst * ci = dyn_cast<CallInst>(i)) {
                        StringRef s = ci->getCalledFunction()->getName();
                        if (s != "call" and s != "genericEval" and s != "characterEval" and s != "envGet" and s != "envSet") {
                            if (ci->use_empty())
                                erase = true;
                        }
                    }
                    if (erase) {
                        llvm::Instruction * v = i;
                        ++i;
                        v->eraseFromParent();
                        c = true;
                    } else {
                        ++i;
                    }
                }
            }
            changed = changed or c;
            if (not c)
                break;
        }
        //f.dump();
        return changed;
    }


} // namespace rift