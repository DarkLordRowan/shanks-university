#ifndef ANDERSON_ACCELERATION_ALGORITHM_HPP
#define ANDERSON_ACCELERATION_ALGORITHM_HPP
#pragma once
#define ANDERSON_DEFAULT_MAX_ORDER 50
#define ANDERSON_DEFAULT_BETA 1.0
#define ANDERSON_DEFAULT_SAFEGUARD 1.0e-12

namespace shanks::algos {
/** Scalar Type-II Anderson on observed pairs (s_i,s_(i+1)); no feedback solver.
 * Walker/Ni (2011), Secs. 1,4: https://doi.org/10.1137/10078356X.
 * safeguard is now a nonnegative ridge penalty on difference coefficients.
 * For zero penalty use the minimum-norm solution of the scalar LS problem.
 * Original authors: Mkhitaryan A.A., Gerasimov A.I.
 */
template<AcceptedLike T,UnsignedIntLike K>
class anderson_acceleration_algorithm final: public series_acceleration<T,K> {
public:
    using float_type=typename real_of<T>::value;
    explicit anderson_acceleration_algorithm(K m=ANDERSON_DEFAULT_MAX_ORDER,
        const float_type& beta=utils::cast<float_type,double>()(ANDERSON_DEFAULT_BETA),
        const float_type& safeguard=utils::cast<float_type,double>()(ANDERSON_DEFAULT_SAFEGUARD))
        :series_acceleration<T,K>("scalar observed-pair Anderson"),m_(m),safeguard_(safeguard) {
        if(m==0)throw std::domain_error("Anderson memory must be positive");
        if(!utils::helpers<float_type>::isfinite(safeguard_) || safeguard_<utils::cast<float_type,int>()(0))
            throw std::domain_error("Anderson ridge penalty must be finite and nonnegative");
        update_beta(beta);
    }
    void update_beta(const float_type& value) {
        if(!utils::helpers<float_type>::isfinite(value) || value<=utils::cast<float_type,int>()(0) || value>utils::cast<float_type,int>()(1))
            throw std::domain_error("Anderson beta must be finite and in (0,1]");
        beta_=value;
    }
    std::size_t how_much(std::size_t n,std::size_t)const override{return n+1;}
    T operator()(K n,K,const series_result<T>& data)const override {
        if(data.Sn.size()<how_much(n,0))throw std::out_of_range("Insufficient Anderson data");
        if(n==0)return data.Sn.at(0);
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),beta=utils::cast<T,float_type>()(beta_,p);
        const T residual=data.Sn[n]-data.Sn[n-1];
        T denominator=utils::cast<T,float_type>()(safeguard_,p),cross=zero;
        const K count=std::min(m_,static_cast<K>(n-1));
        for(K j=n-1-count;j<n-1;++j) {
            const T dx=data.Sn[j+1]-data.Sn[j];
            const T df=data.Sn[j+2]-data.Sn[j+1]-dx;
            denominator+=conjugate(df)*df;
            cross+=(dx+beta*df)*conjugate(df);
        }
        T result=data.Sn[n-1]+beta*residual;
        if(denominator!=zero)result-=residual*cross/denominator;
        if(!utils::helpers<T>::isfinite(result))throw std::overflow_error("Non-finite Anderson estimate");
        return result;
    }
private:
    template<typename U>static U conjugate(const U& value) {
        if constexpr(requires {value.real();value.imag();})return U(value.real(),-value.imag());
        else if constexpr(requires {value.value;})return U(conjugate(value.value));
        else return value;
    }
    K m_;
    float_type beta_,safeguard_;
};
}
#endif
