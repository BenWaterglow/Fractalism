#include "ViewMappingSelector.hpp"

static const char* viewMappingOptions[] = { "none", "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7" };

ViewMappingSelector::ViewMappingSelector(wxWindow* parent, wxString label, cl_char3& mapping, bool& dirty) : wxPanel(parent), mapping(mapping) {
  wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);
  mainBoxSizer->Add(new wxStaticText(this, wxID_ANY, label));
  wxBoxSizer *row;
  // x
  row  = new wxBoxSizer(wxHORIZONTAL);
  row->Add(inverted.x = new wxCheckBox(this, wxID_ANY, "inv?", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT));
  inverted.x->SetValue(mapping.x < 0);
  inverted.x->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& evt) {
    cl_char val = abs(mapping.x);
    mapping.x = evt.IsChecked() ? -val : val;
    dirty = true;
  });
  row->Add(labels.x = new wxStaticText(this, wxID_ANY, "x=>"));
  row->Add(choices.x = new wxChoice(this, wxID_ANY));
  choices.x->Bind(wxEVT_CHOICE, [&](wxCommandEvent& evt) {
    cl_char val = static_cast<cl_char>(evt.GetInt());
    mapping.x = mapping.x < 0 ? -val : val;
    dirty = true;
  });
  mainBoxSizer->Add(row);
  // y
  row = new wxBoxSizer(wxHORIZONTAL);
  row->Add(inverted.y = new wxCheckBox(this, wxID_ANY, "inv?", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT));
  inverted.y->SetValue(mapping.y < 0);
  inverted.y->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& evt) {
    cl_char val = abs(mapping.y);
    mapping.y = evt.IsChecked() ? -val : val;
    dirty = true;
  });
  row->Add(labels.y = new wxStaticText(this, wxID_ANY, "y=>"));
  row->Add(choices.y = new wxChoice(this, wxID_ANY));
  choices.y->Bind(wxEVT_CHOICE, [&](wxCommandEvent& evt) {
    cl_char val = static_cast<cl_char>(evt.GetInt());
    mapping.y = mapping.y < 0 ? -val : val;
    dirty = true;
  });
  mainBoxSizer->Add(row);
  // z
  row = new wxBoxSizer(wxHORIZONTAL);
  row->Add(inverted.z = new wxCheckBox(this, wxID_ANY, "inv?", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT));
  inverted.z->SetValue(mapping.z < 0);
  inverted.z->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& evt) {
    cl_char val = abs(mapping.z);
    mapping.z = evt.IsChecked() ? -val : val;
    dirty = true;
  });
  row->Add(labels.z = new wxStaticText(this, wxID_ANY, "z=>"));
  row->Add(choices.z = new wxChoice(this, wxID_ANY));
  choices.z->Bind(wxEVT_CHOICE, [&](wxCommandEvent& evt) {
    cl_char val = static_cast<cl_char>(evt.GetInt());
    mapping.z = mapping.z < 0 ? -val : val;
    dirty = true;
  });
  mainBoxSizer->Add(row);

  SetSizerAndFit(mainBoxSizer);
}

ViewMappingSelector::~ViewMappingSelector() {}

void ViewMappingSelector::setDimensions(options::Dimensions dimensions) {
  bool showZ = dimensions == options::Dimensions::three;
  inverted.z->Show(showZ);
  labels.z->Show(showZ);
  choices.z->Show(showZ);
  Layout();
}

void ViewMappingSelector::setNumberSystem(options::NumberSystem numberSystem) {
  size_t optionCount;
  switch (numberSystem) {
    case options::NumberSystem::c1:
      optionCount = 3;
      break;
    case options::NumberSystem::c2:
      optionCount = 5;
      break;
    case options::NumberSystem::c3:
      optionCount = 9;
      break;
    default:
      return;
  }

  wxArrayString selectedOptions(optionCount, viewMappingOptions);

  choices.x->Clear();
  choices.y->Clear();
  choices.z->Clear();

  choices.x->Set(selectedOptions);
  choices.x->SetSelection(abs(mapping.x));
  inverted.x->SetValue(mapping.x < 0);
  choices.y->Set(selectedOptions);
  choices.y->SetSelection(abs(mapping.y));
  inverted.y->SetValue(mapping.y < 0);
  choices.z->Set(selectedOptions);
  choices.z->SetSelection(abs(mapping.z));
  inverted.z->SetValue(mapping.z < 0);
  Layout();
}