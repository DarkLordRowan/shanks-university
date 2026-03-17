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
    K order = console_IO<K>::input("order");
    series_result<T> result = series->generate(n + order * 4);

    for (size_t i{0}; i < result.an.size(); ++i) {
        std::cout << "n = " << i << " " << entry.name << "\n";
        std::cout << "an:     " << utils::helpers<T>::to_string(result.an[i]) << "\n";
        std::cout << "Sn:     " << utils::helpers<T>::to_string(result.Sn[i]) << "\n";
        std::cout << "S:      " << utils::helpers<T>::to_string(series->get_sum()) << "\n";
        std::cout << "S - Sn: " << utils::helpers<T>::to_string(series->get_sum() - result.Sn[i]) << "\n";
        std::cout << "\n\n";
    }

    std::ofstream fout(std::string(shanks::series::series_registry_metadata::get_names()[series_idx]) +
                       std::to_string(order) + std::string(".csv"));

    std::vector<int> ints = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<std::function<std::unique_ptr<shanks::algos::series_acceleration<T, K>>(int, T)>> methods_with_int_T{
        [](int p, T beta) {
            return std::make_unique<shanks::algos::pj_algorithm<T, K>>(shanks::remainders::remainder_type::u_type, p,
                                                                       beta);
        },
        [](int p, T beta) {
            return std::make_unique<shanks::algos::pj_algorithm<T, K>>(shanks::remainders::remainder_type::t_type, p,
                                                                       beta);
        },
        [](int p, T beta) {
            return std::make_unique<shanks::algos::pj_algorithm<T, K>>(shanks::remainders::remainder_type::t_wave_type,
                                                                       p, beta);
        },
        [](int p, T beta) {
            return std::make_unique<shanks::algos::pj_algorithm<T, K>>(shanks::remainders::remainder_type::v_type, p,
                                                                       beta);
        },
        [](int p, T beta) {
            return std::make_unique<shanks::algos::pj_algorithm<T, K>>(shanks::remainders::remainder_type::v_wave_type,
                                                                       p, beta);
        }};

    std::vector<std::pair<T, T>> T_T = {
        std::make_pair(utils::cast<T, double>()(0.0001), utils::cast<T, double>()(std::numbers::pi / 3.0)),
        std::make_pair(utils::cast<T, double>()(0.01), utils::cast<T, double>()(std::numbers::pi / 4.0)),
        std::make_pair(utils::cast<T, double>()(0.1), utils::cast<T, double>()(std::numbers::pi / 6.0)),
        std::make_pair(utils::cast<T, double>()(5.0), utils::cast<T, double>()(std::numbers::pi / 12.0)),
        std::make_pair(utils::cast<T, double>()(10.0), utils::cast<T, double>()(std::numbers::pi / 36.0)),
        std::make_pair(utils::cast<T, double>()(20.0), utils::cast<T, double>()(std::numbers::pi / 120.0)),
        std::make_pair(utils::cast<T, double>()(std::numbers::pi / 3.0), utils::cast<T, double>()(0.0001)),
        std::make_pair(utils::cast<T, double>()(std::numbers::pi / 4.0), utils::cast<T, double>()(0.01)),
        std::make_pair(utils::cast<T, double>()(std::numbers::pi / 6.0), utils::cast<T, double>()(0.1)),
        std::make_pair(utils::cast<T, double>()(std::numbers::pi / 12.0), utils::cast<T, double>()(5.0)),
        std::make_pair(utils::cast<T, double>()(std::numbers::pi / 36.0), utils::cast<T, double>()(10.0)),
        std::make_pair(utils::cast<T, double>()(std::numbers::pi / 120.0), utils::cast<T, double>()(20.0))};

    std::vector<std::function<std::unique_ptr<shanks::algos::series_acceleration<T, K>>(T, T)>> methods_with_T_T = {
        [](T a, T b) {
            return std::make_unique<shanks::algos::h_algorithm<T, K>>(shanks::remainders::remainder_type::u_type, a, b);
        },
        [](T a, T b) {
            return std::make_unique<shanks::algos::h_algorithm<T, K>>(shanks::remainders::remainder_type::t_type, a, b);
        },
        [](T a, T b) {
            return std::make_unique<shanks::algos::h_algorithm<T, K>>(shanks::remainders::remainder_type::t_wave_type,
                                                                      a, b);
        },
        [](T a, T b) {
            return std::make_unique<shanks::algos::h_algorithm<T, K>>(shanks::remainders::remainder_type::v_type, a, b);
        },
        [](T a, T b) {
            return std::make_unique<shanks::algos::h_algorithm<T, K>>(shanks::remainders::remainder_type::v_wave_type,
                                                                      a, b);
        },
        [](T a, T b) {
            return std::make_unique<shanks::algos::wynn_rho_algorithm<T, K>>(
                shanks::numerators::numerator_type::gamma_rho_type, a, b);
        }};

    std::vector<T> betas = {
        utils::cast<T, double>()(0.0001), utils::cast<T, double>()(0.01),   utils::cast<T, double>()(0.1),
        utils::cast<T, double>()(2.0),    utils::cast<T, double>()(10.0),   utils::cast<T, double>()(50.0),
        utils::cast<T, double>()(1000.0), utils::cast<T, double>()(5000.0),
    };

    std::vector<std::function<std::unique_ptr<shanks::algos::series_acceleration<T, K>>(T)>> methods_with_one_param = {
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(shanks::remainders::remainder_type::u_type,
                                                                          false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(shanks::remainders::remainder_type::t_type,
                                                                          false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(shanks::remainders::remainder_type::v_type,
                                                                          false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(shanks::remainders::remainder_type::u_type,
                                                                          true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(shanks::remainders::remainder_type::t_type,
                                                                          true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(shanks::remainders::remainder_type::v_type,
                                                                          true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_m_algorithm<T, K>>(
                shanks::remainders::remainder_type::u_type, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_m_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_type, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_m_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_m_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_type, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_m_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::u_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, false, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::u_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::levin_sidi_s_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, true, beta);
        },
        [](T beta) {
            return std::make_unique<shanks::algos::wynn_rho_algorithm<T, K>>(
                shanks::numerators::numerator_type::generalized_type, beta);
        }};

    std::vector<std::function<std::unique_ptr<shanks::algos::series_acceleration<T, K>>(void)>> methods = {
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::u_type, false);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_type, false);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, false);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_type, false);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, false);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::u_type, true);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_type, true);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_type, true);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::t_wave_type, true);
        },
        []() {
            return std::make_unique<shanks::algos::drummond_d_algorithm<T, K>>(
                shanks::remainders::remainder_type::v_wave_type, true);
        },
        []() { return std::make_unique<shanks::algos::anderson_acceleration_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::brezinski_theta_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::chang_wynn_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::f_algorithm<T, K>>(shanks::remainders::remainder_type::u_type); },
        []() { return std::make_unique<shanks::algos::f_algorithm<T, K>>(shanks::remainders::remainder_type::t_type); },
        []() {
            return std::make_unique<shanks::algos::f_algorithm<T, K>>(shanks::remainders::remainder_type::t_wave_type);
        },
        []() { return std::make_unique<shanks::algos::f_algorithm<T, K>>(shanks::remainders::remainder_type::v_type); },
        []() {
            return std::make_unique<shanks::algos::f_algorithm<T, K>>(shanks::remainders::remainder_type::v_wave_type);
        },
        []() { return std::make_unique<shanks::algos::j_transformation_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::ford_sidi_2_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::ford_sidi_3_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::lubkin_w_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::overholt_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::richardson_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::shanks_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::shanks_transform_alternating<T, K>>(); },
        []() { return std::make_unique<shanks::algos::weniger_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::wynn_epsilon_1_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::wynn_epsilon_2_algorithm<T, K>>(); },
        []() { return std::make_unique<shanks::algos::wynn_epsilon_3_algorithm<T, K>>(); },
        []() {
            return std::make_unique<shanks::algos::wynn_rho_algorithm<T, K>>(
                shanks::numerators::numerator_type::rho_type);
        }};

    fout << "limit,an,s";
    for (const auto& x : methods) {
        std::string s = x()->get_name();
        std::replace(std::begin(s), std::end(s), ' ', '_');
        fout << "," << s << ",|An-lim|,|Tn-lim|,|An-lim|>|Tn-lim|";
    }

    // one params
    for (const auto& beta : betas) {
        for (const auto& x : methods_with_one_param) {
            std::string s = x(beta)->get_name();
            std::replace(std::begin(s), std::end(s), ' ', '_');
            fout << "," << s << ",|An-lim|,|Tn-lim|,|An-lim|>|Tn-lim|";
        }
    }

    // two param
    for (const auto& [a, b] : T_T)
        for (const auto& x : methods_with_T_T) {
            std::string s = x(a, b)->get_name();
            std::replace(std::begin(s), std::end(s), ' ', '_');
            fout << "," << s << ",|An-lim|,|Tn-lim|,|An-lim|>|Tn-lim|";
        }

    for (const auto& a : ints) {
        for (const auto& b : betas) {
            for (const auto& x : methods_with_int_T) {
                std::string s = x(a, b)->get_name();
                std::replace(std::begin(s), std::end(s), ' ', '_');
                fout << "," << s << ",|Sn-lim|,|Tn-lim|,|Sn-lim|>|Tn-lim|";
            }
        }
    }

    fout << "\n";

    for (K i{0}; i <= n; ++i) {
        fout << utils::helpers<T>::to_string(series->get_sum()) << "," << utils::helpers<T>::to_string(result.an[i])
             << "," << utils::helpers<T>::to_string(result.Sn[i]);
        for (const auto& x : methods) {
            std::string value;
            try {
                value = utils::helpers<T>::to_string(x()->operator()(i, order, result));
                fout << ",\"" << value << "\",\""
                     << utils::helpers<T>::to_string(utils::math<T>::abs(result.Sn[i] - series->get_sum())) << "\",\""
                     << utils::helpers<T>::to_string(
                            utils::math<T>::abs(x()->operator()(i, order, result) - series->get_sum()))
                     << "\",\""
                     << (utils::math<T>::abs(result.Sn[i] - series->get_sum()) >
                         utils::math<T>::abs(
                             utils::math<T>::abs(x()->operator()(i, order, result) - series->get_sum())))
                     << "\"";
            } catch (const std::exception& e) {
                std::string s = std::string(e.what());
                std::replace(std::begin(s), std::end(s), ' ', '_');
                value = s;
                fout << ",\"" << value << "\",\"" << value << "\",\"" << value << "\",\"" << value << "\"";
            }
        }
        for (const auto& beta : betas) {
            for (const auto& x : methods_with_one_param) {
                std::string value;
                try {
                    value = utils::helpers<T>::to_string(x(beta)->operator()(i, order, result));
                    fout << ",\"" << value << "\",\""
                         << utils::helpers<T>::to_string(utils::math<T>::abs(result.Sn[i] - series->get_sum()))
                         << "\",\""
                         << utils::helpers<T>::to_string(
                                utils::math<T>::abs(x(beta)->operator()(i, order, result) - series->get_sum()))
                         << "\",\""
                         << (utils::math<T>::abs(result.Sn[i] - series->get_sum()) >
                             utils::math<T>::abs(
                                 utils::math<T>::abs(x(beta)->operator()(i, order, result) - series->get_sum())))
                         << "\"";
                } catch (const std::exception& e) {
                    std::string s = std::string(e.what());
                    std::replace(std::begin(s), std::end(s), ' ', '_');
                    value = s;
                    fout << ",\"" << value << "\",\"" << value << "\",\"" << value << "\",\"" << value << "\"";
                }
            }
        }
        for (const auto& [a, b] : T_T) {
            for (const auto& x : methods_with_T_T) {
                std::string value;
                try {
                    value = utils::helpers<T>::to_string(x(a, b)->operator()(i, order, result));
                    fout << ",\"" << value << "\",\""
                         << utils::helpers<T>::to_string(utils::math<T>::abs(result.Sn[i] - series->get_sum()))
                         << "\",\""
                         << utils::helpers<T>::to_string(
                                utils::math<T>::abs(x(a, b)->operator()(i, order, result) - series->get_sum()))
                         << "\",\""
                         << (utils::math<T>::abs(result.Sn[i] - series->get_sum()) >
                             utils::math<T>::abs(
                                 utils::math<T>::abs(x(a, b)->operator()(i, order, result) - series->get_sum())))
                         << "\"";
                } catch (const std::exception& e) {
                    std::string s = std::string(e.what());
                    std::replace(std::begin(s), std::end(s), ' ', '_');
                    value = s;
                    fout << ",\"" << value << "\",\"" << value << "\",\"" << value << "\",\"" << value << "\"";
                }
            }
        }
        for (const auto& a : ints) {
            for (const auto& b : betas) {
                for (const auto& x : methods_with_int_T) {
                    std::string value;
                    try {
                        value = utils::helpers<T>::to_string(x(a, b)->operator()(i, order, result));
                        fout << ",\"" << value << "\",\""
                             << utils::helpers<T>::to_string(utils::math<T>::abs(result.Sn[i] - series->get_sum()))
                             << "\",\""
                             << utils::helpers<T>::to_string(
                                    utils::math<T>::abs(x(a, b)->operator()(i, order, result) - series->get_sum()))
                             << "\",\""
                             << (utils::math<T>::abs(result.Sn[i] - series->get_sum()) >
                                 utils::math<T>::abs(
                                     utils::math<T>::abs(x(a, b)->operator()(i, order, result) - series->get_sum())))
                             << "\"";
                    } catch (const std::exception& e) {
                        std::string s = std::string(e.what());
                        std::replace(std::begin(s), std::end(s), ' ', '_');
                        value = s;
                        fout << ",\"" << value << "\",\"" << value << "\",\"" << value << "\",\"" << value << "\"";
                    }
                }
            }
        }
        fout << "\n";
    }

    fout.close();
}

#endif
