#ifndef TEST_FUNCS_INFO_INTERFACES_HPP
#define TEST_FUNCS_INFO_INTERFACES_HPP

#include <string>
#include <vector>
#include <memory>


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
#define TEST_FUNCTION_ENTRY(id, name, desc) id,
#include "../test_function_registry.def"
#undef TEST_FUNCTION_ENTRY
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
	virtual ~ITestFunctionInfo() = default;

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
 * @brief Generic implementation of ITestFunctionInfo using metadata from registry
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class DeclarativeTestFunctionInfo final : public ITestFunctionInfo {
public:
    DeclarativeTestFunctionInfo(test_function_id_t id, std::string name, std::string description)
        : id_(id), name_(std::move(name)), description_(std::move(description)) {}

    test_function_id_t getId() const override { return id_; }
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }

private:
    test_function_id_t id_;
    std::string name_;
    std::string description_;
};

struct test_function_registry_metadata {
    static std::vector<std::string> get_names() {
        return {
#define TEST_FUNCTION_ENTRY(id, name, desc) name,
#include "../test_function_registry.def"
#undef TEST_FUNCTION_ENTRY
        };
    }

    static std::vector<std::string> get_descriptions() {
        return {
#define TEST_FUNCTION_ENTRY(id, name, desc) desc,
#include "../test_function_registry.def"
#undef TEST_FUNCTION_ENTRY
        };
    }

    static std::vector<test_function_id_t> get_ids() {
        return {
#define TEST_FUNCTION_ENTRY(id, name, desc) test_function_id_t::id,
#include "../test_function_registry.def"
#undef TEST_FUNCTION_ENTRY
        };
    }
};

#endif
