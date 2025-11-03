#ifndef TEST_FUNCS_INFO_INTERFACES_HPP
#define TEST_FUNCS_INFO_INTERFACES_HPP

#include <string>

/**
 * @brief Enum of testing functions IDs
 */
enum class test_function_id_t {
	null_test_function_id,
	cmp_sum_and_transform_id,
	cmp_a_n_and_transform_id,
	transformation_remainder_id,
	cmp_transformations_id,
	eval_transform_time_id,
	test_all_transforms_id,
	test_function_id_t_count
};

/**
 * @brief Interface for test function information
 */
class ITestFunctionInfo {
public:

	virtual ~ITestFunctionInfo();

	virtual test_function_id_t getId() const = 0;
	virtual std::string getName() const = 0;
	virtual std::string getDescription() const = 0;
};

/**
 * @brief Implementations for all test functions
 */
class CmpSumAndTransformInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

class CmpANAndTransformInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

class TransformationRemainderInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

class CmpTransformationsInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

class EvalTransformTimeInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

class TestAllTransformsInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

#endif