#ifndef J_TRANSFORMATION_ALGORITHM_HPP
#define J_TRANSFORMATION_ALGORITHM_HPP
#pragma once
#include "levin_algorithm.hpp"
#define J_TRASFORMATION_DEFAULT_MAX_ORDER 30
#define J_TRASFORMATION_DEFAULT_SAFEGUARD 1.0e-12
namespace shanks::algos {
/** General Homeier J, (33),(94), https://arxiv.org/pdf/math/0005209.
 * Explicit nonzero delta callback replaces the former undefined recurrence.
 * Default delta=1 and next-term omega give Drummond. Safeguard is a relative
 * denominator cancellation threshold (0 disables it), not an additive bias.
 * Original authors: Mkhitaryan A.A., Gerasimov A.I.
 */
template<AcceptedLike T,UnsignedIntLike K>
class j_transformation_algorithm final:public series_acceleration<T,K> {
public:
    using float_type=typename real_of<T>::value;
    std::function<T(K,K)> delta;
    explicit j_transformation_algorithm(K max_order=J_TRASFORMATION_DEFAULT_MAX_ORDER,
        const float_type& safeguard=utils::cast<float_type,double>()(J_TRASFORMATION_DEFAULT_SAFEGUARD),
        std::function<T(K,K)> delta=[](K,K){return utils::cast<T,int>()(1);},
        shanks::remainders::remainder_type variant=shanks::remainders::remainder_type::t_wave_type,
        const float_type& beta=utils::cast<float_type,int>()(1))
        :series_acceleration<T,K>("Homeier generalized J"),delta(delta),max_order_(max_order),safeguard_(safeguard),strategy_(variant,false,beta) {
        if(max_order==0 || !delta)throw std::domain_error("J requires max_order>0 and delta callback");
        if(!utils::helpers<float_type>::isfinite(beta) || beta<=utils::cast<float_type,int>()(0))throw std::domain_error("J beta must be finite and positive");
        if(!utils::helpers<float_type>::isfinite(safeguard) || safeguard<utils::cast<float_type,int>()(0) || safeguard>=utils::cast<float_type,int>()(1))
            throw std::domain_error("J safeguard must be in [0,1)");
    }
    std::size_t how_much(std::size_t n,std::size_t order)const override{return order?strategy_.how_much(n,order):n+1;}
    T operator()(K n,K order,const series_result<T>& data)const override {
        if(order>max_order_)throw std::domain_error("J order exceeds max_order");
        const auto required=how_much(n,order);
        if(data.Sn.size()<required || (order && data.an.size()<required))throw std::out_of_range("Insufficient J input");
        if(order==0)return data.Sn.at(n);
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),beta=utils::cast<T,float_type>()(strategy_.beta_in_use,p);
        std::vector<T> num(order+1),den(order+1);
        std::vector<float_type> bound(order+1);
        for(K j=0;j<=order;++j) {
            den[j]=strategy_.remainder->operator()(n+j,n+j,data.an,beta);
            num[j]=data.Sn[n+j]*den[j];bound[j]=utils::math<T>::abs(den[j]);
        }
        for(K level=0;level<order;++level)for(K j=0;j<order-level;++j) {
            const T divisor=delta(n+j,level);
            if(!utils::helpers<T>::isfinite(divisor) || divisor==zero)throw std::domain_error("J delta must be finite and nonzero");
            num[j]=(num[j+1]-num[j])/divisor;den[j]=(den[j+1]-den[j])/divisor;
            bound[j]=(bound[j+1]+bound[j])/utils::math<T>::abs(divisor);
        }
        if(utils::math<T>::abs(den[0])<=safeguard_*bound[0])throw std::overflow_error("Unresolved J denominator cancellation");
        const T result=num[0]/den[0];
        if(!utils::helpers<T>::isfinite(result))throw std::overflow_error("Non-finite J estimate");
        return result;
    }
private:
    K max_order_;
    float_type safeguard_;
    levin_algorithm<T,K> strategy_;
};
}
#endif
