#ifndef SERIES_BASE_HPP
#define SERIES_BASE_HPP
#pragma once

namespace shanks {
namespace series {

/**
 * @file series_base.hpp
 * @brief Base class for all series implementations.
 */

template <AcceptedLike T, UnsignedIntLike K>
class series_base {
public:
    series_base() = delete;
    explicit series_base(T x) : x(x) {}
    virtual ~series_base() = default;

    T get_x() const { return x; }
    virtual std::string get_name() const { return "unknown_series"; }

    virtual T get_sum() const = 0;
    virtual series_result<T> generate(K n) = 0;
    virtual bool is_invalid() const = 0;

protected:
    T x;

    template <typename IteratorFunc>
    static series_result<T> generate_from_iterator_series(K n, IteratorFunc func) {
        if (n == 0) return {std::vector<T>(), std::vector<T>()};

        const T first = func();
        const size_t precision = utils::helpers<T>::get_precision(first);

        std::vector<T> an(n, utils::cast<T, int>()(0, precision));
        std::vector<T> Sn(n, utils::cast<T, int>()(0, precision));

        an[0] = first;
        Sn[0] = first;

        for (K i = 1; i < n; ++i) {
            an[i] = func();
            Sn[i] = an[i] + Sn[i - static_cast<K>(1)];
        }

        return {Sn, an};
    }

    template <typename IteratorFunc>
    static series_result<T> generate_from_iterator_sequence(K n, IteratorFunc func) {
        if (n == 0) return {std::vector<T>(), std::vector<T>()};

        const T first = func();
        const size_t precision = utils::helpers<T>::get_precision(first);

        std::vector<T> an(n, utils::cast<T, int>()(0, precision));
        std::vector<T> Sn(n, utils::cast<T, int>()(0, precision));

        an[0] = first;
        Sn[0] = first;

        for (K i = 1; i < n; ++i) {
            T current = func();
            // I consider this logic stupid, but I'm not going to say anything against it. Decide for yourselves.
            an[i] = current;
            Sn[i] = current;
        }

        return {Sn, an};
    }
};

template <AcceptedLike T, UnsignedIntLike K, typename State>
class series_base_iter : public series_base<T, K> {
public:
    explicit series_base_iter(T x) : series_base<T, K>(x) {}

    virtual T get_sum() const override = 0;
    virtual State initial_state() const { return State(); };
    virtual T next(K index, State& state) const = 0;

    bool is_invalid() const override { return true; }

    series_result<T> generate(K n) override {
        State state = initial_state();

        return series_base<T, K>::generate_from_iterator_series(
            n, [this, &state, i = K(0)]() mutable { return this->next(i++, state); });
    }
};

template <AcceptedLike T, UnsignedIntLike K>
class series_base_succ : public series_base_iter<T, K, T> {
public:
    explicit series_base_succ(T x) : series_base_iter<T, K, T>(x) {}

    virtual T get_sum() const override = 0;
    T initial_state() const { return utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)); };
    virtual T next(K index, T& state) const = 0;

    bool is_invalid() const override { return true; }
};

template <AcceptedLike T, UnsignedIntLike K>
class series_base2_succ : public series_base_iter<T, K, std::pair<T, T>> {
public:
    explicit series_base2_succ(T x) : series_base_iter<T, K, T>(x) {}

    virtual T get_sum() const override = 0;
    T initial_state() const { return utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)); };
    virtual T next(K index, T& state) const = 0;

    bool is_invalid() const override { return true; }
};

template <AcceptedLike T, UnsignedIntLike K, typename State>
class series_base_seq_iter : public series_base<T, K> {
public:
    explicit series_base_seq_iter(T x) : series_base<T, K>(x) {}

    virtual T get_sum() const override = 0;
    virtual State initial_state() const { return State(); };
    virtual T next(K index, State& state) const = 0;

    bool is_invalid() const override { return true; }

    series_result<T> generate(K n) override {
        State state = initial_state();

        return series_base<T, K>::generate_from_iterator_sequence(
            n, [this, &state, i = K(0)]() mutable { return this->next(i++, state); });
    }
};

template <AcceptedLike T, UnsignedIntLike K>
class series_base_seq_succ : public series_base_seq_iter<T, K, T> {
public:
    explicit series_base_seq_succ(T x) : series_base_seq_iter<T, K, T>(x) {}

    virtual T get_sum() const override = 0;
    T initial_state() const { return utils::cast<T, int>()(0, utils::helpers<T>::get_precision(this->x)); };
    virtual T next(K index, T& state) const = 0;

    bool is_invalid() const override { return true; }
};

}  // namespace series
}  // namespace shanks

#endif  // SERIES_BASE_HPP
