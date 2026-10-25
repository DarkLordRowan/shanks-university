#ifndef SHANKS_ALGTERNATING_ALGORITHM_HPP
#define SHANKS_ALGTERNATING_ALGORITHM_HPP
#pragma once
namespace shanks::algos {
/** Historical name retained for compatibility: this is iterated Aitken,
 * NOT the higher-order Shanks transform. It also applies to nonalternating data.
 * Weniger (1989), (5.1-15): https://arxiv.org/pdf/math/0306302.
 * n is now consistently the starting index; k levels need 2*k+1 values.
 */
template<AcceptedLike T,UnsignedIntLike K>
class shanks_transform_alternating : public series_acceleration<T,K> {
public:
    explicit shanks_transform_alternating():series_acceleration<T,K>("iterated aitken (legacy shanks alternating)") {}
    std::size_t how_much(std::size_t n,std::size_t order) const {return n+2*order+1;}
    T operator()(K n,K order,const series_result<T>& data) const override {
        const auto required=how_much(n,order);
        if(data.Sn.size()<required) throw std::out_of_range("Insufficient partial sums for iterated Aitken");
        if(order==0)return data.Sn.at(n);
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),two=utils::cast<T,int>()(2,p);
        std::vector<T> row(data.Sn.begin()+n,data.Sn.begin()+required);
        for(K level=0;level<order;++level) {
            for(std::size_t j=0;j+2<row.size();++j) {
                const T delta=row[j+1]-row[j];
                const T denominator=row[j+2]-two*row[j+1]+row[j];
                if(denominator==zero)throw std::overflow_error("Singular iterated Aitken denominator");
                row[j]-=delta*delta/denominator;
                if(!utils::helpers<T>::isfinite(row[j]))throw std::overflow_error("Non-finite iterated Aitken cell");
            }
            row.resize(row.size()-2);
        }
        return row[0];
    }
};
}
#endif
