#include <Windows.h>
#include "GameEngine.h"
#include <iostream>
#include "window.hpp"
#include "Memory.h"
#include <thread>
#include "offsets.h"

using offsets::xWorld;

int main()
{
	// thread your cheat here 
	
   	 CDispatcher* mem = CDispatcher::Get();
	 mem->Attach("DayZ_x64.exe");
	 uint64_t base = mem->GetModuleBase("DayZ_x64.exe");
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
		

		if (showEsp) {

			uint64_t worldPtr = mem->ReadMemory<uint64_t>(base + xWorld);
			World world = mem->ReadMemory<World>(worldPtr);
			Camera cam = mem->ReadMemory<Camera>(world.Camera);

			for (int i = 0; i < world.NearTableCount; i++) {
				uint64_t nearEntity = mem->ReadMemory<uint64_t>(world.NearTable + (i * 0x8));
				uint64_t entityVT = mem->ReadMemory<uint64_t>(nearEntity + 0x0);
				uint64_t entityVisState = mem->ReadMemory<uint64_t>(nearEntity + 0x1D0);
				CVector pos = mem->ReadMemory<CVector>(entityVisState + 0x2C);
				CVector screen;

				uint64_t ptrrtti = mem->ReadMemory<uint64_t>(entityVT - 0x8);
				RTTI rtti = mem->ReadMemory<RTTI>(ptrrtti);
				type_descriptor type = mem->ReadMemory<type_descriptor>(base + rtti.rva_type_descriptor);
				const char* typeName = type.get_type_name();
				size_t typeNameLength = strlen(typeName);
				std::string modifiedTypeName(typeName, typeNameLength - 2);

				if (cam.ScreenPosition(pos, screen)) {
					ImGui::GetForegroundDrawList()->AddRect(ImVec2(screen.x - 3.0f, screen.y - 3.0f), ImVec2(screen.x + 3.0f, screen.y + 3.0f), IM_COL32(255, 255, 255, 255), 1, 1.0f);
					ImGui::GetForegroundDrawList()->AddText(ImVec2(screen.x + 6.0, screen.y - 3.0f), IM_COL32(102, 100, 255, 255), modifiedTypeName.c_str());
				}
			}
			for (int i = 0; i < world.FarTableCount; i++) {
				uint64_t farEntity = mem->ReadMemory<uint64_t>(world.FarTable + (i * 0x8));
				uint64_t entityVisState = mem->ReadMemory<uint64_t>(farEntity + 0x1D0);
				uint64_t entityVT = mem->ReadMemory<uint64_t>(farEntity + 0x0);
				CVector pos = mem->ReadMemory<CVector>(entityVisState + 0x2C);
				CVector screen;


				uint64_t ptrrtti = mem->ReadMemory<uint64_t>(entityVT - 0x8);
				RTTI rtti = mem->ReadMemory<RTTI>(ptrrtti);
				type_descriptor type = mem->ReadMemory<type_descriptor>(base + rtti.rva_type_descriptor);
				const char* typeName = type.get_type_name();
				size_t typeNameLength = strlen(typeName);
				std::string modifiedTypeName(typeName, typeNameLength - 2);

				if (cam.ScreenPosition(pos, screen)) {
					ImGui::GetForegroundDrawList()->AddRect(ImVec2(screen.x - 3.0f, screen.y - 3.0f), ImVec2(screen.x + 3.0f, screen.y + 3.0f), IM_COL32(255, 255, 255, 255), 1, 1.0f);
					ImGui::GetForegroundDrawList()->AddText(ImVec2(screen.x + 6.0, screen.y - 3.0f), IM_COL32(102, 100, 255, 255), modifiedTypeName.c_str());
				}
			}
		}
		if (showBulletESP) {
			uint64_t worldPtr = mem->ReadMemory<uint64_t>(base + xWorld);
			World world = mem->ReadMemory<World>(worldPtr);
			Camera cam = mem->ReadMemory<Camera>(world.Camera);



			for (int i = 0; i < world.BulletTableCount; i++) {
				uint64_t bulletEntity = mem->ReadMemory<uint64_t>(world.BulletTable + (i * 0x8));
				uint64_t entityVisState = mem->ReadMemory<uint64_t>(bulletEntity + 0x1D0);
				CVector pos = mem->ReadMemory<CVector>(entityVisState + 0x2C);
				CVector screen;
				if (cam.ScreenPosition(pos, screen)) {
					ImGui::GetForegroundDrawList()->AddCircle(ImVec2(screen.x, screen.y), 8.0f, IM_COL32(57, 22, 255, 220), 0, 1.0f);
				}
			}
		}
		// if you want to render here, you could move the imgui includes to your .hpp file instead of the .cpp file!
		overlay.EndRender();
	}

	overlay.DestroyImGui();
	overlay.DestroyDevice();
	overlay.DestroyOverlay();
}