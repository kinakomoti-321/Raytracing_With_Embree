#pragma once
#include <iostream>
#include <limits>
#include <memory>
#include "sampling/sampler.hpp"
// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

class RNGrandom : public Sampler {
private:
    pcg32_random_t seed;

public:
    RNGrandom() {
        seed.state = 1;
        seed.inc = 1;
    }
    RNGrandom(uint64_t inseed) {
        seed.state = inseed;
        seed.inc = 1;
    }

    void setSeed(uint64_t inseed)override {
        seed.state = inseed;
        seed.inc = 1;
    }
    float getSample() override {
        const float divider = 1.0f / std::numeric_limits<uint32_t>::max();
        return pcg32_random_r(&seed) * divider;
    }
    std::shared_ptr<Sampler> clone() const override {
        return std::make_shared<RNGrandom>();
    }
};