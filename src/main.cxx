#include <GL/glew.h>
#include <GLFW/glfw3.h>

// dear imgui
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// usb
#include <libusb-1.0/libusb.h>

// standard
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <filesystem>
#include <cstdio> // for fprintf
#include <cstdlib> // for getenv
#include <string.h>

// application
#include "settings.h"
#include "settings_manager.h"
#include "Roboto-Medium.h"

const char *input_names[NUM_INPUTS] = {
    "Mic 1", "Mic 2", "Mic 3", "Mic 4",
    "DAW 1", "DAW 2", "DAW 3", "DAW 4",
    "Loop 1", "Loop 2"
};

const char *output_names[NUM_OUTPUTS] = {
    "Out 1", "Out 2", "Out 3", "Out 4"
};

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ users may need to additionnaly link with 'legacy_stdio_definitions.lib' found in every Windows SDK in Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

enum class LayoutMode {
    OutputBased,
    InputBased
};

void set_style() {
    auto& style = ImGui::GetStyle();
    style.FrameRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 4.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

#define EVO_VENDOR_ID 0x2708
#define EVO_4_PRODUCT_ID 0x0006
#define EVO_8_PRODUCT_ID 0x0007

static libusb_device_handle *devh = NULL;

void evo_init()
{
    int err;
    err = libusb_init(NULL);
    if (err != LIBUSB_SUCCESS) {
        printf("libusb_init failed: %s\n", libusb_error_name(err));
    }

    // First try to open an EVO 8
    devh = libusb_open_device_with_vid_pid(NULL, EVO_VENDOR_ID, EVO_8_PRODUCT_ID);
    if (devh == NULL) {

        // Otherwise try EVO 4
        devh = libusb_open_device_with_vid_pid(NULL, EVO_VENDOR_ID, EVO_4_PRODUCT_ID);

        if (devh == NULL) {
            printf("Unable to find an Audient EVO 8 or Audient EVO 4\n");
            exit(1);
        }

    }

    err = libusb_set_auto_detach_kernel_driver(devh, 1);
    if (err < 0) {
        printf("libusb_set_auto_detach_kernel_driver failed: %s\n", libusb_error_name(err));
        exit(1);
    }
    err = libusb_claim_interface(devh, 0);
    if (err < 0) {
        printf("libusb_claim_interface failed: %s\n", libusb_error_name(err));
        exit(1);
    }
}

void evo_close()
{
    if (devh) {
    libusb_reset_device(devh);
    libusb_release_interface(devh, 0);
    libusb_close(devh);
        devh = NULL;
    }
}

void evo_ctrl_volume(int wValue, int wIndex, int32_t volume_dB)
{
    // Volume mapping:
    // It looks like little endian Q8.8 encoded dB values.
    // - Minimum = 0x0080 = 128 = -128 dB
    // - Maximum = 0xefff = 61439 = -0.066 dB

    if (volume_dB <= -100) {
        volume_dB = -128;
    } else if (volume_dB > 0) {
        volume_dB = 0;
    }

    uint16_t v = (uint16_t)(256 * volume_dB);
    uint8_t bytes[2];

    bytes[1] = (v >> 8) & 0xff;
    bytes[0] = (v     ) & 0xff;

    libusb_control_transfer(devh, 0x21, 0x01, wValue, wIndex, bytes, 2, 0);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int argc, char** argv)
{
    evo_init();

    if (argc > 1 && strcmp(argv[1], "--load-settings") == 0) {
        float volumes[NUM_OUTPUTS][NUM_INPUTS];
        load_settings(volumes);
        for (int i = 0; i < NUM_OUTPUTS; i++) {
            for (int j = 0; j < NUM_INPUTS; j++) {
                uint16_t wValue = 0x100 + 4 * j + i;
                evo_ctrl_volume(wValue, 0x3c00, volumes[i][j]);
            }
        }
        evo_close();
        libusb_exit(NULL);
        printf("Settings loaded and applied.\\n");
        return 0;
    }

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "evoctl", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    
    // Force GLEW to ignore missing GLX context if we are on Wayland (EGL) but using standard GLEW
    // This is a hack because standard GLEW tries to probe GLX even if we have an EGL context from GLFW
    // When using static GLEW, we can't easily switch backends at runtime without recompiling.
    // However, if we are in a pure Wayland environment, GLFW uses EGL.
    // If we are in X11/XWayland, GLFW uses GLX/EGL depending on config.
    
    // Try to init normally first
    GLenum err = glewInit();
    
    // If that fails with "No GLX display", it might be because we have a valid context but not a GLX one.
    // In that case, we can try to proceed anyway if we are sure we have a context.
    if (err != GLEW_OK) {
        if (err == GLEW_ERROR_NO_GLX_DISPLAY) {
             fprintf(stderr, "GLEW Warning: No GLX display detected. This is expected on Wayland if using EGL.\n");
             fprintf(stderr, "Attempting to proceed anyway as we have a valid GLFW context...\n");
             // Clear the error and assume success if we have a window
             if (window) {
                 err = GLEW_OK;
             }
        }
    }

    if (err != GLEW_OK) {
        fprintf(stderr, "Failed to initialize OpenGL loader! Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Set IniFilename based on XDG Base Directory Specification
    std::filesystem::path config_dir;
    const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
    const char* home_dir = std::getenv("HOME");

    if (xdg_config_home && xdg_config_home[0] != '\\0') {
        config_dir = xdg_config_home;
    } else if (home_dir && home_dir[0] != '\\0') {
        config_dir = std::filesystem::path(home_dir) / ".config";
    }

    if (!config_dir.empty()) {
        std::filesystem::path app_config_dir = config_dir / "evoctl";
        try {
            std::filesystem::create_directories(app_config_dir);
            static std::string ini_path = (app_config_dir / "imgui.ini").string();
            io.IniFilename = ini_path.c_str();
        } catch (const std::filesystem::filesystem_error& e) {
            fprintf(stderr, "Could not create config directory: %s\\n", e.what());
            io.IniFilename = "imgui.ini"; // Fallback to current directory
        }
    } else {
        io.IniFilename = "imgui.ini"; // Fallback for systems without HOME
    }
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // Load font from file
    io.Fonts->AddFontFromMemoryCompressedTTF(RobotoFont_compressed_data, RobotoFont_compressed_size, 16.0f);

    set_style();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static float volumes[NUM_OUTPUTS][NUM_INPUTS];
    static LayoutMode current_layout = LayoutMode::OutputBased;
    load_settings(volumes);
    for (int i = 0; i < NUM_OUTPUTS; i++) {
        for (int j = 0; j < NUM_INPUTS; j++) {
            uint16_t wValue = 0x100 + 4 * j + i;
            evo_ctrl_volume(wValue, 0x3c00, volumes[i][j]);
        }
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("EVO Mixer", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        if (ImGui::Button("Save")) {
            save_settings(volumes);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            load_settings(volumes);
            for (int i = 0; i < NUM_OUTPUTS; i++) {
                for (int j = 0; j < NUM_INPUTS; j++) {
                    uint16_t wValue = 0x100 + 4 * j + i;
                    evo_ctrl_volume(wValue, 0x3c00, volumes[i][j]);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Switch Layout")) {
            current_layout = (current_layout == LayoutMode::OutputBased) ? LayoutMode::InputBased : LayoutMode::OutputBased;
                }
        ImGui::Separator();

        // Calculate heights for responsive layout
        ImGuiStyle& style = ImGui::GetStyle();
        float available_height = ImGui::GetContentRegionAvail().y;
        
        // --- Output Layout Calculation ---
        float output_total_spacing = (NUM_OUTPUTS > 1) ? (style.ItemSpacing.y * (NUM_OUTPUTS - 1)) : 0;
        float output_net_height = available_height - output_total_spacing;
        float output_row_height = output_net_height / NUM_OUTPUTS;
        if (output_row_height < 0) output_row_height = 0;
        float output_slider_height = output_row_height > 0 ? output_row_height - ImGui::GetTextLineHeightWithSpacing() * 3 : 0;

        // --- Input Layout Calculation ---
        float input_total_spacing = style.ItemSpacing.y * 2; // 2 gaps between the 3 groups
        float input_group_header_height = ImGui::GetTextLineHeightWithSpacing() * 2; // Header + separator for each of 3 groups
        float input_total_header_height = input_group_header_height * 3;
        float input_net_available_height = available_height - input_total_header_height - input_total_spacing;
        float input_child_height = input_net_available_height / 3.0f;
        if (input_child_height < 150) input_child_height = 150; // Set a reasonable minimum height for usability
        
        if (current_layout == LayoutMode::OutputBased) {
            for (int i = 0; i < NUM_OUTPUTS; i++) {
                ImGui::BeginChild(output_names[i], ImVec2(0, output_row_height), false, ImGuiWindowFlags_NoScrollbar);
                ImGui::Text("%s", output_names[i]);
                ImGui::Separator();

                if (ImGui::BeginTable("OutputTable", NUM_INPUTS)) {
                    for (int j = 0; j < NUM_INPUTS; j++) {
                        ImGui::TableNextColumn();
                        ImGui::PushID(i * 100 + j);

                        float slider_width = ImGui::GetColumnWidth() * 0.5f;
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - slider_width) * 0.5f);

                        if (ImGui::VSliderFloat("##v", ImVec2(slider_width, output_slider_height), &volumes[i][j], -100.0f, 0.0f, "%.0f dB")) {
                            uint16_t wValue = 0x100 + 4 * j + i;
                            evo_ctrl_volume(wValue, 0x3c00, volumes[i][j]);
                        }

                        float text_width = ImGui::CalcTextSize(input_names[j]).x;
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - text_width) * 0.5f);
                        ImGui::Text("%s", input_names[j]);

                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }
        } else { // InputBased Layout
            auto render_input_table = [&](const char* table_id, int start_input, int num_inputs) {
                if (ImGui::BeginTable(table_id, num_inputs, ImGuiTableFlags_SizingStretchSame)) {
                    // Row 1: Headers
                    for (int j = 0; j < num_inputs; ++j) {
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", input_names[start_input + j]);
                    }

                    // Row 2: Separators
                    ImGui::TableNextRow();
                    for (int j = 0; j < num_inputs; ++j) {
                        ImGui::TableNextColumn();
                        ImGui::Separator();
                    }

                    // Rows 3+: Sliders
                    for (int i = 0; i < NUM_OUTPUTS; i++) {
                        ImGui::TableNextRow();
                        for (int j = 0; j < num_inputs; j++) {
                            ImGui::TableNextColumn();
                            int input_index = start_input + j;
                            
                            ImGui::PushID(i * 100 + input_index);

                            char label[32];
                            sprintf(label, "To %s", output_names[i]);

                            // Manually render label and slider on the same line for perfect alignment
                            ImGui::AlignTextToFramePadding();
                            ImGui::TextUnformatted(label);
                            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

                            ImGui::PushItemWidth(-1.0f);
                            // Use "##" to hide the slider's label, as we're rendering it manually
                            if (ImGui::SliderFloat("##slider", &volumes[i][input_index], -100.0f, 0.0f, "%.0f dB")) {
                                uint16_t wValue = 0x100 + 4 * input_index + i;
                                evo_ctrl_volume(wValue, 0x3c00, volumes[i][input_index]);
                            }
                            ImGui::PopItemWidth();
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                }
            };
            
            ImGui::Text("Microphone Inputs");
            ImGui::Separator();
            ImGui::BeginChild("MicGroup", ImVec2(0, input_child_height), false, ImGuiWindowFlags_NoScrollbar);
            render_input_table("MicTable", INPUT_MIC1, 4);
            ImGui::EndChild();

            ImGui::Text("DAW Inputs");
            ImGui::Separator();
            ImGui::BeginChild("DawGroup", ImVec2(0, input_child_height), false, ImGuiWindowFlags_NoScrollbar);
            render_input_table("DawTable", INPUT_DAW1, 4);
            ImGui::EndChild();

            ImGui::Text("Loopback Inputs");
            ImGui::Separator();
            ImGui::BeginChild("LoopbackGroup", ImVec2(0, input_child_height), false, ImGuiWindowFlags_NoScrollbar);
            render_input_table("LoopbackTable", INPUT_LOOPBACK1, 2);
            ImGui::EndChild();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    evo_close();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    libusb_exit(NULL);

    return 0;
}
