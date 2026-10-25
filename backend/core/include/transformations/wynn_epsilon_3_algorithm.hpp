#ifndef WYNN_EPSILON_3_ALGORITHM_HPP
#define WYNN_EPSILON_3_ALGORITHM_HPP
#pragma once
#include <optional>
namespace shanks::algos {
/** Condensed epsilon diagonals based on QUADPACK QELG:
 * https://www.netlib.org/quadpack/dqelg.f. Length is updated after compaction;
 * the missing infinite entry is represented explicitly, never by an error.
 * A converged diagonal restarts from the latest raw point; retain the last
 * estimate while refilling so all subsequent input is consumed safely.
 * This API returns an estimate, not a certified error bound.
 * Original authors: Naumov A.U., Lykov D.S., Kreynin R.G., Sobolev Y.A.
 */
template<AcceptedLike T,UnsignedIntLike K>
class wynn_epsilon_3_algorithm final:public series_acceleration<T,K> {
public:
    using float_type=typename real_of<T>::value;
    explicit wynn_epsilon_3_algorithm(const float_type& threshold=utils::cast<float_type,double>()(1e-4),
        const float_type& tolerance=utils::cast<float_type,int>()(0))
        :series_acceleration<T,K>("condensed adaptive epsilon"),threshold_(threshold),tolerance_(tolerance) {
        if(!utils::helpers<float_type>::isfinite(threshold) || threshold<=utils::cast<float_type,int>()(0))throw std::domain_error("Invalid epsilon irregularity threshold");
        if(!utils::helpers<float_type>::isfinite(tolerance) || tolerance<utils::cast<float_type,int>()(0) || tolerance>=utils::cast<float_type,int>()(1))throw std::domain_error("Invalid epsilon relative tolerance");
    }
    std::size_t how_much(std::size_t n,std::size_t order)const override{return n+2*order+1;}
    T operator()(K n,K order,const series_result<T>& data)const override {
        const auto required=how_much(n,order);
        if(data.Sn.size()<required)throw std::out_of_range("Insufficient adaptive epsilon input");
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),one=utils::cast<T,int>()(1,p);
        const float_type tolerance=tolerance_==utils::cast<float_type,int>()(0,p)?utils::helpers<float_type>::epsilon(p):tolerance_;
        using V=std::optional<T>;
        std::vector<V> table(52);
        std::size_t count=0;T estimate=data.Sn[n];bool have=false;
        auto near=[&](const T& difference,const float_type& tol){return utils::math<T>::abs(difference)<=tol;};
        auto tol=[&](const T& a,const T& b){return std::max(utils::math<T>::abs(a),utils::math<T>::abs(b))*tolerance;};
        for(std::size_t input=n;input<required;++input) {
            const T raw=data.Sn[input];table[count++]=raw;
            if(count<3){if(!have)estimate=raw;continue;}
            const auto end=count-1,newelm=end/2;auto final=end;
            table[end+2]=raw;table[end]=std::nullopt;
            T candidate=raw;std::optional<float_type> best;bool restart=false;
            for(std::size_t i=0;i<newelm;++i) {
                const auto at=end-2*i;
                if(!table[at-2] || !table[at-1] || !table[at+2]){final=2*i;break;}
                const T e0=*table[at-2],e1=*table[at-1],e2=*table[at+2];
                const T d2=e2-e1,d3=e1-e0;
                const auto err2=utils::math<T>::abs(d2),err3=utils::math<T>::abs(d3);
                if(near(d2,tol(e2,e1)) && near(d3,tol(e1,e0))){candidate=e2;restart=true;break;}
                const V e3=table[at];table[at]=e1;
                const V d1=e3?V(e1-*e3):V{};
                if(near(d2,tol(e2,e1)) || near(d3,tol(e1,e0)) || (d1 && near(*d1,tol(e1,*e3)))){final=2*i;break;}
                const T ss=(d1?one/(*d1):zero)+one/d2-one/d3;
                if(ss==zero || utils::math<T>::abs(ss*e1)<=threshold_){final=2*i;break;}
                const T value=e1+one/ss;
                if(!utils::helpers<T>::isfinite(value)){final=2*i;break;}
                table[at]=value;
                const auto error=err2+utils::math<T>::abs(value-e2)+err3;
                if(!best || error<=*best){candidate=value;best=error;}
            }
            estimate=candidate;have=true;
            if(restart){table.assign(52,V{});table[0]=raw;count=1;continue;}
            if(final==49)final=48;
            const auto parity=end%2;
            for(std::size_t i=0;i<=newelm;++i)table[parity+2*i]=table[parity+2*i+2];
            if(final!=end)for(std::size_t i=0;i<=final;++i)table[i]=table[end-final+i];
            count=final+1;
        }
        if(!utils::helpers<T>::isfinite(estimate))throw std::overflow_error("Non-finite adaptive epsilon estimate");
        return estimate;
    }
private:
    const float_type threshold_,tolerance_;
};
}
#endif
