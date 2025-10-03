#pragma once

 /**
  * @brief Enum of transformation IDs
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
*/
enum transformation_id_t {
			            null_transformation_id,
	     brezinski_theta_transformation_id,
               weniger_tramsformation_id,
        wynn_epsilon_1_transforamtion_id,
        wynn_epsilon_2_transforamtion_id,
        wynn_epsilon_3_transforamtion_id,
          wynn_rho_rho_transformation_id,
  wynn_rho_generalized_transformation_id,
    wynn_rho_gamma_rho_transformation_id,
};

//INCLUDE TRANSFORMATIONS AND ALGORITHMS
#include "./methods/algorithm/brezinski_theta_algorithm.hpp"
#include "./methods/algorithm/weniger_algorithm.hpp"
#include "./methods/algorithm/wynn_epsilon_1_algorithm.hpp"
#include "./methods/algorithm/wynn_epsilon_2_algorithm.hpp"
#include "./methods/algorithm/wynn_epsilon_3_algorithm.hpp"
#include "./methods/algorithm/wynn_rho_algorithm.hpp"



