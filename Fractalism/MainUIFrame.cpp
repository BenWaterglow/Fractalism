#include "MainUIFrame.hpp"
#include "Options.hpp"
#include "Events.hpp"
#include "GLRenderCanvas.hpp"
#include "NumberInput.hpp"
#include "RenderPanel.hpp"
#include "ViewMappingSelector.hpp"
#include "Utils.hpp"

enum {
  ID_ReloadShaders = wxID_HIGHEST + 1,
  ID_SelectSpaceDisplay,
  ID_SelectRenderDimensions,
  ID_SelectNumberSystem,
  ID_ResolutionSlider,
  ID_IterationModifier,
  ID_IterationsPerFrame
};

MainUIFrame::MainUIFrame(SharedState& sharedState) :
    wxFrame(nullptr, wxID_ANY, "Fractalism"), lastRenderMillis(wxGetUTCTimeMillis()), fps(0.0) {
  // Create the menu bar
  {
    wxMenuBar* menuBar = new wxMenuBar;

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_ReloadShaders, "&Reload Shaders\tCtrl-R", "Reload the active shaders from disk");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    menuBar->Append(menuFile, "&File");

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);
  }

  sharedState.mainStatusBar = CreateStatusBar(5);

  // Main sizer to manage primary layout
  wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);

  // Create the rendering canvases
  RenderPanel* phasePanel = new RenderPanel(this, sharedState.phase);
  mainSizer->Add(phasePanel, 1, wxEXPAND);
  RenderPanel* dynamicalPanel = new RenderPanel(this, sharedState.dynamical);
  mainSizer->Add(dynamicalPanel, 1, wxEXPAND);

  NumberInput *parameter;
  NumberInput *phaseView;
  NumberInput *dynamicalView;

  ViewMappingSelector *phaseViewMapping;
  ViewMappingSelector *dynamicalViewMapping;

  wxButton *centerParameterPhase;
  wxButton *centerParameterDynamical;
  wxButton *screenshotPhase;
  wxButton *screenshotDynamical;

  // Main control panel
  {
    wxBoxSizer *controlPanelSizer = new wxBoxSizer(wxVERTICAL);
    // Parameter and view controls
    {
      wxBoxSizer *numberInputSizer = new wxBoxSizer(wxHORIZONTAL);
      parameter = new NumberInput(
        this,
        "Parameter",
        sharedState.parameter.value,
        sharedState.parameter.isDirty,
        EVT_ParameterChanged);
      parameter->setNumberSystem(sharedState.numberSystem);
      numberInputSizer->Add(parameter);

      phaseView = new NumberInput(
        this,
        "Phase",
        sharedState.phase.view.value.center,
        sharedState.phase.view.isDirty,
        EVT_PhaseViewChanged);
      phaseView->setNumberSystem(sharedState.numberSystem);
      numberInputSizer->Add(phaseView);

      dynamicalView = new NumberInput(
        this,
        "Dynamical",
        sharedState.dynamical.view.value.center,
        sharedState.dynamical.view.isDirty,
        EVT_DynamicalViewChanged);
      dynamicalView->setNumberSystem(sharedState.numberSystem);
      numberInputSizer->Add(dynamicalView);

      controlPanelSizer->Add(numberInputSizer);
    }
    // View mapping slection
    {
      wxBoxSizer *viewMappingSizer = new wxBoxSizer(wxHORIZONTAL);
      phaseViewMapping = new ViewMappingSelector(this, "Phase View Mapping", sharedState.phase.view.value.mapping, sharedState.phase.view.isDirty);
      phaseViewMapping->setDimensions(sharedState.renderDimensions);
      phaseViewMapping->setNumberSystem(sharedState.numberSystem);
      viewMappingSizer->Add(phaseViewMapping);
      dynamicalViewMapping = new ViewMappingSelector(this, "Dynamical View Mapping", sharedState.dynamical.view.value.mapping, sharedState.dynamical.view.isDirty);
      dynamicalViewMapping->setDimensions(sharedState.renderDimensions);
      dynamicalViewMapping->setNumberSystem(sharedState.numberSystem);
      viewMappingSizer->Add(dynamicalViewMapping);

      controlPanelSizer->Add(viewMappingSizer);
    }
    // Center parameter buttons
    {
      wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
      centerParameterPhase = new wxButton(this, wxID_ANY, "Center C (Phase)");
      centerParameterPhase->Bind(wxEVT_BUTTON, [&sharedState](wxCommandEvent&) {
        sharedState.phase.view.value.center = sharedState.parameter;
        sharedState.phase.view.isDirty = true;
      });
      buttonSizer->Add(centerParameterPhase);

      centerParameterDynamical = new wxButton(this, wxID_ANY, "Center C (Dymanical)");
      centerParameterDynamical->Bind(wxEVT_BUTTON, [&sharedState](wxCommandEvent&) {
        sharedState.dynamical.view.value.center = sharedState.parameter;
        sharedState.dynamical.view.isDirty = true;
      });
      buttonSizer->Add(centerParameterDynamical);

      controlPanelSizer->Add(buttonSizer);
    }
    // Save image buttons
    {
      wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
      screenshotPhase = new wxButton(this, wxID_ANY, "Save Image (Phase)");
      screenshotPhase->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
        // TODO
      });
      buttonSizer->Add(screenshotPhase);

      screenshotDynamical = new wxButton(this, wxID_ANY, "Save Image (Dymanical)");
      screenshotDynamical->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
        // TODO
      });
      buttonSizer->Add(screenshotDynamical);

      controlPanelSizer->Add(buttonSizer);
    }
    // Space and dimensions
    {
      wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
      // Space
      {
        wxString labels[] = {"Phase", "Dynamical", "Both"};
        wxRadioBox *radioBox = new wxRadioBox(this, ID_SelectSpaceDisplay, "Space", wxDefaultPosition, wxDefaultSize, 3, labels);
        radioBox->SetSelection(2);
        hSizer->Add(radioBox);
        Bind(wxEVT_RADIOBOX, [=, this](wxCommandEvent& evt) {
          // Add one because we can treat the enum as a set of binary flags:
          // 01 => phase
          // 10 => dynamical
          // 11 => both
          options::Space selected = utils::fromUnderlyingType<options::Space>(evt.GetInt() + 1);
          bool phaseEnabled = utils::enabled<options::Space::phase>(selected);
          bool dynamicalEnabled = utils::enabled<options::Space::dynamical>(selected);

          phasePanel->Show(phaseEnabled);
          centerParameterPhase->Enable(phaseEnabled);
          phaseView->Enable(phaseEnabled);
          phaseViewMapping->Enable(phaseEnabled);
          screenshotPhase->Enable(phaseEnabled);

          dynamicalPanel->Show(dynamicalEnabled);
          centerParameterDynamical->Enable(dynamicalEnabled);
          dynamicalView->Enable(dynamicalEnabled);
          dynamicalViewMapping->Enable(dynamicalEnabled);
          screenshotDynamical->Enable(dynamicalEnabled);

          this->Layout();
        }, ID_SelectSpaceDisplay);
      }
      // Dimensions
      {
        wxString labels[] = {"2D", "3D"};
        wxRadioBox *radioBox = new wxRadioBox(this, ID_SelectRenderDimensions, "2D/3D", wxDefaultPosition, wxDefaultSize, 2, labels);
        radioBox->SetSelection(utils::toUnderlyingType(sharedState.renderDimensions) - 2);
        Bind(wxEVT_RADIOBOX, [&sharedState, phaseViewMapping, dynamicalViewMapping](wxCommandEvent& evt) {
          cltypes::Viewspace phaseView = sharedState.phase.view;
          cltypes::Viewspace dynamicalView = sharedState.dynamical.view;
          switch (evt.GetInt()) {
            case 0:
              sharedState.renderDimensions = options::Dimensions::two;
              sharedState.phase.lastZMapping = phaseView.mapping.z;
              phaseView.mapping.z = 0;
              sharedState.dynamical.lastZMapping = dynamicalView.mapping.z;
              dynamicalView.mapping.z = 0;
              break;
            case 1:
              sharedState.renderDimensions = options::Dimensions::three;
              phaseView.mapping.z = sharedState.phase.lastZMapping;
              dynamicalView.mapping.z = sharedState.phase.lastZMapping;
              break;
            default:
              return;
          }
          sharedState.phase.view = phaseView;
          phaseViewMapping->setDimensions(sharedState.renderDimensions);
          sharedState.dynamical.view = dynamicalView;
          dynamicalViewMapping->setDimensions(sharedState.renderDimensions);
        }, ID_SelectRenderDimensions);
        hSizer->Add(radioBox);
      }
      controlPanelSizer->Add(hSizer);
    }
    // Number system
    {
      wxString labels[] = {"Complex", "Bicomplex", "Tricomplex"};
      wxRadioBox* radioBox = new wxRadioBox(this, ID_SelectNumberSystem, "Number System", wxDefaultPosition, wxDefaultSize, 3, labels);
      radioBox->SetSelection(utils::toUnderlyingType(sharedState.numberSystem));
      Bind(wxEVT_RADIOBOX, [&sharedState, parameter, phaseView, phaseViewMapping, dynamicalView, dynamicalViewMapping, this](wxCommandEvent& evt) {
        options::NumberSystem numberSystem = utils::fromUnderlyingType<options::NumberSystem>(evt.GetInt());
        sharedState.numberSystem = numberSystem;
        sharedState.phase.numberSystem.isDirty = true;
        sharedState.dynamical.numberSystem.isDirty = true;
        parameter->setNumberSystem(numberSystem);
        phaseView->setNumberSystem(numberSystem);
        phaseViewMapping->setNumberSystem(numberSystem);
        dynamicalView->setNumberSystem(numberSystem);
        dynamicalViewMapping->setNumberSystem(numberSystem);
        Layout();
      }, ID_SelectNumberSystem);
      controlPanelSizer->Add(radioBox);
    }
    // Resolution
    {
      controlPanelSizer->Add(new wxStaticText(this, wxID_ANY, "Render Resolution"));
      wxSlider* slider = new wxSlider(this, ID_ResolutionSlider, sharedState.resolution / 64, 1, 20);
      controlPanelSizer->Add(slider, 0, wxEXPAND);
      Bind(wxEVT_SLIDER, [&sharedState, this](wxCommandEvent& evt) {
        int resolution = evt.GetInt() * 64;
        if (resolution != sharedState.resolution) {
          sharedState.resolution = resolution;
          SetStatusText(wxString::Format("Resolution: %d", sharedState.resolution.value), 3);
          sharedState.phase.resolution.isDirty = true;
          sharedState.dynamical.resolution.isDirty = true;
        }
      }, ID_ResolutionSlider);
    }
    // Iteration modifier
    {
      controlPanelSizer->Add(new wxStaticText(this, wxID_ANY, "Iteration Modifier"));
      wxSlider* slider = new wxSlider(this, ID_IterationModifier, ((int) sharedState.iterationModifier) / 5, 1, 100);
      controlPanelSizer->Add(slider, 0, wxEXPAND);
      Bind(wxEVT_SLIDER, [&sharedState, this](wxCommandEvent& evt) {
        sharedState.iterationModifier = evt.GetInt() * 5.0;
        SetStatusText(wxString::Format("Iteration Modifier: %.2f", sharedState.iterationModifier), 1);
        sharedState.phase.view.isDirty = true;
        sharedState.dynamical.view.isDirty = true;
      }, ID_IterationModifier);
    }
    // Iterations per frame
    {
      controlPanelSizer->Add(new wxStaticText(this, wxID_ANY, "Iterations Per Frame"));
      wxSlider *slider = new wxSlider(this, ID_IterationsPerFrame, sharedState.iterationsPerFrame, 1, 1000);
      controlPanelSizer->Add(slider, 0, wxEXPAND);
      Bind(wxEVT_SLIDER, [&sharedState, this](wxCommandEvent& evt) {
        sharedState.iterationsPerFrame = evt.GetInt();
        SetStatusText(wxString::Format("Iterations Per Frame: %d", sharedState.iterationsPerFrame), 2);
      }, ID_IterationsPerFrame);
    }
    mainSizer->Add(controlPanelSizer);
  }

  SetSizerAndFit(mainSizer);
  SetSize(wxSize(800, 600));
  SetStatusText("Fractalism");
  SetStatusText(wxString::Format("Iteration Modifier: %.2f", sharedState.iterationModifier), 1);
  SetStatusText(wxString::Format("Iterations Per Frame: %d", sharedState.iterationsPerFrame), 2);
  SetStatusText(wxString::Format("Resolution: %d", sharedState.resolution.value), 3);

  GLRenderCanvas* phaseCanvas = phasePanel->canvas;
  GLRenderCanvas* dynamicalCanvas = dynamicalPanel->canvas;

  // Bind events to be processed
  Bind(wxEVT_IDLE, [phaseCanvas, dynamicalCanvas, &sharedState, this](wxIdleEvent& evt) {
    bool renderPhase = phaseCanvas->IsShownOnScreen();
    bool renderDynamical = dynamicalCanvas->IsShownOnScreen();
    try {
      
      if (renderPhase) {
        phaseCanvas->render();
      }
      if (renderDynamical) {
        dynamicalCanvas->render();
      }
    } catch (const std::exception& e) {
      wxLogFatalError(e.what());
      wxSafeShowMessage("Error", e.what());
    }
    wxLongLong now = wxGetUTCTimeMillis();
    wxLongLong delta = now - lastRenderMillis;
    lastRenderMillis = now;
    const double fpsSmoothing = 0.7;
    fps = (fps * fpsSmoothing) + ((1.0 / (delta.ToDouble() / 1000.0)) * (1.0 - fpsSmoothing));
    if (isnan(fps) || isinf(fps)) {
      fps = 0.0;
    }
    SetStatusText(wxString::Format("FPS: %.2f", fps), 4);
    evt.RequestMore();
  });
  Bind(wxEVT_MENU, [phaseCanvas, dynamicalCanvas, &sharedState, this](wxCommandEvent&) {
      sharedState.reloadShaders();
    }, ID_ReloadShaders);
  Bind(wxEVT_MENU, [](wxCommandEvent&) {
      wxMessageBox("A unique application for fractal rendering and exploration.", "About Fractalism", wxOK | wxICON_INFORMATION);
    }, wxID_ABOUT);
  Bind(wxEVT_MENU, [this](wxCommandEvent&) {
      Close(true);
    }, wxID_EXIT);
}

MainUIFrame::~MainUIFrame() {}