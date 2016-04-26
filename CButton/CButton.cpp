#include "CButton.h"

//----------------------------------------
// constructor
//----------------------------------------
CButton::CButton (int  i_DI,
                  int  i_iDebounce,
                  bool i_bInstantSet,
                  bool i_bInstantReset)
{
  m_DI            = i_DI;
  m_iDebounce     = i_iDebounce;
  m_bInstantSet   = i_bInstantSet;
  m_bInstantReset = i_bInstantReset;

  m_bState        = false;
  m_bLastState    = false;
  m_iCount        = 0;

  pinMode (i_DI, INPUT);
}

//----------------------------------------
// Read
//----------------------------------------
bool CButton::Read ()
{
  bool bState = digitalRead (m_DI);
  if (bState == m_bLastState)
  {
    if (m_iCount < m_iDebounce)
      m_iCount++;
  }
  else
    m_iCount = 0;
  m_bLastState = bState;

  if (m_iCount >= m_iDebounce)
    m_bState = bState;
  if (m_bInstantSet && bState)
    m_bState = bState;
  if (m_bInstantReset && !bState)
    m_bState = bState;
  return m_bState;
}

//----------------------------------------
// Reset
//----------------------------------------
void CButton::Reset ()
{
  m_bState     = LOW;
  m_bLastState = LOW;
  m_iCount     = 0;
}
