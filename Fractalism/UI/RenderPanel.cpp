#include <string>

#include <Fractalism/UI/RenderPanel.hpp>
#include <Fractalism/UI/GLRenderCanvas.hpp>

namespace fractalism {
  namespace ui {
    static constexpr std::string spaceName(const options::Space space) {
      switch (space) {
      case options::Space::phase: return "Phase Space";
      case options::Space::dynamical: return "Dynamical Space";
      case options::Space::both: return "Phase and Dynamical Spaces";
      default: throw std::invalid_argument("Invalid computation space");
      }
    }

    static GLRenderCanvas& createCanvas(wxWindow& parent, options::Space space) {
      wxStatusBar* statusBar = new wxStatusBar(&parent, wxID_ANY, wxSTB_SHOW_TIPS | wxSTB_ELLIPSIZE_END | wxFULL_REPAINT_ON_RESIZE);
      statusBar->SetStatusText(spaceName(space));
      GLRenderCanvas* const canvas = new GLRenderCanvas(parent, space, *statusBar);
      static const int statusWidths[] = { 100, -1, 130 };
      statusBar->SetFieldsCount(3, statusWidths);
      wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer* contentSizer = new wxBoxSizer(wxHORIZONTAL);
      contentSizer->Add(canvas, 1, wxSHAPED);
      panelSizer->Add(contentSizer, 1, wxEXPAND);
      panelSizer->Add(statusBar, 0, wxEXPAND);
      parent.SetSizerAndFit(panelSizer);
      return *canvas;
    }

    RenderPanel::RenderPanel(wxWindow& parent, options::Space space) :
      wxPanel(&parent),
      canvas(createCanvas(*this, space)) {
    }
  }
}