#pragma once


#include <user_interface/user_interface_window.h>

class ExperimentWindow : public dnf_composer::user_interface::UserInterfaceWindow
{
private:
	int targetObject;
public:
	ExperimentWindow();
	~ExperimentWindow();

	void render() override;
	int getTargetObject() const;
};