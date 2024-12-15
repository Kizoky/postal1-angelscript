#ifndef POSTAL_ANGELSCRIPT_IMGUI_H
#define POSTAL_ANGELSCRIPT_IMGUI_H
#pragma once

extern bool imgui_shown;

class CImguiHelper
{
public:
	CImguiHelper(bool boot);

	int CreateImgui();

	void Enable()
	{
		CreateImgui();
	}

	void Disable()
	{
		// Cleans up automatically
		imgui_shown = false;
	}

	void ToggleShowFlag()
	{
		imgui_shown = !imgui_shown;
	}

public:
	bool IsShown() { return imgui_shown; }
	bool IsUp() { return imgui_up; }

private:
	bool imgui_shown;
	bool imgui_up;
};

extern CImguiHelper g_ImGui;

#endif