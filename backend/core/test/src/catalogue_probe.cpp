// Numerical catalogue probe: exploratory evidence, not a proof of correctness.
#include "lib.hpp"
#include <memory>
int main(int argc, char** argv) {
    if (argc < 2) return 2;
    const std::string name=argv[1];
    const double x=argc>2 ? std::stod(argv[2]) : .2;
    const unsigned count=argc>3 ? std::stoul(argv[3]) : 256;
    using namespace shanks::series;
    std::unique_ptr<series_base<double,unsigned>> series;
    try {
#define SERIES_ENTRY(id, title) if(name==#id) series=std::make_unique<id##_iterator<double,unsigned>>(x);
#define SERIES_LAST(id, title) SERIES_ENTRY(id,title)
#define SERIES_ENTRY_ARGS(id, title, real_arg, int_arg) \
    if(name==#id) { \
        if constexpr (std::is_constructible_v<id##_iterator<double,unsigned>,double,double>) \
            series=std::make_unique<id##_iterator<double,unsigned>>(x,double{.5}); \
        else series=std::make_unique<id##_iterator<double,unsigned>>(x,unsigned{32}); \
    }
#include "series_registry.def"
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
        if (!series) return 3;
        const auto data=series->generate(count);
        double reference=std::numeric_limits<double>::quiet_NaN();
        try { reference=series->get_sum(); } catch(const std::domain_error&) {}
        std::cout << std::setprecision(17) << reference << '\t' << data.Sn.back();
        for(unsigned n=0;n<std::min(count,8u);++n) std::cout << '\t' << data.an[n];
        std::cout << '\n';
    } catch(const std::exception& e) { std::cerr << e.what(); return 1; }
}
