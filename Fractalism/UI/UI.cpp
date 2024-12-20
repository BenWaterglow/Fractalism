#include <cassert>
#include <format>
#include <Fractalism/UI/UI.hpp>
#include <Fractalism/App.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/UI/IDs.hpp>
#include <Fractalism/UI/MenuBar.hpp>
#include <Fractalism/UI/NumberInput.hpp>
#include <Fractalism/UI/RenderPanel.hpp>
#include <Fractalism/UI/ViewMappingSelector.hpp>
#include <Fractalism/Utils.hpp>
#include <Fractalism/Proxy.hpp>

namespace fractalism {
  namespace ui {
    namespace {
      using GLRenderer = gpu::opengl::GLRenderer;
      using CLSolver = gpu::opencl::CLSolver;
    }

    UI::UI() :
        wxFrame(nullptr, wxID_ANY, "Fractalism"),
        lastRenderMillis(wxGetUTCTimeMillis()),
        fps(0.0) {
      Settings& settings = App::get<Settings>();
      SetMenuBar(new MenuBar());

      CreateStatusBar(5);

      // Main sizer to manage primary layout
      wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

      // Create the rendering canvases
      RenderPanel& phasePanel = *new RenderPanel(*this, options::Space::phase);
      phasePanel.Show(settings.phaseEnabled());
      mainSizer->Add(&phasePanel, 1, wxEXPAND);
      RenderPanel& dynamicalPanel = *new RenderPanel(*this, options::Space::dynamical);
      dynamicalPanel.Show(settings.dynamicalEnabled());
      mainSizer->Add(&dynamicalPanel, 1, wxEXPAND);

      NumberInput& parameter = *new NumberInput(
        *this,
        "Parameter",
        settings.parameter,
        EVT_ParameterChanged);

      NumberInput& phaseView = *new NumberInput(
        *this,
        "Phase",
        settings.phaseView,
        EVT_PhaseViewChanged);

      NumberInput& dynamicalView = *new NumberInput(
        *this,
        "Dynamical",
        settings.dynamicalView,
        EVT_DynamicalViewChanged);

      ViewMappingSelector& phaseViewMapping = *new ViewMappingSelector(*this, "Phase View Mapping", settings.phaseView);
      ViewMappingSelector& dynamicalViewMapping = *new ViewMappingSelector(*this, "Dynamical View Mapping", settings.dynamicalView);

      wxButton& centerParameterPhase = *new wxButton(this, wxID_ANY, "Center C (Phase)");
      centerParameterPhase.Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
        App::get<Settings>().phaseView = App::get<Settings>().parameter;
      });
      wxButton& centerParameterDynamical = *new wxButton(this, wxID_ANY, "Center C (Dymanical)");
      centerParameterDynamical.Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
        App::get<Settings>().dynamicalView = App::get<Settings>().parameter;
      });
      wxButton& screenshotPhase = *new wxButton(this, wxID_ANY, "Save Image (Phase)");
      screenshotPhase.Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
        // TODO
      });
      wxButton& screenshotDynamical = *new wxButton(this, wxID_ANY, "Save Image (Dymanical)");
      screenshotDynamical.Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
        // TODO
      });

      // Main control panel
      {
        wxBoxSizer* controlPanelSizer = new wxBoxSizer(wxVERTICAL);
        // Parameter and view controls
        {
          wxBoxSizer* numberInputSizer = new wxBoxSizer(wxHORIZONTAL);
          numberInputSizer->Add(&parameter);
          numberInputSizer->Add(&phaseView);
          numberInputSizer->Add(&dynamicalView);
          controlPanelSizer->Add(numberInputSizer);
        }
        // View mapping slection
        {
          wxBoxSizer* viewMappingSizer = new wxBoxSizer(wxHORIZONTAL);
          viewMappingSizer->Add(&phaseViewMapping);
          viewMappingSizer->Add(&dynamicalViewMapping);
          controlPanelSizer->Add(viewMappingSizer);
        }
        // Center parameter buttons
        {
          wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
          buttonSizer->Add(&centerParameterPhase);
          buttonSizer->Add(&centerParameterDynamical);
          controlPanelSizer->Add(buttonSizer);
        }
        // Save image buttons
        {
          wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
          buttonSizer->Add(&screenshotPhase);
          buttonSizer->Add(&screenshotDynamical);
          controlPanelSizer->Add(buttonSizer);
        }
        // Space and dimensions
        {
          wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
          // Space
          {
            wxString labels[] = { "Phase", "Dynamical", "Both" };
            wxRadioBox* radioBox = new wxRadioBox(this, ids::SelectSpaceDisplay, "Space", wxDefaultPosition, wxDefaultSize, 3, labels);
            radioBox->SetSelection(utils::toUnderlyingType<options::Space>(settings.space) - 1);
            hSizer->Add(radioBox);
            Bind(wxEVT_RADIOBOX, [&, this](wxCommandEvent& evt) {
              Settings& settings = App::get<Settings>();
              // Add one because we treat the enum as a set of binary flags:
              // 01 => phase
              // 10 => dynamical
              // 11 => both
              settings.space = utils::fromUnderlyingType<options::Space>(evt.GetInt() + 1);
              bool phaseEnabled = settings.phaseEnabled();
              bool dynamicalEnabled = settings.dynamicalEnabled();

              phasePanel.Show(phaseEnabled);
              centerParameterPhase.Enable(phaseEnabled);
              phaseView.Enable(phaseEnabled);
              phaseViewMapping.Enable(phaseEnabled);
              screenshotPhase.Enable(phaseEnabled);

              dynamicalPanel.Show(dynamicalEnabled);
              centerParameterDynamical.Enable(dynamicalEnabled);
              dynamicalView.Enable(dynamicalEnabled);
              dynamicalViewMapping.Enable(dynamicalEnabled);
              screenshotDynamical.Enable(dynamicalEnabled);

              this->Layout();
            }, ids::SelectSpaceDisplay);
          }
          // Dimensions
          {
            wxString labels[] = { "2D", "3D" };
            wxRadioBox* radioBox = new wxRadioBox(this, ids::SelectRenderDimensions, "2D/3D", wxDefaultPosition, wxDefaultSize, 2, labels);
            radioBox->SetSelection(utils::toUnderlyingType<options::Dimensions>(settings.renderDimensions));
            Bind(wxEVT_RADIOBOX, [&phaseViewMapping, &dynamicalViewMapping](wxCommandEvent& evt) {
              App::get<Settings>().renderDimensions = utils::fromUnderlyingType<options::Dimensions>(evt.GetInt());
              phaseViewMapping.updateRenderDimensions();
              dynamicalViewMapping.updateRenderDimensions();
            }, ids::SelectRenderDimensions);
            hSizer->Add(radioBox);
          }
          controlPanelSizer->Add(hSizer);
        }
        // Number system
        {
          wxString labels[] = { "Complex", "Bicomplex", "Tricomplex", "Quaternion", "Octonian" };
          wxRadioBox* radioBox = new wxRadioBox(this, ids::SelectNumberSystem, "Number System", wxDefaultPosition, wxDefaultSize, 5, labels);
          radioBox->SetSelection(utils::toUnderlyingType<options::NumberSystem>(settings.numberSystem));
          Bind(wxEVT_RADIOBOX, [&parameter, &phaseView, &phaseViewMapping, &dynamicalView, &dynamicalViewMapping, this](wxCommandEvent& evt) {
            Settings& settings = App::get<Settings>();
            options::NumberSystem numberSystem = utils::fromUnderlyingType<options::NumberSystem>(evt.GetInt());
            settings.numberSystem = numberSystem;
            parameter.updateNumberSystemElementCount();
            phaseView.updateNumberSystemElementCount();
            dynamicalView.updateNumberSystemElementCount();
            phaseViewMapping.updateNumberSystemElementCount();
            dynamicalViewMapping.updateNumberSystemElementCount();
            Layout();
          }, ids::SelectNumberSystem);
          controlPanelSizer->Add(radioBox);
        }
        // Resolution
        {
          controlPanelSizer->Add(new wxStaticText(this, wxID_ANY, "Render Resolution"));
          wxSlider* slider = new wxSlider(this, ids::ResolutionSlider, settings.resolution.unwrapValue<int>() / 64, 1, 20);
          controlPanelSizer->Add(slider, 0, wxEXPAND);
          Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
            cl_uint resolution = static_cast<cl_uint>(evt.GetInt()) * 64;
            App::get<Settings>().resolution = resolution;
            SetStatusText(std::format("Resolution: {}", resolution), 3);
          }, ids::ResolutionSlider);
        }
        // Iteration modifier
        {
          controlPanelSizer->Add(new wxStaticText(this, wxID_ANY, "Iteration Modifier"));
          wxSlider* slider = new wxSlider(this, ids::IterationModifier, settings.iterationModifier.unwrapValue() / 5, 1, 100);
          controlPanelSizer->Add(slider, 0, wxEXPAND);
          Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
            App::get<Settings>().iterationModifier = evt.GetInt() * 5.0;
            SetStatusText(std::format("Iteration Modifier: {:.2f}", App::get<Settings>().iterationModifier.unwrapValue()), 1);
          }, ids::IterationModifier);
        }
        // Iterations per frame
        {
          controlPanelSizer->Add(new wxStaticText(this, wxID_ANY, "Iterations Per Frame"));
          wxSlider* slider = new wxSlider(this, ids::IterationsPerFrame, settings.iterationsPerFrame.unwrapValue<int>(), 1, 1000);
          controlPanelSizer->Add(slider, 0, wxEXPAND);
          Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
            cl_uint iterationsPerFrame = evt.GetInt();
            App::get<Settings>().iterationsPerFrame = iterationsPerFrame;
            SetStatusText(std::format("Iterations Per Frame: {}", iterationsPerFrame), 2);
          }, ids::IterationsPerFrame);
        }
        mainSizer->Add(controlPanelSizer);
      }

      SetSizerAndFit(mainSizer);
      SetSize(wxSize(800, 600));
      SetStatusText("Fractalism");
      SetStatusText(std::format("Iteration Modifier: {:.2f}", settings.iterationModifier.unwrapValue()), 1);
      SetStatusText(std::format("Iterations Per Frame: {}", settings.iterationsPerFrame.unwrapValue()), 2);
      SetStatusText(std::format("Resolution: {}", settings.resolution.unwrapValue()), 3);

      // Bind events to be processed
      Bind(wxEVT_IDLE, [&phasePanel, &dynamicalPanel, this](wxIdleEvent& evt) {
        try {
          std::call_once(setupGpuFlag, [](wxGLCanvas& phaseCanvas, wxGLCanvas& dynamicalCanvas) {
            App& app = App::get<App>();
            if (App::get<Settings>().phaseEnabled()) {
              app.setupGPUContext(phaseCanvas);
            }
            else if (App::get<Settings>().dynamicalEnabled()) {
              app.setupGPUContext(dynamicalCanvas);
            }
            else {
              assert(("No view canvas is enabled. Cannot prepare GPU context without an active OpenGL canvas.", false));
            }
            }, phasePanel, dynamicalPanel);

          GLRenderer& renderer = App::get<GLRenderer>();
          CLSolver::Result& textures = App::get<App>().executeSolver();

          Settings& settings = App::get<Settings>();
          if (settings.phaseEnabled()) {
            renderer.render(phasePanel, textures.phase);
          }
          if (settings.dynamicalEnabled()) {
            renderer.render(dynamicalPanel, textures.dynamical);
          }
        }
        catch (const std::exception& e) {
          wxLogFatalError(e.what());
          wxSafeShowMessage("Error", e.what());
        }
        updateFps();
        evt.RequestMore();
      });
    }

    void UI::updateFps() {
      wxLongLong now = wxGetUTCTimeMillis();
      wxLongLong delta = now - lastRenderMillis;
      lastRenderMillis = now;
      const double fpsSmoothing = 0.7;
      fps = (fps * fpsSmoothing) + ((1.0 / (delta.ToDouble() / 1000.0)) * (1.0 - fpsSmoothing));
      if (isnan(fps) || isinf(fps)) {
        fps = 0.0;
      }
      SetStatusText(std::format("FPS: {:.2f}", fps), 4);
    }
  }
}