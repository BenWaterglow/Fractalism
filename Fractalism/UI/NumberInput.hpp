#ifndef _FRACTALISM_MILTICOMPLEX_INPUT_HPP_
#define _FRACTALISM_MULTICOMPLEX_INPUT_HPP_

#include <format>

#include <Fractalism/UI/WxIncludeHelper.hpp>
#include <wx/valnum.h>

#include <Fractalism/GPU/OpenCL/CLTypes.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/Proxy.hpp>

namespace fractalism {
  namespace ui {
    namespace {
      namespace cltypes = gpu::opencl::cltypes;
    }
    class NumberInput : public wxPanel {
    public:
      template<proxy::Proxy<cltypes::Number> Proxy>
      NumberInput(wxWindow& parent, wxString label, Proxy& proxy, const wxEventTypeTag<StateChangeEvent>& eventType) : wxPanel(&parent) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(this, wxID_ANY, label));
        for (int i = 0; i < fieldCount; i++) {
          wxBoxSizer* viewMappingSizer = new wxBoxSizer(wxHORIZONTAL);
          viewMappingSizer->Add(x[i] = new TextControl(*this, i, proxy));
          sizer->Add(viewMappingSizer);
          sizer->Add(labels[i] = new Label(*this, i, proxy));
        }
        SetSizerAndFit(sizer);
        parent.Bind(eventType, [&](StateChangeEvent&) {
          for (int i = 0; i < fieldCount; i++) {
            x[i]->ChangeValue(std::format("{:.15f}", proxy.unwrap<cltypes::Number>().raw[i]));
          }
        });
        updateNumberSystemElementCount();
      }
      void updateNumberSystemElementCount();
    private:
      class TextControl;
      class Label;
      static const int fieldCount = 8;
      wxTextCtrl* x[fieldCount];
      wxStaticText* labels[fieldCount];
    };

    class NumberInput::TextControl : public wxTextCtrl {
    public:
      template<proxy::Proxy<cltypes::Number> Proxy>
      TextControl(wxWindow& parent, size_t i, Proxy& proxy) :
        wxTextCtrl(
          &parent,
          wxID_ANY,
          std::format("{:+.15f}", proxy.unwrap<cltypes::Number>().raw[i]),
          wxDefaultPosition, wxDefaultSize,
          0L,
          wxFloatingPointValidator<real>(&proxy.unwrap<cltypes::Number>().raw[i])) {
        Bind(wxEVT_TEXT, [this, &proxy, i](wxCommandEvent&) {
          double value;
          GetValue().ToDouble(&value);
          cltypes::Number number = proxy.unwrapValue<cltypes::Number>();
          number.raw[i] = static_cast<real>(value);
          proxy = number;
        });
      }
    };

    class NumberInput::Label : public wxStaticText {
    public:
      template<proxy::Proxy<cltypes::Number> Proxy>
      Label(NumberInput& parent, size_t i, Proxy& proxy) :
        wxStaticText(
          &parent,
          wxID_ANY,
          std::format(L"e{} (drag to change)", static_cast<wchar_t>(L'₀' + i))) {
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
        Bind(wxEVT_MOTION, [this, &parent, &proxy, i](wxMouseEvent& evt) {
          if (evt.LeftIsDown()) {
            real delta = static_cast<real>(evt.GetX() - lastX) / static_cast<real>(GetSize().GetWidth());
            cltypes::Number number = proxy.unwrapValue<cltypes::Number>();
            number.raw[i] += delta;
            parent.x[i]->ChangeValue(std::format("{:.15f}", number.raw[i]));
            proxy = number;
            lastX = evt.GetX();
          }
        });
        Bind(wxEVT_MOUSE_CAPTURE_LOST, [](wxMouseCaptureLostEvent&) {});
      }
    };
  }
}
#endif