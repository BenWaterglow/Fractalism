#ifndef _FRACTALISM_VIEW_MAPPING_SELECTOR_HPP_
#define _FRACTALISM_VIEW_MAPPING_SELECTOR_HPP_

#include <cmath>

#include <Fractalism/UI/WxIncludeHelper.hpp>

#include <Fractalism/Proxy.hpp>
#include <Fractalism/GPU/OpenCL/CLTypes.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/App.hpp>

namespace fractalism {
  namespace ui {
    namespace {
      namespace cltypes = gpu::opencl::cltypes;
      static const wxArrayString viewMappingOptions = wxArrayString({ L"none", L"e₀", L"e₁", L"e₂", L"e₃", L"e₄", L"e₅", L"e₆", L"e₇" });
    }
    class ViewMappingSelector : public wxPanel {
    public:
      template<proxy::Proxy<cltypes::ViewMapping> Proxy>
      ViewMappingSelector(wxWindow& parent, wxString label, Proxy& proxy) :
          wxPanel(&parent),
          x(*new Row<&cltypes::ViewMapping::x>(*this, "x=>", proxy)),
          y(*new Row<&cltypes::ViewMapping::y>(*this, "y=>", proxy)),
          z(*new Row<&cltypes::ViewMapping::z>(*this, "z=>", proxy)) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(this, wxID_ANY, label));
        for (wxBoxSizer* row : std::initializer_list<wxBoxSizer*>{&x, &y, &z}) {
          sizer->Add(row);
        }
        SetSizerAndFit(sizer);
        updateRenderDimensions();
        updateNumberSystemElementCount();
      }
      inline ~ViewMappingSelector() override {};
      void updateRenderDimensions();
      void updateNumberSystemElementCount();
    private:
      template<cl_char cltypes::ViewMapping::* index>
      class Row : public wxBoxSizer {
      public:
        template<proxy::Proxy<cltypes::ViewMapping> Proxy>
        Row(ViewMappingSelector& parent, wxString label, Proxy& proxy) :
          wxBoxSizer(wxHORIZONTAL),
          mapping(proxy.unwrap<cltypes::ViewMapping>()),
          inverted(*new wxCheckBox(&parent, wxID_ANY, "inv?", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT)),
          choice(*new wxChoice(&parent, wxID_ANY)) {

          Add(&inverted);
          Add(new wxStaticText(&parent, wxID_ANY, label));
          Add(&choice);

          int currentIndexValue = mapping.*index;
          inverted.SetValue(currentIndexValue < 0);
          choice.SetSelection(abs(currentIndexValue));
          inverted.Bind(wxEVT_CHECKBOX, [&proxy](wxCommandEvent& evt) {
            cltypes::ViewMapping mapping = proxy.unwrapValue<cltypes::ViewMapping>();
            cl_char val = std::abs(mapping.*index);
            mapping.*index = evt.IsChecked() ? -val : val;
            proxy = mapping;
          });
          choice.Bind(wxEVT_CHOICE, [&proxy](wxCommandEvent& evt) {
            cl_char val = static_cast<cl_char>(evt.GetInt());
            cltypes::ViewMapping mapping = proxy.unwrapValue<cltypes::ViewMapping>();
            mapping.*index = mapping.*index < 0 ? -val : val;
            proxy = mapping;
          });
        }
        void updateOptions() {
          size_t optionCount = App::get<Settings>().getNumberSystemElementCount() + 1;
          choice.Set(optionCount, &viewMappingOptions[0]);
          choice.SetSelection(std::abs(mapping.*index));
          inverted.SetValue(mapping.*index < 0);
        }
      private:
        wxCheckBox& inverted;
        wxChoice& choice;
        cltypes::ViewMapping& mapping;
      };
      Row<&cltypes::ViewMapping::x>& x;
      Row<&cltypes::ViewMapping::y>& y;
      Row<&cltypes::ViewMapping::z>& z;
    };
  }
}
#endif