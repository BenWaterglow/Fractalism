#ifndef _FRACTALISM_NUMBER_INPUT_CONTROL_HPP_
#define _FRACTALISM_NUMBER_INPUT_CONTROL_HPP_

#include <Fractalism/UI/UICommon.hpp>

#include <Fractalism/GPU/Types.hpp>

namespace fractalism::ui::controls {
  class HypercomplexNumberControl : public wxControl {
  private:
    class TextControl : public wxTextCtrl {
    public:
      TextControl(wxWindow &parent, size_t i, gpu::types::Number &number);
    };
    class Label : public wxStaticText {
    public:
      Label(HypercomplexNumberControl &parent, size_t i, gpu::types::Number &number);
    };
  public:
    HypercomplexNumberControl(wxWindow& parent, wxString label, gpu::types::Number& number);
    void updateNumber();
    void updateNumberSystem();
  private:
    gpu::types::Number &number;
    TextControl* controls[MAX_NUMBER_SYSTEM_SIZE];
    Label* labels[MAX_NUMBER_SYSTEM_SIZE];
  };
}
#endif // end include guard