#ifndef FORD_SIDI_2_HPP
#define FORD_SIDI_2_HPP
#pragma once
namespace shanks::algos {
/** Historical name: this helper is backward-search Aitken, not general Ford-Sidi.
 * Aitken formula: https://dlmf.nist.gov/3.9.E7. n is the requested triple start;
 * order is retained and ignored. Search includes index 0, needs n+3 values.
 * Original authors: Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template<AcceptedLike T,UnsignedIntLike K>
class ford_sidi_2_algorithm final:public series_acceleration<T,K> {
public:
    explicit ford_sidi_2_algorithm():series_acceleration<T,K>("backtracking Aitken (legacy Ford-Sidi 2)"){}
    std::size_t how_much(std::size_t n,std::size_t)const override{return n+3;}
    T operator()(K n,K,const series_result<T>& data)const override {
        if(data.Sn.size()<how_much(n,0))throw std::out_of_range("Insufficient backward Aitken data");
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),two=utils::cast<T,int>()(2,p);
        K m=n;
        while(true) {
            const T delta=data.Sn[m+1]-data.Sn[m];
            const T denominator=data.Sn[m+2]-two*data.Sn[m+1]+data.Sn[m];
            if(denominator!=zero) {
                const T result=data.Sn[m]-delta*delta/denominator;
                if(!utils::helpers<T>::isfinite(result))throw std::overflow_error("Non-finite backward Aitken estimate");
                return result;
            }
            if(m==0)break;
            --m;
        }
        throw std::overflow_error("No nonsingular backward Aitken window");
    }
};
}
#endif
