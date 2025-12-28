#ifndef TRANSFORMATIONS_INFO_INTERFACES_HPP
#define TRANSFORMATIONS_INFO_INTERFACES_HPP

#include <string>
#include "../methods.hpp"


/**
 * @brief Interface for transformation information
 */
class ITransformationInfo {
public:

	virtual ~ITransformationInfo();

	virtual shanks::algos::transformation_id_t getId() const = 0;
	virtual std::string getName() const = 0;

};

/**
 * brief Implementations for all transformations
 */

class AndersonAccelerationAlgorithmInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class BrezinskiThetaTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class ChangWynnTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class DrummondDUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class DrummondDTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class DrummondDTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class DrummondDVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class JTransformationAlgorithmInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class DrummondDVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentDrummondDUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentDrummondDTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentDrummondDVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentDrummondDTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentDrummondDVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class FordSidi2TransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class FordSidi3TransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiLUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiLTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiLTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiLVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiLVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiLUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiLTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiLVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiLTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiLVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiMUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiMTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiMTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiMVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiMVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiSUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiSTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiSTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiSVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LevinSidiSVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiSUTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiSTTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiSVTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiSTWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RecurrentLevinSidiSVWaveTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class LubkinWTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class RichardsonTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class ShanksTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class ShanksAlternatingTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WenigerTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WynnEpsilon1TransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WynnEpsilon2TransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WynnEpsilon3TransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WynnRhoRhoTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WynnRhoGeneralizedTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

class WynnRhoGammaRhoTransformationInfo : public ITransformationInfo {
public:
	shanks::algos::transformation_id_t getId() const override;
	std::string getName() const override;
};

#endif