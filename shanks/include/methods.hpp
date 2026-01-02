#ifndef METHODS_HPP
#define METHODS_HPP
#pragma once

/**
 * @file methods.hpp
 * @brief This file aggregates all available transformation algorithms and defines their IDs.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks{ namespace algos{

/**
 * @brief Enumeration of all supported sequence transformation IDs.
 * 
 * Each ID corresponds to a specific implementation or variant of a sequence acceleration algorithm.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
enum class transformation_id_t {
	        		     null_transformation_id,
             anderson_acceleration_algorithm_id,
              brezinski_theta_transformation_id,
                   chang_wynn_transformation_id,
                 drummond_d_u_transformation_id,
                 drummond_d_t_transformation_id,
            drummond_d_t_wave_transformation_id,
                 drummond_d_v_transformation_id,
            drummond_d_v_wave_transformation_id,
                            j_transformation_id,
       recurrent_drummond_d_u_transformation_id,
       recurrent_drummond_d_t_transformation_id,
       recurrent_drummond_d_v_transformation_id,
  recurrent_drummond_d_t_wave_transformation_id,
  recurrent_drummond_d_v_wave_transformation_id,
                  ford_sidi_2_transformation_id,
                  ford_sidi_3_transformation_id,
               levin_sidi_l_u_transformation_id,
               levin_sidi_l_t_transformation_id,
          levin_sidi_l_t_wave_transformation_id,
               levin_sidi_l_v_transformation_id,
          levin_sidi_l_v_wave_transformation_id,
     recurrent_levin_sidi_l_u_transformation_id,
     recurrent_levin_sidi_l_t_transformation_id,
     recurrent_levin_sidi_l_v_transformation_id,
recurrent_levin_sidi_l_t_wave_transformation_id,
recurrent_levin_sidi_l_v_wave_transformation_id,
               levin_sidi_m_u_transformation_id,
               levin_sidi_m_t_transformation_id,
          levin_sidi_m_t_wave_transformation_id,
               levin_sidi_m_v_transformation_id,
          levin_sidi_m_v_wave_transformation_id,
               levin_sidi_s_u_transformation_id,
               levin_sidi_s_t_transformation_id,
          levin_sidi_s_t_wave_transformation_id,
               levin_sidi_s_v_transformation_id,
          levin_sidi_s_v_wave_transformation_id,
     recurrent_levin_sidi_s_u_transformation_id,
     recurrent_levin_sidi_s_t_transformation_id,
     recurrent_levin_sidi_s_v_transformation_id,
recurrent_levin_sidi_s_t_wave_transformation_id,
recurrent_levin_sidi_s_v_wave_transformation_id,
                     lubkin_w_transformation_id,
                   richardson_transformation_id,
                       shanks_transformation_id,
           shanks_alternating_transformation_id,
                      weniger_transformation_id,
               wynn_epsilon_1_transformation_id,
               wynn_epsilon_2_transformation_id,
               wynn_epsilon_3_transformation_id,
                 wynn_rho_rho_transformation_id,
         wynn_rho_generalized_transformation_id,
           wynn_rho_gamma_rho_transformation_id,
                      transformation_id_t_count,
};

} //namespace shanks::algos
} //namespace shanks

// Selection of transformation algorithms for inclusion
#include "transformations/brezinski_theta_algorithm.hpp"
#include "transformations/chang_wynn_algorithm.hpp"
#include "transformations/drummond_d_algorithm.hpp"
#include "transformations/ford_sidi_2_algorithm.hpp"
#include "transformations/ford_sidi_3_algorithm.hpp"
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
#include "transformations/anderson_acceleration_algorithm.hpp"
#include "transformations/j_transformation_algorithm.hpp"

#endif