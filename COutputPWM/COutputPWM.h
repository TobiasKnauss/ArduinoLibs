#ifndef COutputPWM_h
#define COutputPWM_h

#ifdef DEBUG_COutputPWM
#include "Streaming.h"
#endif

#include "Arduino.h"

class COutputPWM
{
public:
  //----------------------------------------------------------------------------
  COutputPWM (int           i_DO,
              bool          i_tTimeBased,
              unsigned long i_tOn,
              unsigned long i_tTotal);

  //----------------------------------------------------------------------------
  // Write
  //----------------------------------------------------------------------------
  void Write ();

  //----------------------------------------------------------------------------
  // SetUp
  //----------------------------------------------------------------------------
  void SetUp (unsigned long i_tOn,
              unsigned long i_tTotal);

private:
  int           m_DO     = 0;
  unsigned long m_tOn    = 0;
  unsigned long m_tTotal = 0;
  unsigned long m_tStart = 0;
  bool          m_bLastState = LOW;
  bool          m_bTimeBased = false;
};

#endif
