#ifndef _FRACTALISM_APP_HPP_
#define _FRACTALISM_APP_HPP_

#include "WxIncludeHelper.hpp"

#include "SharedState.hpp"

class FractalismApp : public wxApp {
public:
  FractalismApp();
  ~FractalismApp();
  virtual bool OnInit();
private:
  SharedState sharedState;
};

wxDECLARE_APP(FractalismApp);

#endif