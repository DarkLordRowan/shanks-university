#ifndef CHANG_WYNN_ALGORITHM_HPP
#define CHANG_WYNN_ALGORITHM_HPP
#pragma once
#include <optional>
namespace shanks::algos {
/** Chang et al. (2019), Eq. (3.20), https://arxiv.org/pdf/1903.10198.
 * Return T_(2*order)^(n). n is now the start and order is honored.
 * Singular cells are unavailable; retain a lower even order or raw value.
 * Original authors: Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template<AcceptedLike T,UnsignedIntLike K>
class chang_wynn_algorithm final:public series_acceleration<T,K> {
public:
    explicit chang_wynn_algorithm():series_acceleration<T,K>("chang wynn"){}
    std::size_t how_much(std::size_t n,std::size_t order)const override{return n+(order?2*order+2:1);}
    T operator()(K n,K order,const series_result<T>& data)const override {
        const auto required=how_much(n,order);
        if(data.Sn.size()<required)throw std::out_of_range("Insufficient Chang-Wynn window");
        if(order==0)return data.Sn.at(n);
        const auto p=utils::helpers<T>::get_precision(data.Sn.at(n));
        const T zero=utils::cast<T,int>()(0,p),one=utils::cast<T,int>()(1,p);
        const std::size_t count=required-n;
        using V=std::optional<T>;
        std::vector<T> delta(count-1);
        std::vector<V> previous(count-1),row(count-3),factors(count-3);
        T estimate=data.Sn[required-1];
        for(std::size_t j=0;j+1<count;++j) {
            delta[j]=data.Sn[n+j+1]-data.Sn[n+j];
            if(delta[j]!=zero) {const T v=one/delta[j];if(utils::helpers<T>::isfinite(v))previous[j]=v;}
        }
        for(std::size_t j=0;j+3<count;++j) {
            const T left=delta[j+1]-delta[j],right=delta[j+2]-delta[j+1];
            const T divisor=delta[j+2]*left-delta[j]*right;
            if(divisor!=zero) {
                const T v=data.Sn[n+j+1]-delta[j]*delta[j+1]*right/divisor;
                const T f=left*right/divisor;
                if(utils::helpers<T>::isfinite(v) && utils::helpers<T>::isfinite(f)){row[j]=v;factors[j]=f;}
            }
        }
        if(row[0])estimate=*row[0];
        for(std::size_t column=2;column<2*static_cast<std::size_t>(order);++column) {
            std::vector<V> next(row.size()-1);
            for(std::size_t j=0;j<next.size();++j)if(row[j] && row[j+1] && previous[j+1] && factors[j]) {
                const T difference=*row[j+1]-*row[j];
                if(difference!=zero) {
                    const T c=utils::cast<T,std::size_t>()(column,p);
                    const T v=*previous[j+1]+(one-c+c*(*factors[j]))/difference;
                    if(utils::helpers<T>::isfinite(v))next[j]=v;
                }
            }
            previous=std::move(row);row=std::move(next);
            if((column+1)%2==0 && row[0])estimate=*row[0];
        }
        return estimate;
    }
};
}
#endif
