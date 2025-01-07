#include <Fractalism/UI/Controls/HypercomplexNumberControl.hpp>

#include <format>
#include <wx/valnum.h>

#include <Fractalism/Utils.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/App.hpp>

namespace fractalism::ui::controls {
  namespace {
    namespace types = gpu::types;
    constexpr size_t fieldCount = MAX_NUMBER_SYSTEM_SIZE;
  }

  HypercomplexNumberControl::HypercomplexNumberControl(wxWindow& parent, wxString label, types::Number& number) :
      wxControl(&parent, wxID_ANY),
      number(number) {
    SetLabel(label);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < fieldCount; i++) {
      sizer->Add(controls[i] = new TextControl(*this, i, number));
      sizer->Add(labels[i] = new Label(*this, i, number));
    }
    SetSizer(sizer);
    updateNumber();
  }

  void HypercomplexNumberControl::updateNumber() {
    for (int i = 0; i < fieldCount; i++) {
      controls[i]->SetValue(std::format("{:.15f}", number.raw[i]));
    }
  }

  void HypercomplexNumberControl::updateNumberSystem() {
    int showCount = App::get<Settings>().getNumberSystemElementCount();
    for (int i = 0; i < fieldCount; i++) {
      controls[i]->Show(i < showCount);
      labels[i]->Show(i < showCount);
    }
    Layout();
    Fit();
  }

  HypercomplexNumberControl::TextControl::TextControl(wxWindow& parent, size_t i, types::Number& number) :
        wxTextCtrl(
          &parent,
          wxID_ANY,
          std::format("{:+.15f}", number.raw[i]),
          wxDefaultPosition,
          wxDefaultSize,
          0L,
          wxFloatingPointValidator<real>(&number.raw[i])) {
    Bind(wxEVT_TEXT, [this, &number, i](wxCommandEvent&) {
      double value;
      GetValue().ToDouble(&value);
      number.raw[i] = static_cast<real>(value);
      events::NumberChanged::fire(this, number);
    });
  }

  HypercomplexNumberControl::Label::Label(HypercomplexNumberControl& parent, size_t i, types::Number& number) :
        wxStaticText(
          &parent,
          wxID_ANY,
          std::format(L"e{} (drag to change)", utils::toSubscript(i))) {
    static wxCoord lastX;
    Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& evt) {
      CaptureMouse();
      lastX = evt.GetX();
    });
    Bind(wxEVT_LEFT_UP, [this](wxMouseEvent&) {
      if (HasCapture()) {
        ReleaseMouse();
      }
    });
    Bind(wxEVT_MOTION, [this, &parent, &number, i](wxMouseEvent& evt) {
      if (evt.LeftIsDown()) {
        real delta = static_cast<real>(evt.GetX() - lastX) / static_cast<real>(GetSize().GetWidth());
        number.raw[i] += delta;
        parent.controls[i]->SetValue(std::format("{:.15f}", number.raw[i]));
        lastX = evt.GetX();
        events::NumberChanged::fire(this, number);
      }
    });
    Bind(wxEVT_MOUSE_CAPTURE_LOST, [](wxMouseCaptureLostEvent&) {});
  }
}