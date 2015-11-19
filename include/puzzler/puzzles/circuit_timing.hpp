#ifndef puzzler_puzzles_circuit_timing_hpp
#define puzzler_puzzles_circuit_timing_hpp

#include <random>
#include <sstream>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  class CircuitTimingPuzzle;
  class CircuitTimingInput;
  class CircuitTimingOutput;

  class CircuitTimingInput
    : public Puzzle::Input
  {
  public:
    uint32_t flipFlopCount, nandGateCount;
    std::vector<uint32_t> wires;

    CircuitTimingInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    CircuitTimingInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(nandGateCount).SendOrRecv(flipFlopCount);
      conn.SendOrRecv(wires);

      if(2*nandGateCount+flipFlopCount!=wires.size())
        throw std::runtime_error("CircuitTimingInput::Persist - Wrong number of wires for number of components.");
    }



  };

  class CircuitTimingOutput
    : public Puzzle::Output
  {
  public:
    std::vector<uint32_t> criticalPathHistogram;

    CircuitTimingOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    CircuitTimingOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(criticalPathHistogram);
    }

    virtual bool Equals(const Output *output) const override
    {
      auto pOutput=As<CircuitTimingOutput>(output);
      return criticalPathHistogram==pOutput->criticalPathHistogram;
    }

  };


  class CircuitTimingPuzzle
    : public PuzzleBase<CircuitTimingInput,CircuitTimingOutput>
  {
  protected:
    unsigned fanout_delay(unsigned src, const CircuitTimingInput *input) const
    {
      unsigned inputs=0;
      for(unsigned i=0; i<input->wires.size(); i++){
        if(input->wires[i]==src)
          inputs++;
      }
      return 1+(unsigned)ceil(log(inputs)/log(2.0));
    }

    unsigned delay(unsigned src, const CircuitTimingInput *input) const
    {
      unsigned inputDelay, logicDelay, outputDelay;

      if(src < input->flipFlopCount){
        inputDelay=0; // No input delay for flip-flip
        logicDelay=0; // No logic for flip-flop
        outputDelay=fanout_delay(src, input); // Need to get value of flip-flop to all places it is used
      }else{
        unsigned base=src + (src - input->flipFlopCount); // There are two inputs per nand gate

        unsigned leftDelay=delay(input->wires.at(base), input);
        unsigned rightDelay=delay(input->wires.at(base+1), input);

        inputDelay=std::max(leftDelay,rightDelay); // Input delay is worse of two inputs
        logicDelay=1; // Each gate takes 1 time unit
        outputDelay=fanout_delay(src, input); // Need to get from output of nand gate to all places it is used
      }
      return inputDelay+logicDelay+outputDelay;
    }

  protected:

    virtual void Execute(
			 ILog *log,
			 const CircuitTimingInput *input,
			 CircuitTimingOutput *output
			 ) const =0;

    void ReferenceExecute(
			  ILog *log,
			  const CircuitTimingInput *pInput,
			  CircuitTimingOutput *pOutput
			  ) const
    {
      log->LogVerbose("Starting timing analysis");

      std::map<unsigned,unsigned> histogram;

      for(unsigned i=0; i<pInput->flipFlopCount; i++){
        // BREAKING-CHANGE, originally:
        // unsigned got=delay(i, pInput);
        unsigned got=delay(pInput->wires.at(i), pInput);
        histogram[got]++;
      }

      log->LogVerbose("Finished timing analysis, converting histogram");

      // We have a map from delay->count, but this may be sparse (some
      // delays may not exist). So we convert to a dense representation
      std::vector<uint32_t> res(histogram.rbegin()->first+1, 0);
      for(unsigned i=0; i<res.size(); i++){
        if(histogram.find(i)!=histogram.end()){
          res.at(i)=histogram.at(i);
        }
        log->LogInfo(" histogram[%u] = %u", i, res[i]);
      }

      pOutput->criticalPathHistogram=res;

      log->LogVerbose("Finished converting histogram");
    }

  public:
    virtual std::string Name() const override
    { return "circuit_timing"; }

    virtual std::shared_ptr<Input> CreateInput(
					       ILog *,
					       int scale
					       ) const override
    {
      std::mt19937 rnd(time(0));  // Not the best way of seeding...

      auto params=std::make_shared<CircuitTimingInput>(this, scale);

      params->flipFlopCount=scale;
      params->nandGateCount=10*scale;

      params->wires.resize(2*params->nandGateCount+params->flipFlopCount);

      std::vector<unsigned> todo;
      std::vector<unsigned> done;

      for(unsigned i=0; i<params->flipFlopCount; i++){
        done.push_back(i);
      }
      for(unsigned i=0; i<params->nandGateCount; i++){
        todo.push_back(i+params->flipFlopCount);
      }

      while(todo.size()>0){
        unsigned idx=rnd()%todo.size();
        unsigned curr=todo[idx];
        todo.erase(todo.begin()+idx);

        unsigned src1=done.at(rnd()%done.size());
        unsigned src2=done.at(rnd()%done.size());

        unsigned base=curr+(curr-params->flipFlopCount);
        params->wires.at(base)=src1;
        params->wires.at(base+1)=src2;

        done.push_back(curr);
      }

      for(unsigned i=0; i<params->flipFlopCount; i++){
        params->wires[i]=done[rnd()%done.size()];
      }

      return params;
    }

  };

};

#endif
