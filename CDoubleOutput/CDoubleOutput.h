#ifndef CDoubleOutput_h
#define CDoubleOutput_h

#ifdef DEBUG_CDoubleOutput
#include "Streaming.h"
#endif

#include "Arduino.h"

class CDoubleOutput
{
public:
  //----------------------------------------------------------------------------
  CDoubleOutput (int           i_DO_A,
                 int           i_DO_B,
                 bool          i_bBothActive,
                 unsigned long i_tResetA,
                 unsigned long i_tResetB);

  //----------------------------------------------------------------------------
  // Write
  //----------------------------------------------------------------------------
  void Write (bool i_bStateA,
              bool i_bStateB,
              bool i_bOnlyIfNew);
  void Write ();

  //----------------------------------------------------------------------------
  // IsActive
  //----------------------------------------------------------------------------
  bool IsActive ();

  //----------------------------------------------------------------------------
  // SetUp
  //----------------------------------------------------------------------------
  void SetUp (unsigned long i_tResetA,
              unsigned long i_tResetB);

private:
  int           m_DO_A        = 0;
  int           m_DO_B        = 0;
  bool          m_bBothActive = false;
  unsigned long m_tResetA     = 0;
  unsigned long m_tResetB     = 0;
  unsigned long m_tStartA     = 0;
  unsigned long m_tStartB     = 0;
  bool          m_bLastStateA = LOW;
  bool          m_bLastStateB = LOW;
};

#endif
