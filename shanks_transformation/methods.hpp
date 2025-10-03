#pragma once

 /**
  * @brief Enum of transformation IDs
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
*/
enum transformation_id_t {
			   null_transformation_id,
	brezinski_theta_transformation_id,
};

//INCLUDE TRANSFORMATIONS AND ALGORITHMS
#include "./methods/algorithm/brezinski_theta_algorithm.hpp"
