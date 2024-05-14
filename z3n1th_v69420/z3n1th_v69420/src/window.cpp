#include "window.hpp"
#include <iostream>
#include <dwmapi.h>
#include <stdio.h>

#include "offsets.h"
#include "GameEngine.h"
#include "Memory.h"


ID3D11Device* Overlay::device = nullptr;

// sends rendering commands to the device
ID3D11DeviceContext* Overlay::device_context = nullptr;

// manages the buffers for rendering, also presents rendered frames.
IDXGISwapChain* Overlay::swap_chain = nullptr;

// represents the target surface for rendering
ID3D11RenderTargetView* Overlay::render_targetview = nullptr;


HWND Overlay::overlay = nullptr;
WNDCLASSEX Overlay::wc = { };

// declaration of the ImGui_ImplWin32_WndProcHandler function
// basically integrates ImGui with the Windows message loop so ImGui can process input and events
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
using namespace offsets;

LRESULT CALLBACK window_procedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// set up ImGui window procedure handler
	if (ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	// switch that disables alt application and checks for if the user tries to close the window.
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu (imgui uses it in their example :shrug:)
			return 0;
		break;

	case WM_DESTROY:
		Overlay::DestroyDevice();
		Overlay::DestroyOverlay();
		Overlay::DestroyImGui();
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		Overlay::DestroyDevice();
		Overlay::DestroyOverlay();
		Overlay::DestroyImGui();
		return 0;
	}

	// define the window procedure
	return DefWindowProc(window, msg, wParam, lParam);
}

bool Overlay::CreateDevice()
{
	// First we setup our swap chain, this basically just holds a bunch of descriptors for the swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	// set number of back buffers (this is double buffering)
	sd.BufferCount = 2;

	// width + height of buffer, (0 is automatic sizing)
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;

	// set the pixel format
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// set the fps of the buffer (60 at the moment)
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	// allow mode switch (changing display modes)
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// set how the bbuffer will be used
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.OutputWindow = overlay;

	// setup the multi-sampling
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// specify what Direct3D feature levels to use
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	// create device and swap chain
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&featureLevel,
		&device_context);

	// if the hardware isn't supported create with WARP (basically just a different renderer)
	if (result == DXGI_ERROR_UNSUPPORTED) {
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			0U,
			featureLevelArray,
			2, D3D11_SDK_VERSION,
			&sd,
			&swap_chain,
			&device,
			&featureLevel,
			&device_context);

		printf("[>>] DXGI_ERROR | Created with D3D_DRIVER_TYPE_WARP\n");
	}

	// can't do much more, if the hardware still isn't supported just return false.
	if (result != S_OK) {
		printf("Device Not Okay\n");
		return false;
	}

	// retrieve back_buffer, im defining it here since it isn't being used at any other point in time.
	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	// if back buffer is obtained then we can create render target view and release the back buffer again
	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_targetview);
		back_buffer->Release();

		printf("[>>] Created Device\n");
		return true;
	}

	// if we reach this point then it failed to create the back buffer
	printf("[>>] Failed to create Device\n");
	return false;
}

void Overlay::DestroyDevice()
{
	// release everything that has to do with the device.
	if (device)
	{
		device->Release();
		device_context->Release();
		swap_chain->Release();
		render_targetview->Release();

		printf("[>>] Released Device\n");
	}
	else
		printf("[>>] Device Not Found when Exiting.\n");
}

void Overlay::CreateOverlay()
{
	// holds descriptors for the window, called a WindowClass
	// set up window class
	wc.cbSize = sizeof(wc);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = GetModuleHandleA(0);
	wc.lpszClassName = "carlgwastaken";

	// register our class
	RegisterClassEx(&wc);

	// create window (the actual one that shows up in your taskbar)
	// WS_EX_TOOLWINDOW hides the new window that shows up in your taskbar and attaches it to any already existing windows instead.
	// (in this case the console)
	overlay = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		wc.lpszClassName,
		"cheat",
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN), // 1920
		GetSystemMetrics(SM_CYSCREEN), // 1080
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (overlay == NULL)
		printf("Failed to create Overlay\n");

	// set overlay window attributes to make the overlay transparent
	SetLayeredWindowAttributes(overlay, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	// set up the DWM frame extension for client area
	{
		// first we define our RECT structures that hold our client and window area
		RECT client_area{};
		RECT window_area{};

		// get the client and window area
		GetClientRect(overlay, &client_area);
		GetWindowRect(overlay, &window_area);

		// calculate the difference between the screen and window coordinates
		POINT diff{};
		ClientToScreen(overlay, &diff);

		// calculate the margins for DWM frame extension
		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		// then we extend the frame into the client area
		DwmExtendFrameIntoClientArea(overlay, &margins);
	}

	// show + update overlay
	ShowWindow(overlay, SW_SHOW);
	UpdateWindow(overlay);

	printf("[>>] Overlay Created\n");
}

const char* FloatToConstChar(float value) {
	static std::string str;
	str = std::to_string(value);
	return str.c_str();
}

void Overlay::DestroyOverlay()
{
	DestroyWindow(overlay);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool Overlay::CreateImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Initalize ImGui for the Win32 library
	if (!ImGui_ImplWin32_Init(overlay)) {
		printf("Failed ImGui_ImplWin32_Init\n");
		return false;
	}

	// Initalize ImGui for DirectX 11.0
	if (!ImGui_ImplDX11_Init(device, device_context)) {
		printf("Failed ImGui_ImplDX11_Init\n");
		return false;
	}

	printf("[>>] ImGui Initialized\n");
	return true;
}

void Overlay::DestroyImGui()
{
	// Cleanup ImGui by shutting down DirectX11, the Win32 Platform and Destroying the ImGui context.
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Overlay::StartRender()
{
	// handle windows messages
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// begin a new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// if the user presses Insert then enable the menu.
	if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
		RenderMenu = !RenderMenu;

		// If we are rendering the menu set the window styles to be able to clicked on.
		if (RenderMenu) {
			SetWindowLong(overlay, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
		}
		else {
			SetWindowLong(overlay, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
		}
	}
}

void Overlay::EndRender()
{
	// Make a color that's clear / transparent
	float color[4]{ 0, 0, 0, 0 };
	// Set the render target and then clear it
	device_context->OMSetRenderTargets(1, &render_targetview, nullptr);
	device_context->ClearRenderTargetView(render_targetview, color);

	// Render ImGui
	ImGui::Render();





	// Render ImGui draw data.
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present rendered frame with V-Sync
	swap_chain->Present(1U, 0U);

	// Present rendered frame without V-Sync
	//swap_chain->Present(0U, 0U);
}





bool switchState = false;
bool showSecondWindow = false;
bool showFourthWindow = false;
bool showFifthWindow = false;
bool showEsp = false;
bool showBulletESP = false;
bool showSlider = false;
bool drawCrosshair = false;
float sliderValue = 10.0f;
float eyeAccomNormal = 1.0f;
float back = -4.0f;
float front = 4.0f;





void Overlay::Render()
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	CDispatcher* mem = CDispatcher::Get();
	uint64_t base = mem->GetModuleBase("DayZ_x64.exe");
	uint64_t worldPtr = mem->ReadMemory<uint64_t>(base + xWorld);
	Camera cam;


	ImGui::SetNextWindowSize({ 375,250});
	ImGui::Begin("Z3N1TH R3L04D3D", &RenderMenu, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

	ImGui::Checkbox("ESP", &showEsp);

	ImGui::Checkbox("Bullet ESP", &showBulletESP);

	if (ImGui::Button("entity list")) {// yes yes
		showFourthWindow = true;
	}

	if (ImGui::Button("Bullet Table")) {
		showSecondWindow = true;
	}

	if (ImGui::Button("Player Position")) {
		showFifthWindow = true;
	}


	if (showSecondWindow) {
		ImGui::SetNextWindowSize({ 433,400 }); 
		ImGui::Begin("Z3N1TH R3L04D3D - Bullet Table", &showSecondWindow, ImGuiWindowFlags_NoSavedSettings);

		World world = mem->ReadMemory<World>(worldPtr);

		for (int i = 0; i < world.BulletTableCount; i++) {
			uint64_t bulletEntity = mem->ReadMemory<uint64_t>(world.BulletTable + (i * 0x8));
			uint64_t entityVisState = mem->ReadMemory<uint64_t>(bulletEntity + 0x1D0);
			CVector pos = mem->ReadMemory<CVector>(entityVisState + 0x2C);
			
			ImGui::Text("(Bullet Table) - Entity %d Pos: %.3f, %.3f, %.3f", i, pos.x, pos.y, pos.z);
		}
			

		ImGui::End();
	}


	if (showFourthWindow) {
		ImGui::SetNextWindowSize({ 700,400 });
		ImGui::Begin("Z3N1TH R3L04D3D - entity list", &showFourthWindow, ImGuiWindowFlags_NoSavedSettings);

		World world = mem->ReadMemory<World>(worldPtr);

		while (true) {
			ImGui::BeginChild("EntityList", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

			for (int i = 0; i < world.NearTableCount; i++) {
				uint64_t entity = mem->ReadMemory<uint64_t>(world.NearTable + (i * 0x8));
				uint64_t entityVT = mem->ReadMemory<uint64_t>(entity + 0x0);
				uint64_t entityVisState = mem->ReadMemory<uint64_t>(entity + 0x1D0);
				CVector pos = mem->ReadMemory<CVector>(entityVisState + 0x2C);
				CVector screenPos;

				//VTABLE 
				uint64_t ptrrtti = mem->ReadMemory<uint64_t>(entityVT - 0x8);
				RTTI rtti = mem->ReadMemory<RTTI>(ptrrtti);
				type_descriptor type = mem->ReadMemory<type_descriptor>(base + rtti.rva_type_descriptor);
				//VTABLE 
				const char* typeName = type.get_type_name();
				size_t typeNameLength = strlen(typeName);
				std::string modifiedTypeName(typeName, typeNameLength - 2);

				ImGui::Text("(Near Table) -(%s) Entity %d Pos: %.2f , %.2f ,%.2f ", modifiedTypeName.c_str(), i, pos.x, pos.y, pos.z);


			}
			
			for (int i = 0; i < world.FarTableCount; i++) {
				uint64_t entity = mem->ReadMemory<uint64_t>(world.FarTable + (i * 0x8));
				uint64_t entityVT = mem->ReadMemory<uint64_t>(entity + 0x0);
				uint64_t entityVisState = mem->ReadMemory<uint64_t>(entity + 0x1D0);
				CVector pos = mem->ReadMemory<CVector>(entityVisState + 0x2C);
				// VTABLE
				uint64_t ptrrtti = mem->ReadMemory<uint64_t>(entityVT - 0x8);
				RTTI rtti = mem->ReadMemory<RTTI>(ptrrtti);
				type_descriptor type = mem->ReadMemory<type_descriptor>(base + rtti.rva_type_descriptor);
				const char* typeName = type.get_type_name();
				size_t typeNameLength = strlen(typeName);
				std::string modifiedTypeName(typeName, typeNameLength - 2);
				//VTABLE

				ImGui::Text("(Far Table) -(%s) Entity %d Pos: %.2f, %.2f, %.2f", modifiedTypeName.c_str(), i, pos.x, pos.y, pos.z);
			
			}

			ImGui::EndChild();
			break;
		}

		ImGui::End();
	}

	if (showEsp) {
		
	}

	if (showFifthWindow) {
		ImGui::SetNextWindowSize({ 372,72 }); // works :D
		ImGui::Begin("Z3N1TH R3L04D3D - Player Position", &showFifthWindow, ImGuiWindowFlags_NoSavedSettings);
		//     [[[[[[DayZ_x64.exe + 0x414CF70] + 0xF48]] + 0x1D0]] + 0x2C] pointer chain to position
		//	   [[DayZ_x64.exe + 0xWORLD] + 0x8] - 0xA8	- Local Player
		World world = mem->ReadMemory<World>(worldPtr);
		uint64_t EntityLink = mem->ReadMemory<uint64_t>(worldPtr + 0x2960);
		uint64_t LocalPlayer = mem->ReadMemory<uint64_t>(EntityLink + 0x8) - 0xA8;
		uint64_t localPlrVisState = mem->ReadMemory<uint64_t>(LocalPlayer + 0x1D0);
		CVector pos = mem->ReadMemory<CVector>(localPlrVisState + 0x2C);

		ImGui::Text("Local Player Position : %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);

		ImGui::End();
	}

	
	if (ImGui::Button("Toggle Eye Accom")) { // works
		showSlider = !showSlider; 
	}

	
	if (showSlider) { // works

		ImGui::SliderFloat("Eye Accom Value", &sliderValue, 0.0f, 50.0f);
		mem->WriteMemory<float>(worldPtr + offsetof(World, EyeAccom), sliderValue);
	}
	else {
		mem->WriteMemory<float>(worldPtr + offsetof(World, EyeAccom), eyeAccomNormal);
	}

	ImGui::Checkbox("Draw Crosshair", &drawCrosshair); 
	{
		// draws in main.cpp
	}

	ImGui::End();
}

void Overlay::SetForeground(HWND window)
{
	if (!IsWindowInForeground(window))
		BringToForeground(window);
}

