#define DEBUG_COutputPWM

#include "COutputPWM.h"

//----------------------------------------
// constructor
//----------------------------------------
COutputPWM::COutputPWM (int           i_DO,
                        bool          i_bTimeBased,
                        unsigned long i_tOn,
                        unsigned long i_tTotal)
{
  m_DO         = i_DO;
  m_bTimeBased = i_bTimeBased;
  m_tOn        = i_tOn;
  m_tTotal     = i_tTotal;

  pinMode (i_DO, OUTPUT);
}

//----------------------------------------
// Write
//----------------------------------------
void COutputPWM::Write ()
{
  bool bState = false;
  if (m_bTimeBased)
  {
    unsigned long tNow = millis();
    if (tNow - m_tStart > m_tTotal)
      m_tStart = tNow;
    if (tNow - m_tStart <= m_tOn)
      bState = true;
  }
  else
  {
    if (m_tStart < m_tOn || m_tOn >= m_tTotal)
      bState = true;
    m_tStart++;
    if (m_tStart >= m_tTotal)
      m_tStart = 0;
  }
#ifdef DEBUG_COutputPWM
  Serial << F("COutputPWM::Write() current=") << m_tStart << F(" set=") << m_tOn << "/" << m_tTotal << F(" state=") << bState << endl;
#endif

  if (bState != m_bLastState)
    digitalWrite (m_DO, bState);
  m_bLastState = bState;
}

//----------------------------------------
// SetUp
//----------------------------------------
void COutputPWM::SetUp (unsigned long i_tOn,
                        unsigned long i_tTotal)
{
  m_tOn    = i_tOn;
  m_tTotal = i_tTotal;
}
