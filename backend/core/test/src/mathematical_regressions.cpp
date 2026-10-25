/** Non-interactive mathematical regressions; build instructions in docs/verification.md.
 * References: https://dlmf.nist.gov/3.9, /4.6, /4.19, /4.24.
 * Uses the actual production headers, double arithmetic and explicit failures
 * (not assert, so checks remain active with -DNDEBUG).
 */
#include "lib.hpp"

static void near(double got, double expected, const char* label) {
    if (!std::isfinite(got) || std::abs(got - expected) > 2e-10 * (1 + std::abs(expected)))
        throw std::runtime_error(std::string(label) + ": got " + std::to_string(got));
}

template<class Exception, class F> void raises(F operation) {
    try { operation(); } catch (const Exception&) { return; }
    throw std::runtime_error("Expected exception was not raised");
}

template<template<class, class> class Series>
void terms(const char* name, double x, bool emit) {
    Series<double, unsigned> series(x);
    auto data = series.generate(32);
    double total = 0;
    for (unsigned n = 0; n < 32; ++n) {
        total += data.an[n];
        near(data.Sn[n], total, "partial sum indexing");
        if (emit && n < 8) std::cout << name << '\t' << x << '\t' << n << '\t' << data.an[n] << '\n';
    }
    near(total, series.get_sum(), name);
    if (!series.generate(0).Sn.empty()) throw std::runtime_error("generate(0)");
}

template<class S>
void extension_terms(S& series,const char* name,double x,bool emit,bool fast=true) {
    auto data=series.generate(32);
    if(fast) near(data.Sn.back(),series.get_sum(),name);
    if(emit) for(unsigned n=0;n<8;++n)
        std::cout << name << '\t' << x << '\t' << n << '\t' << data.an[n] << '\n';
}

void extensions(bool emit) {
    using namespace shanks::series;
#define EXT(id) { id##_iterator<double,unsigned> series(.2); extension_terms(series,#id,.2,emit); }
    EXT(four_arctan)
    EXT(arcsin_x_minus_x)
    EXT(half_asin_two_x)
    EXT(two_arcsin_square_x_halfed)
    EXT(cos_sqrt_x)
    EXT(one_div_sqrt2_sin_xdivsqrt2)
    EXT(mean_sinh_sin)
    EXT(two_degree_x)
    EXT(ln_1plussqrt1plusxsquare_minus_ln_2)
    EXT(sqrt_oneminussqrtoneminusx_div_x)
    EXT(erf)
    EXT(exp_squared_erf)
    EXT(si_x)
    EXT(ci_x)
#undef EXT
    bin_iterator<double,unsigned> bin(.2,.5);
    extension_terms(bin,"bin",.2,emit);
    m_fact_1mx_mp1_inverse_iterator<double,unsigned> derivative(.2,2u);
    extension_terms(derivative,"m_fact_1mx_mp1_inverse",.2,emit);
    ln2_iterator<double,unsigned> log_two(.2);
    extension_terms(log_two,"ln2",.2,emit,false);
    two_ln2_iterator<double,unsigned> twice_log(.2);
    extension_terms(twice_log,"two_ln2",.2,emit,false);
    euler_mascheroni_iterator<double,unsigned> euler(.2);
    extension_terms(euler,"euler_mascheroni",.2,emit,false);
    m_fact_1mx_mp1_inverse_iterator<double,unsigned> overflow_test(.1,13u);
    near(overflow_test.generate(1).an[0],6227020800.0,"13! overflows a 32-bit index");
    near(overflow_test.get_sum(),6227020800.0/std::pow(.9,14),"Derivative analytic sum");
    half_asin_two_x_iterator<double,unsigned> asin(.2);
    near(asin.generate(3).an[2],1.2*std::pow(.2,5),"Half arcsine x^5 coefficient");
}

void algorithm_extensions(bool emit) {
    using namespace shanks::algos;
    using R=shanks::remainders::remainder_type;
    series_result<double> data;
    for(unsigned n=0;n<20;++n) {
        const double x=n+1;
        data.Sn.push_back((7*x*x+3*x+5)/(x*x+2*x+9));
        data.an.push_back(n ? data.Sn[n]-data.Sn[n-1] : data.Sn[0]);
    }
    wynn_rho_algorithm<double,unsigned> rho;
    brezinski_theta_algorithm<double,unsigned> theta;
    for(unsigned order : {1u,2u}) for(unsigned n : {0u,1u,2u}) {
        const double r=rho(n,2*order,data);
        const double t=theta(n,2*order,data);
        if(order==2) near(r,7,"Rho exact rational kernel");
        if(emit) {
            std::cout << "wynn_rho\t" << n << '\t' << order << "\tnext\t" << r << '\n';
            std::cout << "brezinski_theta\t" << n << '\t' << order << "\tnext\t" << t << '\n';
        }
    }
    const std::array<std::pair<const char*,R>,5> variants={{{"t",R::t_type},
        {"next",R::t_wave_type},{"u",R::u_type},{"v",R::v_type},{"v_shift",R::v_wave_type}}};
    for(const auto& variant : variants) {
        drummond_d_algorithm<double,unsigned> direct(variant.second,false),recursive(variant.second,true);
        for(unsigned order : {1u,2u}) for(unsigned n : {0u,1u,2u}) {
            const double value=direct(n,order,data);
            near(value,recursive(n,order,data),"Drummond direct equals recursive");
            if(emit) std::cout << "drummond\t" << n << '\t' << order << '\t' << variant.first << '\t' << value << '\n';
        }
    }
    series_result<double> empty;
    raises<std::out_of_range>([&] { rho(0,2,empty); });
    raises<std::out_of_range>([&] { theta(0,2,empty); });
    raises<std::domain_error>([&] { rho(0,1,data); });
    raises<std::domain_error>([&] { theta(0,1,data); });
    if(rho.how_much(0,2)!=3) throw std::runtime_error("Rho input count");
    wynn_rho_algorithm<double,unsigned> generalized(shanks::numerators::generalized_type,-1,1);
    near(generalized(0,4,data),rho(0,4,data),"Generalized gamma=-1 equals classical rho");
    data.an.clear();
    near(rho(0,4,data),7,"Classic rho does not use series terms as nodes");

}

template<template<class,class> class Series>
void high_index_rational(long double expected) {
    Series<double,unsigned> series(.2);
    double state=0;
    const double value=series.next(1000000u,state);
    if(!std::isfinite(value) || std::abs(value/static_cast<double>(expected)-1)>1e-12)
        throw std::runtime_error("Rational index arithmetic overflow");
}

void third_extensions(bool emit) {
    using namespace shanks::series;
    using namespace shanks::algos;
    using R=shanks::remainders::remainder_type;
    const long double k=1000001;
    high_index_rational<eighth_pi_m_one_third_iterator>(.2L/((2*k-1)*(2*k+1)*(2*k+3)));
    high_index_rational<five_pi_twelve_iterator>(1.L/(3*(2*k-1)));
    high_index_rational<four_ln2_m_3_iterator>(-.2L/(k*k*(k+1)*(k+1)));
    high_index_rational<minus_one_quarter_iterator>(-.2L/(k*(k+2)));
    high_index_rational<minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator>(.2L/(k*(36*k*k-1)));
    high_index_rational<one_iterator>(.2L/(k*(k+1)));
    high_index_rational<one_third_pi_squared_m_nine_iterator>(.2L/(k*k*(k+1)*(k+1)));
    high_index_rational<one_twelfth_iterator>(.2L/((2*k-1)*(2*k+1)*(2*k+3)));
    high_index_rational<pi_3_iterator>(.2L/(k*(2*k-1)*(4*k-3)));
    high_index_rational<pi_4_iterator>(.2L/(2*k-1));
    high_index_rational<pi_cubed_32_iterator>(.2L/((2*k-1)*(2*k-1)*(2*k-1)));
    high_index_rational<pi_four_minus_ln2_halfed_iterator>(.2L/((2*k-1)*(2*k)));
    high_index_rational<pi_six_min_half_iterator>(.2L/((6*k-1)*(6*k+1)));
    high_index_rational<pi_squared_6_minus_one_iterator>(.2L/(k*k*(k+1)));
    high_index_rational<pi_squared_twelve_iterator>(.2L/(k*k));
    high_index_rational<three_minus_pi_iterator>(-.2L/(k*(k+1)*(2*k+1)));
    high_index_rational<x_two_iterator>(.2L/((2*k-1)*(2*k+1)));
    high_index_rational<x_two_throught_squares_iterator>((2*k*k-1)*.2L/(4*k*k*k*k+1));
    three_minus_pi_iterator<double,unsigned> three(.2);
    near(three.get_sum(),.2*(3-std::numbers::pi),"Pi must not be cast to int");
    raises<std::invalid_argument>([] { bessel_first_kind_iterator<double,unsigned> series(-.2,.5); });
    raises<std::invalid_argument>([] { bessel_first_kind_iterator<double,unsigned> series(.2,-.5); });
    raises<std::invalid_argument>([] { incomplete_Gamma_func_iterator<double,unsigned> series(.2,0.0); });
    raises<std::invalid_argument>([] { incomplete_Gamma_func_iterator<double,unsigned> series(-.2,.5); });
    ln_sinx_minus_ln_x_iterator<double,unsigned> sinc(0);
    near(sinc.get_sum(),0,"Log-sinc removable singularity");
    near(sinc.generate(3).Sn.back(),0,"Log-sinc zero terms");
    const auto saved=mpfr::mpreal::get_default_prec();
    mpfr::mpreal::set_default_prec(200);
    three_minus_pi_iterator<mpfr::mpreal,unsigned> high_three(mpfr::mpreal(1));
    if(mpfr::abs(high_three.get_sum()-(3-mpfr::const_pi()))>mpfr::mpreal("1e-50"))
        throw std::runtime_error("three_minus_pi high precision constant");
    series_result<mpfr::mpreal> high_data;
    mpfr::mpreal high_term=1,high_sum=0;
    for(unsigned j=0;j<60;++j) {
        high_sum+=high_term; high_data.an.push_back(high_term); high_data.Sn.push_back(high_sum);
        high_term*=mpfr::mpreal("-.5");
    }
    for(bool recursive : {false,true}) {
        levin_algorithm<mpfr::mpreal,unsigned> high_levin(R::t_wave_type,recursive,mpfr::mpreal("2.5"));
        levin_sidi_s_algorithm<mpfr::mpreal,unsigned> high_sidi(R::t_wave_type,recursive,mpfr::mpreal("2.5"));
        const mpfr::mpreal limit=mpfr::mpreal(2)/3;
        if(mpfr::abs(high_levin(0,40,high_data)-limit)>mpfr::mpreal("1e-50") ||
           mpfr::abs(high_sidi(0,40,high_data)-limit)>mpfr::mpreal("1e-50"))
            throw std::runtime_error("High-order Levin/S binomial arithmetic");
    }
    mpfr::mpreal::set_default_prec(saved);
    series_result<double> data;
    double total=0;
    for(unsigned j=0;j<40;++j) {
        const double term=(j%2 ? -1.0 : 1.0)/(j+1);
        data.an.push_back(term); total+=term; data.Sn.push_back(total);
    }
    const std::array<std::pair<const char*,R>,5> variants={{{"t",R::t_type},
        {"next",R::t_wave_type},{"u",R::u_type},{"v",R::v_type},{"v_shift",R::v_wave_type}}};
    for(double beta : {1.0,2.5}) for(const auto& variant : variants)
        for(unsigned order : {1u,2u,3u,4u}) for(unsigned n : {0u,1u,2u}) {
            levin_algorithm<double,unsigned> levin(variant.second,false,beta),levin_rec(variant.second,true,beta);
            levin_sidi_s_algorithm<double,unsigned> sidi(variant.second,false,beta),sidi_rec(variant.second,true,beta);
            const double lv=levin(n,order,data),sv=sidi(n,order,data);
            near(lv,levin_rec(n,order,data),"Levin direct equals recursive");
            near(sv,sidi_rec(n,order,data),"Sidi-S direct equals recursive");
            if(emit) {
                std::cout << "levin\t" << n << '\t' << order << '\t' << variant.first << '\t' << beta << '\t' << lv << '\n';
                std::cout << "weniger\t" << n << '\t' << order << '\t' << variant.first << '\t' << beta << '\t' << sv << '\n';
            }
        }
    weniger_algorithm<double,unsigned> weniger;
    for(unsigned order : {1u,2u,3u,4u}) for(unsigned n : {0u,1u,2u}) {
        levin_sidi_s_algorithm<double,unsigned> sidi(R::t_wave_type,false,1.0);
        near(weniger(n,order,data),sidi(n,order,data),"Weniger next equals Sidi-S next");
        if(emit) std::cout << "weniger\t" << n << '\t' << order << "\tnext\t1\t" << weniger(n,order,data) << '\n';
    }
    overholt_algorithm<double,unsigned> overholt;
    lubkin_w_algorithm<double,unsigned> lubkin;
    for(unsigned order : {0u,1u,2u,3u}) for(unsigned n : {0u,1u,2u}) {
        const double value=overholt(n,order,data);
        if(emit) std::cout << "overholt\t" << n << '\t' << order << "\tnext\t1\t" << value << '\n';
    }
    for(unsigned order : {0u,1u,2u}) for(unsigned n : {0u,1u,2u}) {
        const double value=lubkin(n,order,data);
        if(emit) std::cout << "lubkin_w\t" << n << '\t' << order << "\tnext\t1\t" << value << '\n';
    }
    series_result<double> empty,short_data;
    short_data.Sn={1,.5,5.0/6}; short_data.an={1,-.5,1.0/3};
    raises<std::out_of_range>([&] { weniger(0,2,short_data); });
    raises<std::out_of_range>([&] { weniger(0,1,empty); });
    raises<std::out_of_range>([&] { lubkin(0,1,empty); });
    if(overholt.how_much(0,3)!=5) throw std::runtime_error("Overholt input count");
}

void fourth_sources() {
    using namespace shanks::series;
    natural_series_iterator<double,unsigned> natural(0);
    near(natural.generate(100).Sn.back(),4950,"natural partial sums");
    raises<std::domain_error>([&]{ natural.get_sum(); });
    near(natural.get_regularized_sum(),-1.0/12,"separate zeta regularization");
    rump_seq1_iterator<double,unsigned> r1(0);
    rump_seq2_iterator<double,unsigned> r2(0);
    rump_seq4_iterator<double,unsigned> r4(0);
    raises<std::domain_error>([&]{ r1.get_sum(); });
    raises<std::domain_error>([&]{ r2.get_sum(); });
    raises<std::domain_error>([&]{ r4.get_sum(); });
    rump_seq5_iterator<double,unsigned> r5(0),perturbed(.01);
    near(r5.get_sum(),5,"Rump exact arithmetic limit");
    raises<std::domain_error>([&]{ perturbed.get_sum(); });
    const auto rdata=r5.generate(5);
    for(unsigned i=0;i<5;++i) near(rdata.Sn[i],rdata.an[i],"sequence is not resummed");
    integral1_iterator<double,unsigned> integral(0,4u,16);
    const auto q=integral.generate(4);
    for(unsigned n=0;n<4;++n) {
        const unsigned panels=4*(n+1);
        double expected=0;
        for(unsigned j=1;j<panels;++j) {
            const double u=double(j)/panels;
            expected+=4*u*std::log(u)/(1+u*u)/panels;
        }
        near(q.Sn[n],expected,"complete trapezoid quadrature");
        near(q.an[n],q.Sn[n],"quadrature sequence semantics");
    }
    raises<std::out_of_range>([&]{ integral.generate(5); });
    raises<std::invalid_argument>([]{ integral1_iterator<double,unsigned> bad(0,0u); });
    const auto saved=mpfr::mpreal::get_default_prec();
    mpfr::mpreal::set_default_prec(200);
    // Independent integral quadratures at 110 decimal digits (see Python tests).
    minus_one_n_fact_n_in_n_iterator<mpfr::mpreal,unsigned> factorial(mpfr::mpreal(1));
    minus_one_ned_in_n_iterator<mpfr::mpreal,unsigned> dream(mpfr::mpreal(1));
    const auto f=factorial.generate(180).Sn.back();
    const auto d=dream.generate(80).Sn.back();
    if(mpfr::abs(factorial.get_sum()-f)>mpfr::mpreal("1e-50") ||
       mpfr::abs(dream.get_sum()-d)>mpfr::mpreal("1e-50"))
        throw std::runtime_error("Rapid references must retain MPFR precision");
    mpfr::mpreal::set_default_prec(saved);
}

void fifth_algorithms(bool emit) {
    using namespace shanks::algos;
    using R=shanks::remainders::remainder_type;
    series_result<double> data;
    double total=0;
    for(unsigned j=0;j<40;++j) {
        data.an.push_back((j%2 ? -1.0:1.0)/(j+1));
        total+=data.an.back();data.Sn.push_back(total);
    }
    const std::array<std::pair<const char*,R>,5> variants={{{"t",R::t_type},
        {"next",R::t_wave_type},{"u",R::u_type},{"v",R::v_type},{"v_shift",R::v_wave_type}}};
    for(double gamma : {4.5,7.0,100.5})for(const auto& variant:variants)
        for(unsigned k=0;k<=3;++k)for(unsigned n=0;n<=3;++n) {
            levin_sidi_m_algorithm<double,unsigned> algorithm(variant.second,gamma);
            if(emit)std::cout<<"levin_sidi_m\t"<<n<<'\t'<<k<<'\t'<<variant.first<<'\t'<<gamma<<'\t'<<algorithm(n,k,data)<<'\n';
        }
    wynn_epsilon_2_algorithm<double,unsigned> epsilon;
    wynn_epsilon_1_algorithm<double,unsigned> original;
    shanks_transform_alternating<double,unsigned> aitken;
    for(unsigned k=0;k<=5;++k)for(unsigned n=0;n<=3;++n) {
        near(epsilon(n,k,data),original(n,k,data),"Wynn 2 sliding window");
        if(emit) {
            std::cout<<"wynn_epsilon_fixed\t"<<n<<'\t'<<k<<"\tnext\t100.5\t"<<epsilon(n,k,data)<<'\n';
            std::cout<<"iterated_aitken\t"<<n<<'\t'<<k<<"\tnext\t100.5\t"<<aitken(n,k,data)<<'\n';
        }
    }
    series_result<double> empty,constant,geometric;
    constant.Sn={2,2,2};
    raises<std::out_of_range>([&]{epsilon(0,1,empty);});
    raises<std::out_of_range>([&]{aitken(0,1,empty);});
    levin_sidi_m_algorithm<double,unsigned> m(R::t_type,2.5);
    raises<std::out_of_range>([&]{m(0,1,empty);});
    raises<std::domain_error>([]{levin_sidi_m_algorithm<double,unsigned> bad(R::t_type,-1);});
    raises<std::domain_error>([]{levin_sidi_m_algorithm<double,unsigned> bad(R::t_type,std::numeric_limits<double>::infinity());});
    raises<std::overflow_error>([&]{epsilon(0,1,constant);});
    raises<std::overflow_error>([&]{aitken(0,1,constant);});
    for(unsigned i=0;i<12;++i) {
        geometric.Sn.push_back(3+std::pow(-.5,i));
        geometric.an.push_back(i==0 ? geometric.Sn[0]:geometric.Sn[i]-geometric.Sn[i-1]);
    }
    for(unsigned n=0;n<4;++n) {
        near(epsilon(n,1,geometric),3,"Wynn n=0 and odd/even shifts");
        near(aitken(n,1,geometric),3,"Aitken genuine geometric kernel");
    }
    near(m(1,2,geometric),3,"M exact geometric kernel");
    raises<std::domain_error>([&]{m(0,5,geometric);});
    if(m.how_much(0,0)!=1)throw std::runtime_error("M order zero input count");
    const auto saved=mpfr::mpreal::get_default_prec();
    mpfr::mpreal::set_default_prec(200);
    series_result<mpfr::mpreal> high;
    for(unsigned i=0;i<45;++i) {
        high.Sn.push_back(mpfr::mpreal(3)+mpfr::pow(mpfr::mpreal("-.5"),i));
        high.an.push_back(i==0 ? high.Sn[0]:high.Sn[i]-high.Sn[i-1]);
    }
    levin_sidi_m_algorithm<mpfr::mpreal,unsigned> high_m(R::t_type,mpfr::mpreal("41.5"));
    if(mpfr::abs(high_m(1,40,high)-3)>mpfr::mpreal("1e-50"))throw std::runtime_error("M order 40 MPFR precision");
    mpfr::mpreal::set_default_prec(saved);
}

void sixth_algorithms(bool emit) {
    using namespace shanks::algos;
    using R=shanks::remainders::remainder_type;
    series_result<double> data;
    double total=0;
    for(unsigned j=0;j<45;++j) {
        data.an.push_back((j%2 ? -1.0:1.0)/(j+1));
        total+=data.an.back();data.Sn.push_back(total);
    }
    const std::array<std::pair<const char*,R>,5> variants={{{"t",R::t_type},
        {"next",R::t_wave_type},{"u",R::u_type},{"v",R::v_type},{"v_shift",R::v_wave_type}}};
    for(double beta : {1.0,2.5})for(const auto& variant:variants)
        for(unsigned k=0;k<=3;++k)for(unsigned n=0;n<=2;++n) {
            for(unsigned power : {1u,2u}) {
                f_algorithm<double,unsigned> a(variant.second,[power](unsigned i){return 4+.5*std::pow(double(i),power);},beta);
                const auto value=a(n,k,data);
                if(emit)std::cout<<"homeier_f\t"<<n<<'\t'<<k<<'\t'<<variant.first<<'\t'<<beta<<'\t'<<power<<'\t'<<value<<'\n';
            }
            for(double alpha : {.7,1.2}) {
                h_algorithm<double,unsigned> a(variant.second,beta,alpha);
                const auto value=a(n,k,data);
                if(emit)std::cout<<"homeier_h\t"<<n<<'\t'<<k<<'\t'<<variant.first<<'\t'<<beta<<'\t'<<alpha<<'\t'<<value<<'\n';
            }
            for(int p : {1,2,3,5}) {
                pj_algorithm<double,unsigned> a(variant.second,p,beta);
                const auto value=a(n,k,data);
                if(emit)std::cout<<"homeier_pj\t"<<n<<'\t'<<k<<'\t'<<variant.first<<'\t'<<beta<<'\t'<<p<<'\t'<<value<<'\n';
            }
        }
    f_algorithm<double,unsigned> f(R::t_type);
    h_algorithm<double,unsigned> h(R::t_type,1,.7);
    pj_algorithm<double,unsigned> pj(R::t_type);
    series_result<double> empty,single;
    single.Sn={5};
    raises<std::out_of_range>([&]{f(0,1,empty);});
    raises<std::out_of_range>([&]{h(0,1,empty);});
    raises<std::out_of_range>([&]{pj(0,1,empty);});
    near(f(0,0,single),5,"F order zero needs no terms");
    near(h(0,0,single),5,"H order zero needs no terms");
    near(pj(0,0,single),5,"pJ order zero needs no terms");
    if(h.how_much(0,3)!=7)throw std::runtime_error("H needs 2k+1 partial sums");
    raises<std::domain_error>([&]{pj_algorithm<double,unsigned> a(R::t_type,0);a(0,1,data);});
    raises<std::domain_error>([&]{h_algorithm<double,unsigned> a(R::t_type,1,0);a(0,1,data);});
    raises<std::domain_error>([&]{f_algorithm<double,unsigned> a(R::t_type,[](unsigned){return 4.;});a(0,1,data);});
    raises<std::domain_error>([&]{f_algorithm<double,unsigned> a(R::t_type,[](unsigned i){return 4.-i;});a(0,1,data);});
    raises<std::domain_error>([&]{pj_algorithm<double,unsigned> a(R::t_type,2,-1);a(0,1,data);});
    // Affine-grid F is the factorial S transform with beta equal to the shift.
    for(unsigned k=1;k<=3;++k)for(unsigned n=0;n<=2;++n) {
        levin_sidi_s_algorithm<double,unsigned> s(R::t_type,false,4.0);
        near(f(n,k,data),s(n,k,data),"F affine grid equals factorial S transform");
    }
    // The polynomial (E^2-2*cos(alpha)*E+1)^k is an independent H oracle.
    for(unsigned k=1;k<=3;++k) {
        std::vector<double> coefficients{1};
        for(unsigned level=0;level<k;++level) {
            std::vector<double> next(coefficients.size()+2,0);
            for(unsigned j=0;j<coefficients.size();++j) {
                next[j]+=coefficients[j];next[j+1]-=2*std::cos(.7)*coefficients[j];next[j+2]+=coefficients[j];
            }
            coefficients=next;
        }
        for(unsigned n=0;n<=2;++n) {
            double num=0,den=0;
            for(unsigned j=0;j<coefficients.size();++j) {
                const double w=coefficients[j]*std::pow(n+j+1,k-1)/data.an[n+j];
                num+=w*data.Sn[n+j];den+=w;
            }
            near(h(n,k,data),num/den,"H independent polynomial oracle");
        }
    }
    using M=mpfr::mpreal;
    const auto saved=M::get_default_prec();M::set_default_prec(200);
    series_result<M> high;
    M high_sum=0;
    for(unsigned i=0;i<45;++i) {
        high.an.push_back(M(i%2 ? -1:1)/M(i+1));high_sum+=high.an.back();high.Sn.push_back(high_sum);
    }
    f_algorithm<M,unsigned> high_f(R::t_type);
    levin_sidi_s_algorithm<M,unsigned> high_s(R::t_type,false,M(4));
    if(mpfr::abs(high_f(0,20,high)-high_s(0,20,high))>M("1e-50"))throw std::runtime_error("F MPFR order 20");
    pj_algorithm<M,unsigned> high_pj(R::t_type,3,M("2.5"));
    std::vector<M> num,den;
    for(unsigned i=0;i<=12;++i) {den.push_back(1/high.an[i]);num.push_back(high.Sn[i]*den.back());}
    for(unsigned k=0;k<12;++k)for(unsigned j=0;j<12-k;++j) {
        const M z=M(j)+M("2.5")+M(2*k),factor=z*(z+1);
        num[j]=(num[j+1]-num[j])*factor;den[j]=(den[j+1]-den[j])*factor;
    }
    if(mpfr::abs(high_pj(0,12,high)-num[0]/den[0])>M("1e-50"))throw std::runtime_error("pJ MPFR generalized differences");
    h_algorithm<M,unsigned> high_h(R::t_type,M("2.5"),M(".7"));
    std::vector<M> coefficients{M(1)};
    for(unsigned level=0;level<3;++level) {
        std::vector<M> next(coefficients.size()+2,M(0));
        for(unsigned j=0;j<coefficients.size();++j) {
            next[j]+=coefficients[j];next[j+1]-=2*mpfr::cos(M(".7"))*coefficients[j];next[j+2]+=coefficients[j];
        }
        coefficients=next;
    }
    M hn=0,hd=0;
    for(unsigned j=0;j<coefficients.size();++j) {
        const M w=coefficients[j]*mpfr::pow(M(j)+M("2.5"),2)/high.an[j];
        hn+=w*high.Sn[j];hd+=w;
    }
    if(mpfr::abs(high_h(0,3,high)-hn/hd)>M("1e-50"))throw std::runtime_error("H MPFR polynomial oracle");
    M::set_default_prec(saved);
}

void seventh_algorithms(bool emit) {
    using namespace shanks::algos;
    using R=shanks::remainders::remainder_type;
    series_result<double> data;
    double total=0;
    for(unsigned j=0;j<45;++j) {
        data.an.push_back((j%2 ? -1.0:1.0)/(j+1));total+=data.an.back();data.Sn.push_back(total);
    }
    const std::array<std::pair<const char*,R>,5> variants={{{"t",R::t_type},
        {"next",R::t_wave_type},{"u",R::u_type},{"v",R::v_type},{"v_shift",R::v_wave_type}}};
    for(double beta:{1.,2.5})for(const auto& variant:variants)for(unsigned k=0;k<=3;++k)for(unsigned n=0;n<=3;++n) {
        ford_sidi_3_algorithm<double,unsigned> f(variant.second,beta);
        levin_algorithm<double,unsigned> l(variant.second,false,beta);
        near(f(n,k,data),l(n,k,data),"Ford-Sidi equals independent weighted Levin formula");
        if(emit)std::cout<<"ford_sidi\t"<<n<<'\t'<<k<<'\t'<<variant.first<<'\t'<<beta<<"\t5\t0\t"<<f(n,k,data)<<'\n';
    }
    for(unsigned m:{1u,2u,5u})for(double beta:{1.,.4})for(double reg:{0.,1e-8,.1})for(unsigned n=0;n<15;++n) {
        anderson_acceleration_algorithm<double,unsigned> a(m,beta,reg);
        if(emit)std::cout<<"anderson\t"<<n<<"\t0\tnext\t"<<beta<<'\t'<<m<<'\t'<<reg<<'\t'<<a(n,0,data)<<'\n';
    }
    ford_sidi_2_algorithm<double,unsigned> backward;
    for(unsigned n:{0u,3u})if(emit)std::cout<<"aitken_backtracking\t"<<n<<"\t0\tnext\t1\t5\t0\t"<<backward(n,0,data)<<'\n';
    series_result<double> empty,single,linear,back;
    single.Sn={5};linear.Sn={1,2,3,4,5};back.Sn={1,2,4,6,8,10};
    anderson_acceleration_algorithm<double,unsigned> aa(3,1,0);
    ford_sidi_3_algorithm<double,unsigned> fs;
    raises<std::out_of_range>([&]{aa(0,0,empty);});
    raises<std::out_of_range>([&]{fs(0,1,empty);});
    raises<std::out_of_range>([&]{backward(0,0,empty);});
    near(fs(0,0,single),5,"Ford-Sidi zero order requires no terms");
    if(backward.how_much(0,0)!=3)throw std::runtime_error("Aitken triple requires 3 values");
    near(backward(3,0,back),0,"Backward Aitken includes index zero");
    raises<std::overflow_error>([&]{backward(2,0,linear);});
    raises<std::domain_error>([]{anderson_acceleration_algorithm<double,unsigned> bad(0);});
    raises<std::domain_error>([]{anderson_acceleration_algorithm<double,unsigned> bad(2,-1,0);});
    raises<std::domain_error>([]{anderson_acceleration_algorithm<double,unsigned> bad(2,1,-1);});
    raises<std::domain_error>([]{ford_sidi_3_algorithm<double,unsigned> bad(R::t_type,-1);});
    for(double amplitude:{-1.,1.})for(double ratio:{-.5,.5}) {
        series_result<double> geometric;
        for(unsigned i=0;i<15;++i)geometric.Sn.push_back(3+amplitude*std::pow(ratio,i));
        for(unsigned m:{1u,2u,5u})for(unsigned n=2;n<12;++n) {
            anderson_acceleration_algorithm<double,unsigned> a(m,1,0);
            near(a(n,0,geometric),3,"Anderson affine fixed-point kernel and denominator sign");
        }
    }
    using C=std::complex<double>;
    series_result<C> complex;
    for(unsigned i=0;i<12;++i)complex.Sn.push_back(C(3,2)+C(1,-.2)*std::pow(C(.5,.2),i));
    anderson_acceleration_algorithm<C,unsigned> ca(5,1.,0.);
    const auto cvalue=ca(8,0,complex);
    near(cvalue.real(),3,"Complex Anderson conjugate real");near(cvalue.imag(),2,"Complex Anderson conjugate imaginary");
    using M=mpfr::mpreal;
    const auto saved=M::get_default_prec();M::set_default_prec(400);
    series_result<M> high;M sum=0;
    for(unsigned i=0;i<45;++i) {
        high.an.push_back(M(i%2 ? -1:1)/M(i+1));sum+=high.an.back();high.Sn.push_back(sum);
    }
    ford_sidi_3_algorithm<M,unsigned> high_f(R::t_wave_type,M("2.5"));
    levin_algorithm<M,unsigned> high_l(R::t_wave_type,false,M("2.5"));
    if(mpfr::abs(high_f(0,20,high)-high_l(0,20,high))>M("1e-70"))throw std::runtime_error("Ford-Sidi order 20 MPFR");
    M::set_default_prec(saved);
}

void polymorphic_lifetimes() {
    using A=shanks::algos::series_acceleration<double,unsigned>;
    using R=shanks::remainders::transform_base<double,unsigned>;
    static_assert(std::has_virtual_destructor_v<A>);
    static_assert(std::has_virtual_destructor_v<R>);
    struct AlgorithmProbe final:A {
        bool& destroyed;
        explicit AlgorithmProbe(bool& flag):destroyed(flag){}
        ~AlgorithmProbe()override{destroyed=true;}
        std::size_t how_much(std::size_t,std::size_t)const override{return 0;}
        double operator()(unsigned,unsigned,const series_result<double>&)const override{return 0;}
    };
    struct RemainderProbe final:R {
        bool& destroyed;
        explicit RemainderProbe(bool& flag):destroyed(flag){}
        ~RemainderProbe()override{destroyed=true;}
        double operator()(unsigned,unsigned,const std::vector<double>&,const double&)const override{return 0;}
    };
    bool algorithm=false,remainder=false;
    {std::unique_ptr<A> owner=std::make_unique<AlgorithmProbe>(algorithm);}
    {std::unique_ptr<const R> owner=std::make_unique<RemainderProbe>(remainder);}
    if(!algorithm || !remainder)throw std::runtime_error("Polymorphic ownership must destroy derived objects");
}

void eighth_algorithms(bool emit) {
    using namespace shanks::algos;using R=shanks::remainders::remainder_type;
    series_result<double> data;double sum=0;
    for(unsigned i=0;i<150;++i){data.an.push_back((i%2?-1.:1.)/(i+1));sum+=data.an.back();data.Sn.push_back(sum);}
    const std::array<std::pair<const char*,R>,5> variants={{{"t",R::t_type},{"next",R::t_wave_type},{"u",R::u_type},{"v",R::v_type},{"v_shift",R::v_wave_type}}};
    for(const std::string mode:{"constant","pochhammer","power"})for(double shift:{0.,2.,4.})for(double beta:{1.,2.5})
        for(const auto& variant:variants)for(unsigned k=0;k<=3;++k)for(unsigned n=0;n<=2;++n) {
            auto delta=[mode,shift,beta](unsigned i,unsigned level){
                const double z=i+beta+shift*level;
                return mode=="constant"?1.:(mode=="pochhammer"?1./(z*(z+1)):1./(z*z*z));
            };
            j_transformation_algorithm<double,unsigned> j(30,0.,delta,variant.second,beta);
            const auto value=j(n,k,data);
            if(mode=="constant" && (variant.second!=R::u_type || beta==1.)){drummond_d_algorithm<double,unsigned> d(variant.second);near(value,d(n,k,data),"J constant delta equals Drummond");}
            if(mode=="pochhammer"){pj_algorithm<double,unsigned> p(variant.second,int(shift)+1,beta);near(value,p(n,k,data),"J equals independent pJ");}
            if(emit)std::cout<<"homeier_j\t"<<n<<'\t'<<k<<'\t'<<variant.first<<'\t'<<beta<<'\t'<<mode<<'\t'<<shift<<"\t0.0001\t"<<value<<'\n';
        }
    chang_wynn_algorithm<double,unsigned> chang;
    for(unsigned k=0;k<=3;++k)for(unsigned n=0;n<=3;++n) {
        if(k==1){levin_algorithm<double,unsigned> v(R::v_type,false,1.);near(chang(n,k,data),v(n+1,1,data),"Chang T2 equals shifted v1");}
        if(emit)std::cout<<"chang_wynn\t"<<n<<'\t'<<k<<"\tnext\t1\tconstant\t2\t0.0001\t"<<chang(n,k,data)<<'\n';
    }
    for(double cutoff:{.0001,.1})for(unsigned k:{0u,1u,2u,3u,10u,30u})for(unsigned n=0;n<=2;++n) {
        wynn_epsilon_3_algorithm<double,unsigned> adaptive(cutoff,std::numeric_limits<double>::epsilon());
        const auto value=adaptive(n,k,data);
        if(emit)std::cout<<"wynn_epsilon_adaptive\t"<<n<<'\t'<<k<<"\tnext\t1\tconstant\t2\t"<<cutoff<<'\t'<<value<<'\n';
    }
    series_result<double> empty,single,constant,short_window;
    single.Sn={5};constant.Sn=std::vector<double>(30,5);short_window.Sn={1,2,3};
    j_transformation_algorithm<double,unsigned> j;
    wynn_epsilon_3_algorithm<double,unsigned> adaptive;
    raises<std::out_of_range>([&]{j(0,1,empty);});raises<std::out_of_range>([&]{chang(0,1,empty);});raises<std::out_of_range>([&]{adaptive(0,1,empty);});
    raises<std::out_of_range>([&]{chang(0,1,short_window);});
    near(j(0,0,single),5,"J order zero");near(chang(0,0,single),5,"Chang order zero");near(adaptive(0,0,single),5,"adaptive order zero");
    near(chang(0,4,constant),5,"Chang unavailable cells retain raw constant");near(adaptive(0,10,constant),5,"adaptive constant restart");
    raises<std::domain_error>([]{j_transformation_algorithm<double,unsigned> bad(0);});
    raises<std::domain_error>([]{j_transformation_algorithm<double,unsigned> bad(30,-1.);});
    raises<std::domain_error>([&]{j_transformation_algorithm<double,unsigned> bad(30,0.,[](unsigned,unsigned){return 0.;});bad(0,1,data);});
    raises<std::domain_error>([]{wynn_epsilon_3_algorithm<double,unsigned> bad(0.);});
    raises<std::domain_error>([]{wynn_epsilon_3_algorithm<double,unsigned> bad(.0001,-1.);});
    using C=std::complex<double>;series_result<C> complex;
    for(unsigned i=0;i<12;++i)complex.Sn.push_back(C(3,2)+std::pow(C(.5,.2),i));
    wynn_epsilon_3_algorithm<C,unsigned> ca;
    const auto cv=ca(0,1,complex);near(cv.real(),3,"Complex adaptive real");near(cv.imag(),2,"Complex adaptive imaginary");
    using M=mpfr::mpreal;const auto saved=M::get_default_prec();M::set_default_prec(200);
    series_result<M> high;M total=0;
    for(unsigned i=0;i<45;++i){high.an.push_back(M(i%2?-1:1)/M(i+1));total+=high.an.back();high.Sn.push_back(total);}
    j_transformation_algorithm<M,unsigned> high_j(30,M(0));
    drummond_d_algorithm<M,unsigned> high_d(R::t_wave_type);
    if(mpfr::abs(high_j(0,20,high)-high_d(0,20,high))>M("1e-50"))throw std::runtime_error("J order20 MPFR");
    series_result<M> geometric;for(unsigned i=0;i<15;++i)geometric.Sn.push_back(M(3)+mpfr::pow(M(".5"),i));
    wynn_epsilon_3_algorithm<M,unsigned> high_e;
    chang_wynn_algorithm<M,unsigned> high_c;
    if(mpfr::abs(high_e(0,3,geometric)-3)>M("1e-50") || mpfr::abs(high_c(0,1,geometric)-3)>M("1e-50"))throw std::runtime_error("Final nonlinear MPFR kernel");
    M::set_default_prec(saved);
}

int main(int argc, char** argv) {
    const bool emit = argc > 1;
    std::cout << std::setprecision(17);
    if(argc>1 && std::string(argv[1])=="--extension-terms") { extensions(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--algorithm-values") { algorithm_extensions(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--third-algorithm-values") { third_extensions(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--fifth-algorithm-values") { fifth_algorithms(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--sixth-algorithm-values") { sixth_algorithms(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--seventh-algorithm-values") { seventh_algorithms(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--eighth-algorithm-values") { eighth_algorithms(true); return 0; }
    if(argc>1 && std::string(argv[1])=="--adaptive-evaluate") {
        series_result<double> data;double value;
        while(std::cin>>value)data.Sn.push_back(value);
        if(data.Sn.empty() || data.Sn.size()%2==0)return 2;
        shanks::algos::wynn_epsilon_3_algorithm<double,unsigned> a;
        std::cout<<a(0,static_cast<unsigned>((data.Sn.size()-1)/2),data)<<'\n';return 0;
    }
    if(!emit) { eighth_algorithms(false); polymorphic_lifetimes(); seventh_algorithms(false); sixth_algorithms(false); fifth_algorithms(false); fourth_sources(); extensions(false); algorithm_extensions(false); third_extensions(false); }
    using namespace shanks::series;
    terms<exp_iterator>("exp", .2, emit);
    terms<sin_iterator>("sin", .2, emit);
    terms<cos_iterator>("cos", .2, emit);
    terms<sinh_iterator>("sinh", .2, emit);
    terms<cosh_iterator>("cosh", .2, emit);
    terms<ln1mx_iterator>("ln1mx", .2, emit);
    terms<arctan_x_iterator>("arctan_x", .2, emit);
    terms<arctanh_x_iterator>("arctanh_x", .2, emit);
    terms<arcsin_x_iterator>("arcsin_x", .2, emit);
    terms<arcsinh_x_iterator>("arcsinh_x", .2, emit);
    terms<sqrt_1plusx_iterator>("sqrt_1plusx", .2, emit);
    terms<inverse_sqrt_1m4x_iterator>("inverse_sqrt_1m4x", .05, emit);
    terms<one_minus_sqrt_1minus4x_div_2x_iterator>("one_minus_sqrt_1minus4x_div_2x", .05, emit);
    terms<x_div_1minx_iterator>("x_div_1minx", .2, emit);
    terms<x_div_1minx2_iterator>("x_div_1minx2", .2, emit);
    terms<x_1mx_squared_iterator>("x_1mx_squared", .2, emit);
    terms<inverse_1mx_iterator>("geometric", .2, emit);

    terms<sin_x2_iterator>("sin_x2", .2, emit);
    terms<cos_x2_iterator>("cos_x2", .2, emit);
    terms<sinh_x2_iterator>("sinh_x2", .2, emit);
    terms<arcsin_x2_iterator>("arcsin_x2", .2, emit);
    terms<arctanh_x2_iterator>("arctanh_x2", .2, emit);
    terms<arctan_x2_iterator>("arctan_x2", .2, emit);
    terms<arctan_x3_iterator>("arctan_x3", .2, emit);
    terms<ln1px4_iterator>("ln1px4", .2, emit);
    terms<ln_1_plus_x3_iterator>("ln_1_plus_x3", .2, emit);
    terms<ln1_m_x2_iterator>("ln1_m_x2", .2, emit);
    terms<sqrt_1plusx_min_1_min_x_div_2_iterator>("sqrt_1plusx_min_1_min_x_div_2", .2, emit);
    euler_mascheroni_iterator<double,unsigned> gamma(.2);
    near(gamma.generate(1).an[0], .2*(1-std::log(2.0)), "Euler scale factor");
    auto gamma_data=gamma.generate(128);
    double harmonic=0;
    for(unsigned i=1;i<=128;++i) harmonic+=1.0/i;
    near(gamma_data.Sn.back(),.2*(harmonic-std::log(129.0)),"Euler telescoping identity");
    two_ln2_iterator<double,unsigned> twice_log(.2);
    auto log_data=twice_log.generate(1001);
    const long double k=1001;
    const long double expected=(12*k*k-1)*.2L/(k*(4*k*k-1)*(4*k*k-1));
    if (std::abs(log_data.an.back()/static_cast<double>(expected)-1)>1e-12)
        throw std::runtime_error("Integer overflow in two_ln2");

    raises<std::invalid_argument>([] { ten_minus_x_iterator<double,unsigned> s(.05); });
    raises<std::invalid_argument>([] { ten_minus_x_iterator<double,unsigned> s(5); });
    raises<std::invalid_argument>([] { ten_minus_x_iterator<double,unsigned> s(15); });
    ten_minus_x_iterator<double,unsigned> sawtooth(7.5);
    near(sawtooth.get_sum(),2.5,"Fourier branch");
    near(sawtooth.generate(1).an[0],10/std::numbers::pi,"Fourier first term");

    double hN=0, h2N=0;
    for(unsigned i=1;i<=2002;++i) { h2N+=1.0/i; if(i<=1001) hN+=1.0/i; }
    near(log_data.Sn.back(),.2*(2*(h2N-hN)+1.0/2003-1.0/(2003.0*2003)),"two_ln2 telescoping");

    shanks::algos::wynn_epsilon_1_algorithm<double, unsigned> wynn;
    shanks::algos::shanks_algorithm<double, unsigned> shanks;
    series_result<double> data;
    for (unsigned n = 0; n < 10; ++n)
        data.Sn.push_back(3.0 + std::pow(.5, n) + 2.0 * std::pow(-.25, n));
    for (unsigned n : {0u, 1u, 2u}) {
        near(wynn(n, 0, data), data.Sn[n], "Wynn order zero");
        near(wynn(n, 2, data), 3, "Wynn two exponentials");
        near(shanks(n, 2, data), 3, "Shanks two exponentials");
        near(wynn(n, 1, data), shanks(n, 1, data), "Wynn equals Shanks");
    }
    series_result<double> empty;
    raises<std::out_of_range>([&] { wynn(0, 1, empty); });
    raises<std::out_of_range>([&] { shanks(0, 1, empty); });
    series_result<double> constant;
    constant.Sn = {2, 2, 2};
    raises<std::overflow_error>([&] { wynn(0, 1, constant); });

    shanks::algos::richardson_algorithm<double, unsigned> richardson;
    raises<std::out_of_range>([&] { richardson(1, 0, empty); });
    data.Sn.clear();
    for (unsigned n = 0; n < 8; ++n) data.Sn.push_back(7.0 + 3.0 * std::pow(.25, n));
    for (unsigned n = 1; n < 7; ++n) near(richardson(n, 0, data), 7, "Richardson parity");
    // Exercise production templates beyond double arithmetic.
    using C=std::complex<double>;
    series_result<C> complex_data;
    for(unsigned n=0;n<7;++n)
        complex_data.Sn.push_back(C(3,2)+std::pow(C(.5,.2),n)+C(2)*std::pow(C(-.25,.1),n));
    shanks::algos::wynn_epsilon_1_algorithm<C,unsigned> complex_wynn;
    const auto complex_result=complex_wynn(0,2,complex_data);
    near(complex_result.real(),3,"Complex Wynn real part");
    near(complex_result.imag(),2,"Complex Wynn imaginary part");
    const auto saved_precision=mpfr::mpreal::get_default_prec();
    mpfr::mpreal::set_default_prec(200);
    series_result<mpfr::mpreal> high_data;
    for(unsigned n=0;n<7;++n)
        high_data.Sn.push_back(mpfr::mpreal(3)+mpfr::pow(mpfr::mpreal("0.5"),n)+
                              2*mpfr::pow(mpfr::mpreal("-0.25"),n));
    shanks::algos::wynn_epsilon_1_algorithm<mpfr::mpreal,unsigned> high_wynn;
    if(mpfr::abs(high_wynn(0,2,high_data)-3)>mpfr::mpreal("1e-50"))
        throw std::runtime_error("MPFR Wynn precision");
    mpfr::mpreal::set_default_prec(saved_precision);
    if (!emit) std::cout << "C++ mathematical regressions passed\n";
}
