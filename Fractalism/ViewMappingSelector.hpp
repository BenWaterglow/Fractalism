#ifndef _VIEW_MAPPING_SELECTOR_HPP_
#define _VIEW_MAPPING_SELECTOR_HPP_

#include "WxIncludeHelper.hpp"

#include "CLTypes.hpp"
#include "Options.hpp"

class ViewMappingSelector : public wxPanel {
public:
  ViewMappingSelector(wxWindow* parent, wxString label, cl_char3& mapping, bool& dirty);
  ~ViewMappingSelector();
  void setDimensions(options::Dimensions dimensions);
  void setNumberSystem(options::NumberSystem numberSystem);
private:
  cl_char3& mapping;
  template <class T>
  struct XYZHolder {
    T x;
    T y;
    T z;
  };
  XYZHolder<wxCheckBox *> inverted;
  XYZHolder<wxStaticText *> labels;
  XYZHolder<wxChoice *> choices;
};

#endif