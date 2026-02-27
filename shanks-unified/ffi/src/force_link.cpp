#include <gsl/gsl_cblas.h>

namespace shanks::ffi::bridge {
// This function is never called, but its existence forces the linker
// to include a dependency on libgslcblas because it uses a symbol from it.
void shanks_force_link_gslcblas() {
    // We only need to reference a symbol from libgslcblas to force the linker to include it.
    // Taking the address is enough; we don't need to actually call it and risk
    // runtime parameter validation errors from GSL.
    auto volatile p = &cblas_ctrmv;
    (void)p;
}
}
