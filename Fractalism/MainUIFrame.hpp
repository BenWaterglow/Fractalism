#ifndef _MAIN_UI_FRAME_HPP_
#define _MAIN_UI_FRAME_HPP_

#include "WxIncludeHelper.hpp"
#include "SharedState.hpp"

class MainUIFrame : public wxFrame {
public:
  MainUIFrame(SharedState& sharedState);
  ~MainUIFrame();
private:
  wxLongLong lastRenderMillis;
  double fps;
};

#endif