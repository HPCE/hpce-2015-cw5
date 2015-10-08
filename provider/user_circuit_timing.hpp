#ifndef user_circuit_timing_hpp
#define user_circuit_timing_hpp

#include "puzzler/puzzles/circuit_timing.hpp"


class CircuitTimingProvider
  : public puzzler::CircuitTimingPuzzle
{
public:
  CircuitTimingProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::CircuitTimingInput *input,
				puzzler::CircuitTimingOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
