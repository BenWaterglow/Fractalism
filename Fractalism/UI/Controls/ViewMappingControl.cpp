#include <Fractalism/UI/Controls/ViewMappingControl.hpp>

#include <Fractalism/App.hpp>

namespace fractalism::ui::controls {
  namespace {
    namespace types = gpu::types;
    static const wxArrayString viewMappingOptions = []() {
      wxString options[MAX_NUMBER_SYSTEM_SIZE];
      for (size_t i = 0; i < MAX_NUMBER_SYSTEM_SIZE; i++) {
        options[i] = std::format(L"e{}", utils::toSubscript(i));
      }
      return wxArrayString(MAX_NUMBER_SYSTEM_SIZE, &options[0]);
    }();
  }

  template <cl_char types::ViewMapping::*index>
  ViewMappingControl::Row<index>::Row(ViewMappingControl& parent, wxString label, types::ViewMapping& mapping) :
    wxBoxSizer(wxHORIZONTAL),
    mapping(mapping),
    inverted(*new wxCheckBox(&parent, wxID_ANY, "inverted?", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT)),
    choice(*new wxChoice(&parent, wxID_ANY)) {

    Add(new wxStaticText(&parent, wxID_ANY, label));
    Add(&choice);
    Add(&inverted);

    inverted.Bind(wxEVT_CHECKBOX, &Row::onInvert, this);
    choice.Bind(wxEVT_CHOICE, &Row::onMappingChanged, this);

    updateNumberSystem();
    updateViewMappingElement();
  }

  template <cl_char types::ViewMapping::*index>
  void ViewMappingControl::Row<index>::updateNumberSystem() {
    choice.Set(App::get<Settings>().getNumberSystemElementCount(), &viewMappingOptions[0]);
    choice.SetSelection(std::abs(mapping.*index) - 1);
  }

  template <cl_char types::ViewMapping::*index>
  void ViewMappingControl::Row<index>::updateViewMappingElement() {
    choice.SetSelection(std::abs(mapping.*index) - 1);
    inverted.SetValue(mapping.*index < 0);
  }

  template <cl_char types::ViewMapping::*index>
  void ViewMappingControl::Row<index>::onInvert(wxCommandEvent& evt) {
    // While it's unlikely that the sign could be changed elsewhere at this
    // point, it seems prudent to avoid possible future bugs. So we do this
    // instead of `mapping.*index = -mapping.*index;`.
    mapping.*index = std::abs(mapping.*index) * evt.IsChecked() ? -1 : 1;
    events::ViewMappingChanged::fire(this->GetContainingWindow(), mapping);
  }

  template <cl_char types::ViewMapping::*index>
  void ViewMappingControl::Row<index>::onMappingChanged(wxCommandEvent& evt) {
    mapping.*index = (1 + static_cast<cl_char>(evt.GetInt())) * cl_char(inverted.GetValue() ? -1 : 1);
    events::ViewMappingChanged::fire(this->GetContainingWindow(), mapping);
  }

  ViewMappingControl::ViewMappingControl(wxWindow & parent, types::ViewMapping & mapping) :
        wxControl(&parent, wxID_ANY),
        x(*new Row<&types::ViewMapping::x>(*this, "x=>", mapping)),
        y(*new Row<&types::ViewMapping::y>(*this, "y=>", mapping)),
        z(*new Row<&types::ViewMapping::z>(*this, "z=>", mapping)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    for (wxBoxSizer* row : std::initializer_list<wxBoxSizer*>{ &x, &y, &z }) {
      sizer->Add(row);
    }
    SetSizer(sizer);
    updateNumberSystem();
    updateViewMapping();
    updateRenderDimensions();
  }

  void ViewMappingControl::updateNumberSystem() {
    x.updateNumberSystem();
    y.updateNumberSystem();
    z.updateNumberSystem();
  }

  void ViewMappingControl::updateViewMapping() {
    x.updateViewMappingElement();
    y.updateViewMappingElement();
    z.updateViewMappingElement();
  }

  void ViewMappingControl::updateRenderDimensions() {
    z.ShowItems(App::get<Settings>().renderDimensions == options::Dimensions::three);
    Layout();
    Fit();
  }
}