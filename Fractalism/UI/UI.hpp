#ifndef _FRACTALISM_UI_HPP_
#define _FRACTALISM_UI_HPP_

#include <mutex>

#include <Fractalism/UI/WxIncludeHelper.hpp>

namespace fractalism {
  namespace ui {
    class UI : public wxFrame {
    public:
      UI();
    private:
      void updateFps();
      std::once_flag setupGpuFlag;
      wxLongLong lastRenderMillis;
      double fps;
    };
  }
}
#endif