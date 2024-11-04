#include "WxIncludeHelper.hpp"
#include <wx/valnum.h>

#include "NumberInput.hpp"

NumberInput::NumberInput(wxWindow *parent, wxString label, cltypes::Number& number, bool& dirty, const wxEventTypeTag<StateChangeEvent> &eventType) : wxPanel(parent) {
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(this, wxID_ANY, label));
  for (int i = 0; i < fieldCount; i++) {
    wxTextCtrl *ctrl = new wxTextCtrl(
      this,
      wxID_ANY,
      wxString::Format("%+.15f", number.raw[i]),
      wxDefaultPosition, wxDefaultSize,
      0L,
      wxFloatingPointValidator<double>(&number.raw[i]));
    ctrl->Bind(wxEVT_TEXT, [&,i](wxCommandEvent&) {
      double value;
      ctrl->GetValue().ToDouble(&value);
      number.raw[i] = value;
      dirty = true;
    });
    wxBoxSizer *viewMappingSizer = new wxBoxSizer(wxHORIZONTAL);
    viewMappingSizer->Add(x[i] = ctrl);
    sizer->Add(viewMappingSizer);
    static wxCoord lastX;
    wxStaticText *label = new wxStaticText(this, wxID_ANY, wxString::Format("x%d (drag to change)", i));
    label->Bind(wxEVT_LEFT_DOWN, [=](wxMouseEvent& evt) {
      label->CaptureMouse();
      lastX = evt.GetX();
    });
    label->Bind(wxEVT_LEFT_UP, [=](wxMouseEvent&) {
      if (label->HasCapture()) {
        label->ReleaseMouse();
      }
    });
    label->Bind(wxEVT_MOTION, [&,i](wxMouseEvent& evt) {
      if (evt.LeftIsDown()) {
        double delta = static_cast<double>(evt.GetX() - lastX) / static_cast<double>(GetSize().GetWidth());
        number.raw[i] += delta;
        dirty = true;
        x[i]->ChangeValue(wxString::Format("%.15f", number.raw[i]));
        lastX = evt.GetX();
      }
    });
    label->Bind(wxEVT_MOUSE_CAPTURE_LOST, [](wxMouseCaptureLostEvent&){});
    sizer->Add(labels[i] = label);
  }
  SetSizerAndFit(sizer);
  parent->Bind(eventType, [&](StateChangeEvent&) {
    for (int i = 0; i < fieldCount; i++) {
      x[i]->ChangeValue(wxString::Format("%.15f", number.raw[i]));
    }
  });
}

NumberInput::~NumberInput() {}

void NumberInput::setNumberSystem(options::NumberSystem numberSystem) {
  int showCount;
  switch (numberSystem) {
    case options::NumberSystem::c1:
      showCount = 2;
      break;
    case options::NumberSystem::c2:
      showCount = 4;
      break;
    case options::NumberSystem::c3:
      showCount = 8;
      break;
    default:
      return;
  }
  for (int i = 0; i < showCount; i++) {
    x[i]->Show(true);
    labels[i]->Show(true);
  }
  for (int i = showCount; i < fieldCount; i++) {
    x[i]->Show(false);
    labels[i]->Show(false);
  }
  Layout();
}

