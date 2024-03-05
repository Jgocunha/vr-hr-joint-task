#pragma once


#include <user_interface/user_interface_window.h>

class ExperimentWindow : public dnf_composer::user_interface::UserInterfaceWindow
{
public:
	ExperimentWindow();
	~ExperimentWindow();

	void render() override;
};