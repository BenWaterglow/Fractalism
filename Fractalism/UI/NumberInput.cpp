#include <Fractalism/UI/NumberInput.hpp>

#include <Fractalism/App.hpp>

namespace fractalism {
  namespace ui {
    void NumberInput::updateNumberSystemElementCount() {
      int showCount = App::get<Settings>().getNumberSystemElementCount();
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
  }
}