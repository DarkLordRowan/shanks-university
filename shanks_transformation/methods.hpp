#pragma once

 /**
  * @brief Enum of transformation IDs
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
*/
enum transformation_id_t {
			   null_transformation_id,
			 shanks_transformation_id,
          	weniger_transformation_id,
		   rho_wynn_transformation_id,
	brezinski_theta_transformation_id,
				 epsilon_algorithm_id,
			   epsilon_algorithm_2_id,
               epsilon_algorithm_3_id,
           chang_epsilon_algorithm_id,
				       L_algorithm_id,
                       S_algorithm_id,
					   D_algorithm_id,
                       M_algorithm_id,
					   W_algorithm_id,
			  richardson_algorithm_id,
		   Ford_Sidi_algorithm_two_id,
		 Ford_Sidi_algorithm_three_id,
};

//INCLUDE TRANSFORMATIONS AND ALGORITHMS
#include "./methods/wynn_numerators.hpp"
#include "./methods/remainders.hpp"
#include "./methods/algorithm/shanks_algorithm.hpp"
#include "./methods/algorithm/wynn_epsilon_1_algorithm.hpp"
#include "./methods/algorithm/levin_algorithm.hpp"
#include "./methods/algorithm/wynn_epsilon_2_algorithm.hpp"
#include "./methods/algorithm/chang_wynn_algorithm.hpp"
#include "./methods/algorithm/levin_sidi_m_algorithm.hpp"
#include "./methods/algorithm/levin_sidi_s_algorithm.hpp"
#include "./methods/algorithm/drummond_d_algorithm.hpp"
#include "./methods/algorithm/weniger_algorithm.hpp"
#include "./methods/algorithm/rho_wynn_algorithm.hpp"
#include "./methods/algorithm/brezinski_theta_algorithm.hpp"
#include "./methods/algorithm/wynn_epsilon_3_algorithm.hpp"
#include "./methods/algorithm/lubkin_w_algorithm.hpp"
#include "./methods/algorithm/richardson_algorithm.hpp"
#include "./methods/algorithm/ford_sidi_2_algorithm.hpp"
#include "./methods/algorithm/ford_sidi_3_algorithm.hpp"
