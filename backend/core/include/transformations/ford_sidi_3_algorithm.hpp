#ifndef FORD_SIDI_3_HPP
#define FORD_SIDI_3_HPP
#pragma once
#include "levin_algorithm.hpp"
namespace shanks::algos {
/** Ford-Sidi elimination for the error basis omega_i*(beta+i)^(-r), r=0..k-1.
 * https://doi.org/10.1137/0724080. Evaluates the same model as Levin by a
 * different elimination algorithm; no claim to implement every W^(m) variant.
 * Interface correction: n is the start, order is k (formerly ignored).
 * Original authors: Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template<AcceptedLike T,UnsignedIntLike K>
class ford_sidi_3_algorithm final:public series_acceleration<T,K> {
public:
    using float_type=typename real_of<T>::value;
    explicit ford_sidi_3_algorithm(shanks::remainders::remainder_type variant=shanks::remainders::remainder_type::u_type,
        const float_type& beta=utils::cast<float_type,int>()(1))
        :series_acceleration<T,K>("Ford-Sidi elimination (Levin basis)"),strategy_(variant,false,beta){
        if(!utils::helpers<float_type>::isfinite(beta) || beta<=utils::cast<float_type,int>()(0))
            throw std::domain_error("Ford-Sidi beta must be finite and positive");
    }
    std::size_t how_much(std::size_t n,std::size_t order)const override {
        return order==0?n+1:strategy_.how_much(n,order);
    }
    T operator()(K n,K order,const series_result<T>& data)const override {
        const auto required=how_much(n,order);
        if(data.Sn.size()<required || (order>0 && data.an.size()<required))throw std::out_of_range("Insufficient Ford-Sidi data");
        if(order==0)return data.Sn.at(n);
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),one=utils::cast<T,int>()(1,p);
        const T beta=utils::cast<T,float_type>()(strategy_.beta_in_use,p);
        std::vector<T> num(order+1),den(order+1);
        std::vector<std::vector<T>> auxiliary(order,std::vector<T>(order+1));
        for(K j=0;j<=order;++j) {
            const K index=n+j;
            den[j]=strategy_.remainder->operator()(index,index,data.an,beta);
            num[j]=data.Sn.at(index)*den[j];
            const T inverse=one/(beta+utils::cast<T,K>()(index,p));
            T power=one;
            for(K r=1;r<order;++r) {power*=inverse;auxiliary[r][j]=power;}
        }
        for(K level=1;level<=order;++level)for(K j=0;j<=order-level;++j) {
            const T divisor=level==order?one:auxiliary[level][j+1]-auxiliary[level][j];
            if(divisor==zero)throw std::overflow_error("Singular Ford-Sidi elimination pivot");
            num[j]=(num[j+1]-num[j])/divisor;den[j]=(den[j+1]-den[j])/divisor;
            for(K r=level+1;r<order;++r)auxiliary[r][j]=(auxiliary[r][j+1]-auxiliary[r][j])/divisor;
        }
        if(den[0]==zero)throw std::overflow_error("Singular Ford-Sidi denominator");
        const T result=num[0]/den[0];
        if(!utils::helpers<T>::isfinite(result))throw std::overflow_error("Non-finite Ford-Sidi estimate");
        return result;
    }
private:
    levin_algorithm<T,K> strategy_;
};
}
#endif
