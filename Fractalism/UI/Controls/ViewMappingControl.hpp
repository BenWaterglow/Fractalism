#ifndef _FRACTALISM_VIEW_MAPPING_SELECTOR_HPP_
#define _FRACTALISM_VIEW_MAPPING_SELECTOR_HPP_

#include <Fractalism/UI/UICommon.hpp>

#include <Fractalism/GPU/Types.hpp>

namespace fractalism::ui::controls {
  class ViewMappingControl : public wxControl {
  public:
    ViewMappingControl(wxWindow& parent, wxString label, gpu::types::ViewMapping& mapping);
    void updateViewMapping();
    void updateNumberSystem();
    void updateRenderDimensions();
  private:
    template <cl_char gpu::types::ViewMapping::*index>
    class Row : public wxBoxSizer {
    public:
      Row(ViewMappingControl& parent, wxString label, gpu::types::ViewMapping& mapping);
      void updateNumberSystem();
      void updateViewMappingElement();
    private:
      wxCheckBox& inverted;
      wxChoice& choice;
      gpu::types::ViewMapping& mapping;

      void onInvert(wxCommandEvent& evt);
      void onMappingChanged(wxCommandEvent& evt);
    };

    Row<&gpu::types::ViewMapping::x>& x;
    Row<&gpu::types::ViewMapping::y>& y;
    Row<&gpu::types::ViewMapping::z>& z;
  };
}
#endif