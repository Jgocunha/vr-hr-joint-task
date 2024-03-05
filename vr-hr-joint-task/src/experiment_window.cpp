#include "experiment_window.h"


ExperimentWindow::ExperimentWindow()
{
}

ExperimentWindow::~ExperimentWindow()
{
}

void ExperimentWindow::render()
{
	if(ImGui::Begin("Experiment"))
		ImGui::Text("This is the experiment window");
	ImGui::End();
}
