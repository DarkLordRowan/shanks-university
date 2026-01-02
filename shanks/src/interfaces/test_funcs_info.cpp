#include "../../include/interfaces/test_funcs_info.hpp"

/**
 * @file test_funcs_info.cpp
 * @brief This file contains the information about all testing functions of application.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */


ITestFunctionInfo::~ITestFunctionInfo() {}

test_function_id_t      CmpSumAndTransformInfo::getId() const { return test_function_id_t::cmp_sum_and_transform_id ;}
test_function_id_t       CmpANAndTransformInfo::getId() const { return test_function_id_t::cmp_a_n_and_transform_id ;}
test_function_id_t TransformationRemainderInfo::getId() const { return test_function_id_t::transformation_remainder_id ;}
test_function_id_t      CmpTransformationsInfo::getId() const { return test_function_id_t::cmp_transformations_id ;}
test_function_id_t       EvalTransformTimeInfo::getId() const { return test_function_id_t::eval_transform_time_id ;}
test_function_id_t       TestAllTransformsInfo::getId() const { return test_function_id_t::test_all_transforms_id ;}

std::string      CmpSumAndTransformInfo::getName() const { return "cmp_sum_and_transform";}
std::string       CmpANAndTransformInfo::getName() const { return "cmp_a_n_and_transform";}
std::string TransformationRemainderInfo::getName() const { return "transformation_remainder";}
std::string      CmpTransformationsInfo::getName() const { return "cmp_transformations";}
std::string       EvalTransformTimeInfo::getName() const { return "eval_transform_time";}
std::string       TestAllTransformsInfo::getName() const { return "test_all_transforms";}

std::string      CmpSumAndTransformInfo::getDescription() const { return "showcases the difference between the transformed partial sum and the nontransformed one";}
std::string       CmpANAndTransformInfo::getDescription() const { return "showcases the difference between series' terms and transformed ones";}
std::string TransformationRemainderInfo::getDescription() const { return "showcases the difference between series' sum and transformed partial sum";}
std::string      CmpTransformationsInfo::getDescription() const { return "showcases the difference between convergence of sums accelerated by different transformations";}
std::string       EvalTransformTimeInfo::getDescription() const { return "evaluates the time it takes to transform series";}
std::string       TestAllTransformsInfo::getDescription() const { return "test all algorithms on sum";}