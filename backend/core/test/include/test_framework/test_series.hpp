#ifndef TEST_SERIES_HPP
#define TEST_SERIES_HPP
#pragma once

template <typename T>
void test_series() {
    using K = size_t;

    print_series_info();
    size_t series_idx = console_IO<size_t>::input("series_index") - 1;

    const auto& entry = shanks::series::series_registry<T, K>::get_entries()[series_idx];
    T x = console_IO<T>::input();
    T tParam = utils::cast<T, int>()(1);
    K kParam = static_cast<K>(1);

    if (entry.needsT || entry.needsK) {
        std::cout << "Enter additional parameters for " << entry.name << ":" << '\n';
        if (entry.needsT) tParam = console_IO<T>::input("tParam (parameter/alpha)");
        if (entry.needsK) kParam = console_IO<K>::input("kParam (order/m)");
    }

    std::unique_ptr<shanks::series::series_base<T, K>> series =
        shanks::series::series_registry<T, K>::create(series_idx, x, tParam, kParam);

    std::cout << "\nChosen series: " << entry.name << "\n";

    K n = console_IO<K>::input("n");

    series_result<T> result = series->generate(
        n, static_cast<shanks::series::series_id_t>(series_idx) == shanks::series::series_id_t::strange_seq1_id ||
               static_cast<shanks::series::series_id_t>(series_idx) == shanks::series::series_id_t::strange_seq2_id);

    for (size_t i{0}; i < result.an.size(); ++i) {
        std::cout << "n = " << i << " " << entry.name << "\n";
        std::cout << "an:     " << utils::helpers<T>::to_string(result.an[i]) << "\n";
        std::cout << "Sn:     " << utils::helpers<T>::to_string(result.Sn[i]) << "\n";
        std::cout << "S:      " << utils::helpers<T>::to_string(series->get_sum()) << "\n";
        std::cout << "S - Sn: " << utils::helpers<T>::to_string(series->get_sum() - result.Sn[i]) << "\n";
        std::cout << "\n\n";
    }
}

#endif