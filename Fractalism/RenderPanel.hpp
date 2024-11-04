#ifndef _RENDER_PANEL_HPP_
#define _RENDER_PANEL_HPP_

#include "WxIncludeHelper.hpp"
#include "SharedState.hpp"
#include "GLRenderCanvas.hpp"

class RenderPanel : public wxPanel {
public:
  RenderPanel(wxWindow* parent, SharedState::RenderState& renderState);
  ~RenderPanel();
  GLRenderCanvas* const canvas;
};

#endif