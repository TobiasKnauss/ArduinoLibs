#define DEBUG_CDoubleOutput

#include "CDoubleOutput.h"

//----------------------------------------
// constructor
//----------------------------------------
CDoubleOutput::CDoubleOutput (int           i_DO_A,
                              int           i_DO_B,
                              bool          i_bBothActive,
                              unsigned long i_tResetA,
                              unsigned long i_tResetB)
{
  m_DO_A        = i_DO_A;
  m_DO_B        = i_DO_B;
  m_bBothActive = i_bBothActive;
  m_tResetA     = i_tResetA;
  m_tResetB     = i_tResetB;

  pinMode (i_DO_A, OUTPUT);
  pinMode (i_DO_B, OUTPUT);
}

//----------------------------------------
// Write
//----------------------------------------
void CDoubleOutput::Write (bool i_bStateA,
                           bool i_bStateB,
                           bool i_bOnlyIfNew)
{
  if (i_bStateA && i_bStateB && !m_bBothActive)
    i_bStateB = LOW;

#ifdef DEBUG_CDoubleOutput
  Serial << F("CDoubleOutput::Write() A=") << i_bStateA << F("B=") << i_bStateB << endl;
#endif

  if (i_bOnlyIfNew
  &&  i_bStateA == m_bLastStateA
  &&  i_bStateB == m_bLastStateB)
    return;

  digitalWrite (m_DO_A, i_bStateA);
  digitalWrite (m_DO_B, i_bStateB);
  m_bLastStateA = i_bStateA;
  m_bLastStateB = i_bStateB;
  m_tStartA = m_tStartB = millis ();

#ifdef DEBUG_CDoubleOutput
  Serial << F("CDoubleOutput::Write() set A and B") << endl;
#endif
}

//----------------------------------------
// Write
//----------------------------------------
void CDoubleOutput::Write ()
{
  unsigned long tNow = millis ();
  if (m_tResetA > 0)
  {
    if (tNow - m_tStartA > m_tResetA)
    {
      digitalWrite (m_DO_A, LOW);
#ifdef DEBUG_CDoubleOutput
      Serial << F("CDoubleOutput::Write() reset A") << endl;
#endif
    }
  }
  if (m_tResetB > 0)
  {
    if (tNow - m_tStartB > m_tResetB)
    {
      digitalWrite (m_DO_B, LOW);
#ifdef DEBUG_CDoubleOutput
      Serial << F("CDoubleOutput::Write() reset B") << endl;
#endif
    }
  }
}

//----------------------------------------
// IsActive
//----------------------------------------
bool CDoubleOutput::IsActive()
{
  bool bStateA = digitalRead (m_DO_A);
  bool bStateB = digitalRead (m_DO_B);
  return bStateA || bStateB;
}

//----------------------------------------
// SetUp
//----------------------------------------
void CDoubleOutput::SetUp (unsigned long i_tResetA,
                           unsigned long i_tResetB)
{
  m_tResetA = i_tResetA;
  m_tResetB = i_tResetB;
}
