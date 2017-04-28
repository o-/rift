#include "gc.h"
#include "runtime.h"

namespace gc {

void GarbageCollector::visitChildren(RVal* val) {
    switch (val->type) {
        case Type::Environment: {
            Environment* env = (Environment*)val;
            if (env->bindings)
                mark(env->bindings);
            if (env->parent)
                mark(env->parent);
            break;
        }

        case Type::Bindings: {
            Bindings* env = (Bindings*)val;
            for (unsigned i = 0; i < env->size; ++i) {
                mark(env->binding[i].value);
            }
            break;
        }


        case Type::Function: {
            RFun* fun = (RFun*)val;
            if (fun->args)
                mark(fun->args);
            if (fun->env)
                mark(fun->env);
            break;
        }

        case Type::FunctionArgs:
        case Type::Double:
        case Type::Character:
            // leaf nodes
            break;

        default:
            assert(false && "Broken RVal");
    }
}

// The core mark & sweep algorithm
void GarbageCollector::doGc() {
#ifdef GC_DEBUG
    unsigned memUsage = size() - free();
    verify();
#endif

    mark();

#ifdef GC_DEBUG
    verify();
#endif

    sweep();

#ifdef GC_DEBUG
    verify();
    unsigned memUsage2 = size() - free();
    assert(memUsage2 <= memUsage);
    std::cout << "reclaiming " << memUsage - memUsage2
    << "b, used " << memUsage2 << "b, total " << size() << "b\n";
#endif
}

void GarbageCollector::scanStackWrapper() {
    // Clobber all registers:
    // -> forces all variables currently hold in registers to be spilled
    //    to the stack where our stackScan can find them.
    __asm__ __volatile__("nop" : :
        : "%rax", "%rbx", "%rcx", "%rdx", "%rsi", "%rdi",
        "%r8", "%r9", "%r10", "%r11", "%r12",
        "%r13", "%r14", "%r15");
    scanStack();
}

// The stack scan traverses the memory of the C stack and looks at every
// possible stack slot. If we find a valid heap pointer we mark the
// object as well as all objects reachable through it as live.
void __attribute__((noinline)) GarbageCollector::scanStack() {
    void ** p = (void**)__builtin_frame_address(0);

    while (p < BOTTOM_OF_STACK) {
        if ((uintptr_t)p > 1024)
            markMaybe(*p);
        p++;
    }
}


void GarbageCollector::mark() {
    // TODO: maybe some mechanism to define static roots?
    scanStack();
}


}
