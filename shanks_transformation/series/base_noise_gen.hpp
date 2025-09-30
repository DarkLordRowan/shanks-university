#include "../custom_concepts.hpp"

enum noise_gen_id{
    none_id,
    uniform_id
};

template<AcceptedLike T>
class BaseNoiseGenerator {
protected:

    unsigned long long int seed;

public:

    BaseNoiseGenerator() {};

    virtual T operator()(size_t index) = 0;

};