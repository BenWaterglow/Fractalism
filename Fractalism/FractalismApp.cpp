#include "FractalismApp.hpp"
#include "MainUIFrame.hpp"
#include "SharedState.hpp"

wxIMPLEMENT_APP(FractalismApp);

FractalismApp::FractalismApp() : sharedState() {}

FractalismApp::~FractalismApp() {}

bool FractalismApp::OnInit() {
  (new MainUIFrame(sharedState))->Show(true);
  return true;
}
