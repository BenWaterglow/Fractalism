#include "WxIncludeHelper.hpp"

#include "RenderPanel.hpp"
#include "GLRenderCanvas.hpp"

static GLRenderCanvas* const createCanvas(wxWindow* parent, SharedState::RenderState& renderState) {
  wxStatusBar* statusBar = new wxStatusBar(parent, wxID_ANY, wxSTB_SHOW_TIPS | wxSTB_ELLIPSIZE_END | wxFULL_REPAINT_ON_RESIZE);
  GLRenderCanvas* const canvas = new GLRenderCanvas(parent, renderState, *statusBar);
  static const int statusWidths[] = { 100, -1, 130 };
  statusBar->SetFieldsCount(3, statusWidths);
  wxBoxSizer* canvasSizer = new wxBoxSizer(wxVERTICAL);
  canvasSizer->Add(canvas, 1, wxSHAPED);
  canvasSizer->Add(statusBar, 0, wxEXPAND);
  parent->SetSizerAndFit(canvasSizer);
  return canvas;
}

RenderPanel::RenderPanel(wxWindow* parent, SharedState::RenderState& renderState) :
    wxPanel(parent),
    canvas(createCanvas(this, renderState)) {}

RenderPanel::~RenderPanel() {}