#ifndef user_brute_force_hpp
#define user_brute_force_hpp

#include "puzzler/puzzles/brute_force.hpp"

class BruteForceProvider
  : public puzzler::BruteForcePuzzle
{
public:
  BruteForceProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::BruteForceInput *input,
		       puzzler::BruteForceOutput *output
		       ) const override {
    return ReferenceExecute(log, input, output);
  }

};

#endif
