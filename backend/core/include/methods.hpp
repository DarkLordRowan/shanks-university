#ifndef METHODS_HPP
#define METHODS_HPP
#pragma once

/**
 * @file methods.hpp
 * @brief This file aggregates all available transformation algorithms and defines their IDs.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace algos {

/**
 * @brief Enumeration of all supported sequence transformation IDs.
 * Generated automatically from transformation_registry.def.
 */
enum class transformation_id_t {
    null_transformation_id,
#define TRANSFORMATION_ENTRY(id, name, camel, cls, binding, ...) id,
#include "transformation_registry.def"
#undef TRANSFORMATION_ENTRY
    transformation_id_t_count,
};

}  // namespace algos
}  // namespace shanks

// Selection of transformation algorithms for inclusion
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "transformations/anderson_acceleration_algorithm.hpp"
#include "transformations/brezinski_theta_algorithm.hpp"
#include "transformations/chang_wynn_algorithm.hpp"
#include "transformations/drummond_d_algorithm.hpp"
#include "transformations/ford_sidi_2_algorithm.hpp"
#include "transformations/ford_sidi_3_algorithm.hpp"
#include "transformations/j_transformation_algorithm.hpp"
#include "transformations/levin_algorithm.hpp"
#include "transformations/levin_sidi_m_algorithm.hpp"
#include "transformations/levin_sidi_s_algorithm.hpp"
#include "transformations/lubkin_w_algorithm.hpp"
#include "transformations/richardson_algorithm.hpp"
#include "transformations/shanks_algorithm.hpp"
#include "transformations/shanks_alternating_algorithm.hpp"
#include "transformations/weniger_algorithm.hpp"
#include "transformations/wynn_epsilon_1_algorithm.hpp"
#include "transformations/wynn_epsilon_2_algorithm.hpp"
#include "transformations/wynn_epsilon_3_algorithm.hpp"
#include "transformations/wynn_rho_algorithm.hpp"

namespace shanks {
namespace algos {

/**
 * @brief Metadata and factory logic for transformations.
 */
template <AcceptedLike T, UnsignedIntLike K>
class transformation_registry {
public:
    using Factory = std::function<std::unique_ptr<series_acceleration<T, K>>()>;

    struct entry {
        transformation_id_t id;
        std::string name;
        Factory factory;
    };

    /**
     * @brief Get the transformation/algorithms entries
     * return a vector of entry structs containing transformation id, name, and a function to create one
     * @return const std::vector<entry>&
     */
    static const std::vector<entry>& get_entries() {
        static const std::vector<entry> entries = {
#define TRANSFORMATION_ENTRY(id, name, camel, cls, binding, ...) \
    {transformation_id_t::id, name, []()->std::unique_ptr<series_acceleration<T, K>> __VA_ARGS__},
#include "transformation_registry.def"
#undef TRANSFORMATION_ENTRY
        };
        return entries;
    }

    /**
     * @brief Create unique_ptr of a transformation with given id
     * @param id transformation_id_t
     * @throws std::domain_error if an invalid transformation ID given
     * @return std::unique_ptr<series_acceleration<T, K>>
     */
    static std::unique_ptr<series_acceleration<T, K>> create(transformation_id_t id) {
        for (const auto& e : get_entries())
            if (e.id == id) return e.factory();
        throw std::domain_error("Invalid transformation ID");
    }

    /**
     * @brief Returns unique_ptr of a transformation created by an index in transformation_registry.def
     * @param index
     * @return std::unique_ptr<series_acceleration<T, K>>
     */
    static std::unique_ptr<series_acceleration<T, K>> create_by_index(size_t index) {
        const auto& entries = get_entries();
        if (index >= entries.size()) throw std::out_of_range("Transformation index out of range");
        return entries[index].factory();
    }
};

struct transformation_registry_metadata {
    /**
     * @brief Get the vector of transformation names
     * @return std::vector<std::string>
     */
    static std::vector<std::string> get_names() {
        return {
#define TRANSFORMATION_ENTRY(id, name, camel, cls, binding, ...) name,
#include "transformation_registry.def"
#undef TRANSFORMATION_ENTRY
        };
    }

    /**
     * @brief Get the vector of transformation ids
     * @return std::vector<transformation_id_t>
     */
    static std::vector<transformation_id_t> get_ids() {
        return {
#define TRANSFORMATION_ENTRY(id, name, camel, cls, binding, ...) transformation_id_t::id,
#include "transformation_registry.def"
#undef TRANSFORMATION_ENTRY
        };
    }
};

}  // namespace algos
}  // namespace shanks

#endif