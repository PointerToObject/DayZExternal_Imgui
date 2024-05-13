#include <Windows.h>
#include <iostream>
#include "window.hpp"
#include "Memory.h"
#include <thread>



int main()
{
	// thread your cheat here 
	
   	 CDispatcher* mem = CDispatcher::Get();
	 mem->Attach("DayZ_x64.exe");
	// hide console window since we're making our own window, or use WinMain() instead.
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	overlay.shouldRun = true;
	overlay.RenderMenu = false;

	overlay.CreateOverlay();
	overlay.CreateDevice();
	overlay.CreateImGui();

	printf("[>>] Hit NUM0 to show the menu in this overlay!\n");

	overlay.SetForeground(GetConsoleWindow());

	while (overlay.shouldRun)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		overlay.StartRender();

		if (overlay.RenderMenu) {
			overlay.Render();
		}
		else
		ImGui::GetBackgroundDrawList()->AddText({ 0, 0 }, ImColor(245.0f, 0.0f, 31.0f), "Z3N1TH R3L04D3D - Press NUM0 to use");
		if (drawCrosshair == true) {
			ImVec2 screenSize = ImGui::GetIO().DisplaySize;
			ImVec2 center(screenSize.x * 0.5f, screenSize.y * 0.5f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(center.x - 20, center.y), ImVec2(center.x + 20, center.y), IM_COL32(57, 255, 20, 220), 0.3f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(center.x, center.y - 20), ImVec2(center.x, center.y + 20), IM_COL32(57, 255, 20, 220), 0.3f);
		}
		// if you want to render here, you could move the imgui includes to your .hpp file instead of the .cpp file!
		overlay.EndRender();
	}

	overlay.DestroyImGui();
	overlay.DestroyDevice();
	overlay.DestroyOverlay();
}