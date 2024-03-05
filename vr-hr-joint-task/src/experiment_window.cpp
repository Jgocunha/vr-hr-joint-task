#include "experiment_window.h"


ExperimentWindow::ExperimentWindow()
{
	targetObject = 1;
}

ExperimentWindow::~ExperimentWindow()
{
}

void ExperimentWindow::render()
{
	if(ImGui::Begin("Target object selector window"))
	{
		ImGui::Text("Choose the target object:");
		ImGui::RadioButton("Object 1", &targetObject, 1);
		ImGui::RadioButton("Object 2", &targetObject, 2);
		ImGui::RadioButton("Object 3", &targetObject, 3);
	}
	ImGui::End();
}

int ExperimentWindow::getTargetObject() const
{
	return targetObject;
}
