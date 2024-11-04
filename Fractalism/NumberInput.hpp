#ifndef _MILTICOMPLEX_INPUT_HPP_
#define _MULTICOMPLEX_INPUT_HPP_

#include "WxIncludeHelper.hpp"

#include "CLTypes.hpp"
#include "Options.hpp"
#include "Events.hpp"

class NumberInput : public wxPanel {
public:
  NumberInput(wxWindow *parent, wxString label, cltypes::Number& number, bool& dirty, const wxEventTypeTag<StateChangeEvent> &eventType);
  ~NumberInput();
  void setNumberSystem(options::NumberSystem numberSystem);
private:
  static const int fieldCount = 8;
  wxTextCtrl *x[fieldCount];
  wxStaticText *labels[fieldCount];
};


#endif