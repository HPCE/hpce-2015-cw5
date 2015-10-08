#ifndef puzzler_puzzles_brute_force_hpp
#define puzzler_puzzles_brute_force_hpp

#include <random>
#include <regex>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{
  uint64_t TEA_hash (uint64_t v, const uint32_t *k, unsigned rounds) {
    uint32_t v0=v&0xFFFFFFFFull, v1=v>>32, sum=0, i;           /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    
    uint64_t res=1234567801234567ull;
    for (i=0; i < rounds; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        res=((res << 7) ^ (res>>57)) + (v0&v1); // Non-linear, oh-noes! 
    }     /* end cycle */
    return res;
  }
  
  class BruteForceInput
    : public Puzzle::Input
  {
  public:
    uint32_t n;
    uint32_t rounds;
    std::vector<uint32_t> key; // Always 4 elements
    uint64_t cipherText;    

    BruteForceInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
      , n(scale)
    {
      std::mt19937 rng(time(0));
      
      uint64_t plainText=rng()%n;
      
      rounds=1+(unsigned)sqrt(n);
      
      key.resize(4);
      for(unsigned i=0; i<4; i++){
        key[i] = rng()%n;
      }
      
      cipherText=TEA_hash(plainText, &key[0], rounds);
    }

    BruteForceInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(n);
      conn.SendOrRecv(rounds);
      conn.SendOrRecv(key);
      conn.SendOrRecv(cipherText);
    }
  };

  class BruteForceOutput
    : public Puzzle::Output
  {
  public:
    uint64_t plainText;

    BruteForceOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    BruteForceOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

  protected:
    virtual void PersistImpl(PersistContext &conn)
    {
      conn.SendOrRecv(plainText);
    }
  public:
    virtual bool Equals(const Puzzle::Output *output) const override
    {
      auto pOutput=As<BruteForceOutput>(output);
      return plainText==pOutput->plainText;
    }

  };


  class BruteForcePuzzle
    : public PuzzleBase<BruteForceInput,BruteForceOutput>
  {
  protected:
    

    void ReferenceExecute(
			  ILog *log,
			  const BruteForceInput *pInput,
			  BruteForceOutput *pOutput
			  ) const override final
    {
      log->LogInfo("Starting search, n=%u, cipherText=%llu, rounds=%u", pInput->n, pInput->cipherText, pInput->rounds);

      std::vector<int> hit(pInput->n, 0); // Array to hold hit/miss
      
      for(unsigned i=0; i<pInput->n; i++){
        uint64_t tmp=TEA_hash(i, &pInput->key[0], pInput->rounds);
        hit[i] = tmp==pInput->cipherText;
        log->LogDebug("  encrypt(%llu) == %llu, hit=%i", (uint64_t)i, tmp, hit[i]);
      }
      
      uint32_t index=std::find(hit.begin(), hit.end(), 1)-hit.begin();
      if(index>=hit.size())
        throw std::runtime_error("Plain-text not found.");
      
      pOutput->plainText = index;
    }

  public:
    virtual std::string Name() const override
    { return "brute_force"; }

    virtual std::shared_ptr<Input> CreateInput
    (
     ILog *,
     int scale
     ) const override
    {
      std::mt19937 rnd(time(0));

      return std::make_shared<BruteForceInput>(this, scale);
    }



  };

};

#endif
