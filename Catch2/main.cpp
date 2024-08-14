#include "pch.h"

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)    // NOLINT(clang-diagnostic-unused-macros)
// ReSharper disable once CppInconsistentNaming
#define new DEBUG_NEW  // NOLINT(clang-diagnostic-keyword-macro)
#endif

static Catch::LeakDetector leakDetector;

int main()
{
    // We want to force the linker not to discard the global variable
        // and its constructor, as it (optionally) registers leak detector
    (void)&leakDetector;

    int numArgs = 0;
    const LPWSTR* pArgs = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    const int result = Catch::Session().run(numArgs, pArgs);

    return result;
}
