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
#include "./methods/shanks_transformation.hpp"
#include "./methods/epsilon_algorithm.hpp"
#include "./methods/levin_L_algorithm.hpp"
#include "./methods/epsilon_algorithm2.hpp"
#include "./methods/chang_whynn_algorithm.hpp"
#include "./methods/levin_sidi_M_algorithm.hpp"
#include "./methods/levin_sidi_S_algorithm.hpp"
#include "./methods/drummond_D_algorithm.hpp"
#include "./methods/weniger_algorithm.hpp"
#include "./methods/rho_wynn_algorithm.hpp"
#include "./methods/brezinski_theta_algorithm.hpp"
#include "./methods/epsilon_algorithm3.hpp"
#include "./methods/lubkin_W_algorithm.hpp"
#include "./methods/richardson_algorithm.hpp"
#include "./methods/FSA2.hpp"
#include "./methods/FSA3.hpp"