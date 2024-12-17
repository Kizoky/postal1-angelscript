#include "as_imgui.h"
#include "as_main.h"

#include "SDL.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"

#include "game.h"
#include "realm.h"

#include "as_imgui_console.h"

extern SDL_Renderer* sdlRenderer; // Extern - Kizoky
extern SDL_Window* sdlWindow;

CImguiHelper g_ImGui(true);

CImguiHelper::CImguiHelper(bool boot)
{
	imgui_shown = false;
	imgui_up = false;
}

// Main loop of ImGui for AngelScript debugging - Kizoky
int CImguiHelper::CreateImgui()
{
	if (!imgui_shown)
	{
		return 0;
	}

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Set ImGui style
	ImGui::StyleColorsDark();

	// Initialize ImGui SDL2 and SDL_Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow, sdlRenderer);
	ImGui_ImplSDLRenderer2_Init(sdlRenderer);

	imgui_up = true;

	// This is supposed to be "clear" but I'm not sure where to place this code
	// to make Postal render properly - Kizoky
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		if (!imgui_shown)
		{
			done = true;
		}

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DELETE)
			{
				imgui_shown = false;
				done = true;
			}

			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdlWindow))
				done = true;

			// Always let ImGui process the event
			ImGui_ImplSDL2_ProcessEvent(&event);
		}

		// Start a new ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		static bool bShowConsole = false;
		static bool bShowModule = false;
		static bool bShowEngine = false;
		static bool bShowGame = false;
		static ExampleAppConsole console;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Main"))
			{
				if (ImGui::MenuItem("Show/hide engine", "1")) 
				{
					bShowEngine = !bShowEngine;
				}
				if (ImGui::MenuItem("Show/hide script module", "2"))
				{
					bShowModule = !bShowModule;
				}
				if (ImGui::MenuItem("Show/hide console", "3"))
				{
					bShowConsole = !bShowConsole;
					if (bShowConsole)
					{
						console.ClearLog();
						for (int i = 0; i < g_AngelScript.GetLog().size(); i++)
						{
							console.AddLog(g_AngelScript.GetLog()[i].c_str());
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Show/hide game stats", "4"))
				{
					bShowGame = !bShowGame;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// ImGui overlay
		//ImGui::SetNextWindowBgAlpha(0.5f);
		if (bShowEngine)
		{
			ImGui::Begin("Engine", &imgui_shown, /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("Exe: '%s'", g_loaderPath.c_str());
				ImGui::Text("Enum count: %d", (int)g_AngelScript.GetEngine()->GetEnumCount());
				ImGui::Text("Funcdef count: %d", (int)g_AngelScript.GetEngine()->GetFuncdefCount());
				ImGui::Text("Global function count: %d", (int)g_AngelScript.GetEngine()->GetGlobalFunctionCount());
				ImGui::Text("Global property count: %d", (int)g_AngelScript.GetEngine()->GetGlobalPropertyCount());
				ImGui::Text("Module count: %d", (int)g_AngelScript.GetEngine()->GetModuleCount());
				ImGui::Text("Typedef count: %d", (int)g_AngelScript.GetEngine()->GetTypedefCount());
				ImGui::Checkbox("Log Default to console", &g_AngelScript.bLogNone);
				ImGui::Checkbox("Log Script to console", &g_AngelScript.bLogScript);
				ImGui::Checkbox("Log Callback to console", &g_AngelScript.bLogCallback);
				ImGui::Checkbox("Log Main to console", &g_AngelScript.bLogMain);
				ImGui::Checkbox("Log Compile to console", &g_AngelScript.bLogCompile);
				ImGui::Checkbox("Log ScriptObj Construct to console", &g_AngelScript.bLogObjConstr);
				ImGui::Checkbox("Log ScriptObj Destruct to console", &g_AngelScript.bLogObjDestr);
				ImGui::Checkbox("Log ScriptObj Fail to console", &g_AngelScript.bLogObjFail);
				//ImGui::SliderFloat("Example Slider", &io.Framerate, 0.0f, 120.0f, "%.1f FPS");
			ImGui::End();
		}

		if (bShowModule)
		{
			if (g_AngelScript.GetModule())
			{
				ImGui::Begin("Module", &imgui_shown, /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_AlwaysAutoResize);
					ImGui::Text("Default namespace: '%s'", g_AngelScript.GetModule()->GetDefaultNamespace());
					ImGui::Text("Enum count: %d", (int)g_AngelScript.GetModule()->GetEnumCount());
					ImGui::Text("Function count: %d", (int)g_AngelScript.GetModule()->GetFunctionCount());
					ImGui::Text("Var count: %d", (int)g_AngelScript.GetModule()->GetGlobalVarCount());
					ImGui::Text("Imported function count: %d", (int)g_AngelScript.GetModule()->GetImportedFunctionCount());
					ImGui::Text("Name: '%s'", g_AngelScript.GetModule()->GetName());
					ImGui::Text("Object count: %d", (int)g_AngelScript.GetModule()->GetObjectTypeCount());
					ImGui::Text("Typedef count: %d", (int)g_AngelScript.GetModule()->GetTypedefCount());
					//ImGui::SliderFloat("Example Slider", &io.Framerate, 0.0f, 120.0f, "%.1f FPS");
					if (ImGui::Button("Recompile (main menu only)"))
					{
						g_AngelScript.Recompile();
					}
				ImGui::End();
			}
			else
			{
				ImGui::Begin("Module (error)", &imgui_shown, /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_AlwaysAutoResize);
					ImGui::Text("error");
				ImGui::End();
			}
		}

		if (bShowConsole)
		{
			//ShowExampleAppConsole(&bShowConsole);
			console.Draw("Console", &bShowConsole);

			//ImGui::Begin("Console", &imgui_shown, /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_AlwaysAutoResize);
			//	for (int i = 0; i < g_AngelScript.GetLog().size(); i++)
			//	{
			//		ImGui::Text("(%d) %s", i, g_AngelScript.GetLog()[i].c_str());
			//	}
			//	if (ImGui::Button("Clear log"))
			//	{
			//		g_AngelScript.ClearLog();
			//	}
			//ImGui::End();
		}

		if (bShowGame)
		{
			ImGui::Begin("Game", &imgui_shown, /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_AlwaysAutoResize);
			CRealm* pRealm = g_AngelScript.GetRealm();
			if (pRealm)
			{
				ImGui::Text("Killed: %d", Stat_KilledHostiles + Stat_KilledCivilians);
				ImGui::Text("Population: %d/%d (alive: %d)", pRealm->m_sPopulationDeaths, pRealm->m_sPopulationBirths, pRealm->m_sPopulation);
				ImGui::Text("Hostiles: %d/%d (alive: %d)", pRealm->m_sHostileKills, pRealm->m_sHostileBirths, pRealm->m_sHostiles);
				ImGui::Text("Civilians: %d/%d (alive: %d)", pRealm->m_sPopulationDeaths - pRealm->m_sHostileKills, pRealm->m_sPopulationBirths - pRealm->m_sHostileBirths, pRealm->m_sPopulation - pRealm->m_sHostiles);
				ImGui::Text("");
			}

				ImGui::Text("Bullets fired: %d", Stat_BulletsFired);
				ImGui::Text("Bullets hit: %d", Stat_BulletsHit);
				ImGui::Text("Bullets missed: %d", Stat_BulletsMissed);
				ImGui::Text("Deaths: %d", Stat_Deaths);
				ImGui::Text("Suicides: %d", Stat_Suicides);
				ImGui::Text("Executions: %d", Stat_Executions);
				ImGui::Text("Hits Taken: %d", Stat_HitsTaken);
				ImGui::Text("Damage Taken: %d", Stat_DamageTaken);
				ImGui::Text("Burns: %d", Stat_Burns);
				ImGui::Text("Time Running: %d", Stat_TimeRunning);
				ImGui::Text("Killed Hostiles: %d", Stat_KilledHostiles);
				ImGui::Text("Killed Civilians: %d", Stat_KilledCivilians);
				ImGui::Text("Total Killed: %d", Stat_TotalKilled);
				ImGui::Text("Levels Played: %d", Stat_LevelsPlayed);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(sdlRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(sdlRenderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(sdlRenderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
		SDL_RenderPresent(sdlRenderer);
	}

	// Cleanup ImGui
	imgui_up = false;
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return 0;
}