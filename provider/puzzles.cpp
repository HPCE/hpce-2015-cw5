#ifndef local_puzzles_hpp
#define local_puzzles_hpp

#include "user_circuit_timing.hpp"
#include "user_matrix_exponent.hpp"


void puzzler::PuzzleRegistrar::UserRegisterPuzzles()
{
  Register(std::make_shared<CircuitTimingProvider>());
  Register(std::make_shared<MatrixExponentProvider>());
  
}


#endif
