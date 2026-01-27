#ifndef TRANSFORMATIONS_INFO_INTERFACES_HPP
#define TRANSFORMATIONS_INFO_INTERFACES_HPP

#include <memory>
#include <string>
#include <vector>

#include "../methods.hpp"

/**
 * @file transformations_info.hpp
 * @brief This file contains metadata information for transformation algorithms.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Interface for accessing transformation identification and naming metadata
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
class ITransformationInfo
{
public:
    /**
     * @brief Virtual destructor
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    virtual ~ITransformationInfo() = default;

    /**
     * @brief Returns the unique transformation ID
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return shanks::algos::transformation_id_t
     */
    virtual shanks::algos::transformation_id_t getId() const = 0;

    /**
     * @brief Returns the name of the transformation
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return std::string
     */
    virtual std::string getName() const = 0;
};

/**
 * @brief Generic implementation of ITransformationInfo using metadata from registry
 */
class DeclarativeTransformationInfo final : public ITransformationInfo
{
public:
    DeclarativeTransformationInfo(shanks::algos::transformation_id_t id, std::string name)
        : id_(id), name_(std::move(name))
    {
    }

    shanks::algos::transformation_id_t getId() const override
    {
        return id_;
    }
    std::string getName() const override
    {
        return name_;
    }

private:
    shanks::algos::transformation_id_t id_;
    std::string name_;
};

#endif
