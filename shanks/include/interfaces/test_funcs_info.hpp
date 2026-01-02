#ifndef TEST_FUNCS_INFO_INTERFACES_HPP
#define TEST_FUNCS_INFO_INTERFACES_HPP

#include <string>


/**
 * @file test_funcs_info.hpp
 * @brief This file contains metadata information about testing functions.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Enumeration of testing functions IDs
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
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
 * @brief Interface for accessing test function identification and descriptive metadata
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class ITestFunctionInfo {
public:

	/**
	 * @brief Virtual destructor
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 */
	virtual ~ITestFunctionInfo();

	/**
	 * @brief Returns the unique test function ID
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @return test_function_id_t
	 */
	virtual test_function_id_t getId() const = 0;

	/**
	 * @brief Returns the name of the test function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @return std::string
	 */
	virtual std::string getName() const = 0;

	/**
	 * @brief Returns a detailed description of the test function
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @return std::string
	 */
	virtual std::string getDescription() const = 0;
};

/**
 * @brief Information class for comparing partial sums and transformations
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class CmpSumAndTransformInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

/**
 * @brief Information class for comparing terms and transformations
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class CmpANAndTransformInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

/**
 * @brief Information class for transformation remainder analysis
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class TransformationRemainderInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

/**
 * @brief Information class for comparing different transformations
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class CmpTransformationsInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

/**
 * @brief Information class for evaluating transformation computation time
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class EvalTransformTimeInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

/**
 * @brief Information class for testing all algorithms on a single sum
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class TestAllTransformsInfo : public ITestFunctionInfo {
public:
	test_function_id_t getId() const override;
	std::string getName() const override;
	std::string getDescription() const override;
};

#endif