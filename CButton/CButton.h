#ifndef CButton_h
#define CButton_h

#include "Arduino.h"

class CButton
{
public:
  //----------------------------------------
  CButton (int  i_DI,
           int  i_iDebounce,
           bool i_bInstantSet,
           bool i_bInstantReset);
  //----------------------------------------------------------------------------
  // Read
  //----------------------------------------------------------------------------
  bool Read ();

  //----------------------------------------------------------------------------
  // Reset
  //----------------------------------------------------------------------------
  void Reset ();

private:
  int  m_DI;
  int  m_iDebounce;
  bool m_bInstantSet;
  bool m_bInstantReset;

  bool m_bState;
  bool m_bLastState;
  int  m_iCount;
}
;

#endif
