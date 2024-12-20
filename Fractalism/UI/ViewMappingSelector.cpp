#include <Fractalism/UI/ViewMappingSelector.hpp>

#include <Fractalism/App.hpp>

namespace fractalism {
  namespace ui {

    void fractalism::ui::ViewMappingSelector::updateRenderDimensions() {
      z.ShowItems(App::get<Settings>().renderDimensions == options::Dimensions::three);
      Layout();
    }

    void ViewMappingSelector::updateNumberSystemElementCount() {
      x.updateOptions();
      y.updateOptions();
      z.updateOptions();
      Layout();
    }
  }
}