
#ifndef _RANDOMGEN_H_
#define _RANDOMGEN_H_

typedef boost::mt19937 RNGType;

struct shuffle : std::unary_function<unsigned, unsigned> {
    RNGType& _state;
    unsigned operator()(unsigned i) {
        boost::uniform_int<> rng(0, i - 1);
        return rng(_state);
    }
    shuffle(RNGType& state) : _state(state) {}
};

#endif // #ifndef _RANDOMGEN_H_
