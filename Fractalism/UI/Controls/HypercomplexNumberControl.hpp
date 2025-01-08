#ifndef _FRACTALISM_NUMBER_INPUT_CONTROL_HPP_
#define _FRACTALISM_NUMBER_INPUT_CONTROL_HPP_

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/UI/UICommon.hpp>

namespace fractalism::ui::controls {

/**
 * @class HypercomplexNumberControl
 * @brief A control for inputting hypercomplex numbers.
 */
class HypercomplexNumberControl : public wxControl {
private:
  /**
   * @class TextControl
   * @brief A text control for inputting a component of a hypercomplex number.
   */
  class TextControl : public wxTextCtrl {
  public:
    /**
     * @brief Constructs a TextControl.
     * @param parent The parent window.
     * @param i The index of the component.
     * @param number The hypercomplex number.
     */
    TextControl(
        wxWindow& parent,
        size_t i,
        gpu::types::Number& number);
  };

  /**
   * @class Label
   * @brief A label for displaying the name of a component of a hypercomplex
   * number.
   */
  class Label : public wxStaticText {
  public:
    /**
     * @brief Constructs a Label.
     * @param parent The parent control.
     * @param i The index of the component.
     * @param number The hypercomplex number.
     */
    Label(
        HypercomplexNumberControl& parent,
        size_t i,
        gpu::types::Number& number);
  };

public:
  /**
   * @brief Constructs a HypercomplexNumberControl.
   * @param parent The parent window.
   * @param label The label for the control.
   * @param number The hypercomplex number.
   */
  HypercomplexNumberControl(
      wxWindow& parent,
      wxString label,
      gpu::types::Number& number);

  /**
   * @brief Updates the displayed number.
   */
  void updateNumber();

  /**
   * @brief Updates the number system.
   */
  void updateNumberSystem();

private:
  gpu::types::Number& number;                     ///< The hypercomplex number.
  TextControl* controls[MAX_NUMBER_SYSTEM_SIZE];  ///< Array of text controls for the components.
  Label* labels[MAX_NUMBER_SYSTEM_SIZE];          ///< Array of labels for the components.
};
} // namespace fractalism::ui::controls

#endif // end include guard
