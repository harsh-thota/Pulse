#include "clayman.hpp"
#include "application.hpp"
#include "screens/performance_screen.hpp"
#include "screens/processes_screen.hpp"
#include "screens/network_screen.hpp"
#include "screens/alerts_screen.hpp"
#include "components/graph_component.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "clay.h"
#include <windows.h>

// Clay SDL2 renderer function - defined in external file
extern "C" {
    extern Clay_Dimensions SDL2_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);
    extern void Clay_SDL2_Render(SDL_Renderer *renderer, Clay_RenderCommandArray renderCommands, SDL2_Font *fonts);
}

static void HandleClayErrors(Clay_ErrorData errorData)
{
    std::cerr << "[Clay Error]: " << errorData.errorText.chars << "\n";
}

bool Application::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    window_ = SDL_CreateWindow("Pulse - System Monitor", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        1200, 800, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window_)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        TTF_Quit(); SDL_Quit();
        return false;
    }

    // Keep window at a comfortable minimum size so layouts aren’t squished
    SDL_SetWindowMinimumSize(window_, 800, 600);

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window_);
        TTF_Quit(); SDL_Quit();
        return false;
    }

    // Turn on linear filtering so our text stays crisp when resized
    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == SDL_FALSE) {
        std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
    }

    // Try loading Roboto; if it’s missing, fall back to a Windows font so nothing breaks
    bodyFont_ = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 12);
    if (!bodyFont_)
    {
        std::cerr << "Failed to load custom font, trying system font...\n";
        // Try common Windows system fonts as fallback
        bodyFont_ = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 12);
        if (!bodyFont_)
        {
            bodyFont_ = TTF_OpenFont("C:/Windows/Fonts/segoeui.ttf", 12);
            if (!bodyFont_)
            {
                std::cerr << "Failed to load any fonts! TTF_Error: " << TTF_GetError() << "\n";
                std::cerr << "Please download Roboto font and place it in assets/fonts/Roboto-Regular.ttf\n";
                return false;
            }
        }
        std::cout << "Using system font as fallback\n";
    }

    fonts_[0] = { 0, bodyFont_ };

    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    clayMan_ = std::make_unique<ClayMan>(w, h, SDL2_MeasureText, fonts_);

    dataCollector_ = std::make_unique<DataCollector>();
    if (!dataCollector_->Initialize())
    {
        std::cerr << "Failed to initialize system monitor\n";
        return false;
    }

    // Initialize modular screens
    screens_[Screen::Performance] = std::make_unique<PerformanceScreen>();
    screens_[Screen::Processes] = std::make_unique<ProcessesScreen>();
    screens_[Screen::Network] = std::make_unique<NetworkScreen>();
    screens_[Screen::Alerts] = std::make_unique<AlertsScreen>();

    std::cout << "Pulse Initialized - All screens loaded successfully\n";
    return true;
}

void Application::Run()
{
    bool running = true;
    SDL_Event event;
    
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    while (running)
    {
        int mouseX = 0, mouseY = 0;
        bool mousePressed = false;
        Clay_Vector2 scrollDelta = {};

        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                int width, height;
                SDL_GetWindowSize(window_, &width, &height);
                // Handle window resize properly
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                mousePressed = true;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                // Improved scroll handling for vertical scrolling
                scrollDelta.x = static_cast<float>(event.wheel.x * 30); // Horizontal scroll
                scrollDelta.y = static_cast<float>(event.wheel.y * 30); // Vertical scroll (more responsive)
            }
        }
        
        if (!running) break;
        
    // Figure out how long the last frame took (for smooth timing)
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        int width, height;
        SDL_GetWindowSize(window_, &width, &height);
        clayMan_->updateClayState(width, height, 
            static_cast<float>(mouseX), static_cast<float>(mouseY),
            scrollDelta.x, scrollDelta.y, 
            static_cast<float>(deltaTime), mousePressed);

        Update();
        Render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Application::Update()
{
    if (dataCollector_)
    {
        dataCollector_->Update();
    }
}

void Application::Render()
{
    if (!clayMan_ || !dataCollector_) return;
    
    // Paint the window black (perfect backdrop for our data)
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    
    // Get Clay render commands and render them
    clayMan_->beginLayout();
    
    // Render the UI elements
    RenderUIElements();
    
    Clay_RenderCommandArray renderCommands = clayMan_->endLayout();
    Clay_SDL2_Render(renderer_, renderCommands, fonts_);
    
    SDL_RenderPresent(renderer_);
    // Trim working set to minimize memory footprint in Task Manager
#ifdef _WIN32
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
#endif
}

void Application::RenderUIElements()
{
    const SystemState& systemState = dataCollector_->GetSystemState();
    
    // Main layout: sidebar on the left, content on the right
    Clay_ElementDeclaration appContainer = {};
    appContainer.layout.sizing = clayMan_->expandXY();
    appContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    
    clayMan_->element(appContainer, [this, &systemState]() {
        RenderModernSidebar();
        RenderScrollableMainContent();
    });
}

void Application::RenderUI()
{
    // This method is now just for compatibility
    // The actual rendering is handled in the Render() method
}

void Application::RenderModernSidebar()
{
    uint32_t sidebarWidth = 240;
    
    Clay_ElementDeclaration sidebar = {};
    sidebar.layout.sizing = clayMan_->fixedSize(sidebarWidth, clayMan_->getWindowHeight());
    sidebar.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    sidebar.layout.padding = clayMan_->padAll(20);
    sidebar.layout.childGap = 12;
    sidebar.backgroundColor = { 0, 0, 0, 255 }; // Darker sidebar
    
    clayMan_->element(sidebar, [this]() {
    // Up top: that little Pulsing P icon and the app name
        Clay_ElementDeclaration brandSection = {};
        brandSection.layout.sizing = clayMan_->expandXfixedY(80);
        brandSection.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        brandSection.layout.padding = clayMan_->padXY(8, 16);
        
        clayMan_->element(brandSection, [this]() {
            Clay_ElementDeclaration brandContainer = {};
            brandContainer.layout.sizing = clayMan_->expandXY();
            brandContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            brandContainer.layout.childGap = 12;
            brandContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            
            clayMan_->element(brandContainer, [this]() {
                // Icon/Logo
                Clay_ElementDeclaration iconContainer = {};
                iconContainer.layout.sizing = clayMan_->fixedSize(40, 40);
                iconContainer.backgroundColor = { 0, 255, 150, 255 }; // Brand green
                iconContainer.cornerRadius = { 8, 8, 8, 8 };
                iconContainer.layout.childAlignment = clayMan_->centerXY();
                
                clayMan_->element(iconContainer, [this]() {
                    Clay_TextElementConfig iconText = {};
                    iconText.textColor = { 25, 25, 25, 255 };
                    iconText.fontId = 0;
                    iconText.fontSize = 20;
                    clayMan_->textElement("P", iconText);
                });
                
                // Brand name
                Clay_ElementDeclaration nameContainer = {};
                nameContainer.layout.sizing = clayMan_->expandXY();
                nameContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                
                clayMan_->element(nameContainer, [this]() {
                    Clay_TextElementConfig brandText = {};
                    brandText.textColor = { 240, 240, 240, 255 };
                    brandText.fontId = 0;
                    brandText.fontSize = 22;
                    clayMan_->textElement("Pulse", brandText);
                });
            });
        });
        
    // A neat dividing line to keep things organized
        Clay_ElementDeclaration separator = {};
        separator.layout.sizing = clayMan_->expandXfixedY(1);
        separator.backgroundColor = { 45, 45, 45, 255 };
        clayMan_->element(separator, []() {});
        
    // Here’s where you click to jump between different views
        Clay_ElementDeclaration navSection = {};
        navSection.layout.sizing = clayMan_->expandXY();
        navSection.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        navSection.layout.childGap = 8;
        navSection.layout.padding = clayMan_->padY(16);
        
        clayMan_->element(navSection, [this]() {
        RenderModernNavButton("", "Performance", Screen::Performance, currentScreen_ == Screen::Performance);
        RenderModernNavButton("", "Processes", Screen::Processes, currentScreen_ == Screen::Processes);
        RenderModernNavButton("", "Network", Screen::Network, currentScreen_ == Screen::Network);
        RenderModernNavButton("", "Alerts", Screen::Alerts, currentScreen_ == Screen::Alerts);
        });
    });
}

void Application::RenderModernNavButton(const std::string& icon, const std::string& label, Screen screen, bool isActive)
{
    Clay_ElementDeclaration button = {};
    // Assign a unique ID so I can catch when you hover or click
    button.id = clayMan_->hashID(label);
    button.layout.sizing = clayMan_->expandXfixedY(48);
    button.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
    button.layout.padding = clayMan_->padXY(16, 12);
    
    // Style the button – active ones get a subtle green glow
    if (isActive) {
        button.backgroundColor = { 0, 255, 150, 25 }; // Subtle green background
        // Border configuration for Clay
        button.border.width = {1, 1, 1, 1}; // All sides 1 pixel
        button.border.color = { 0, 255, 150, 100 };
    } else {
        button.backgroundColor = { 35, 35, 35, 255 };
    }
    button.cornerRadius = { 12, 12, 12, 12 };
    
    clayMan_->element(button, [this, icon, label, screen, isActive]() {
    // If you click this while hovering, we flip to that screen
        if (clayMan_->pointerOver(label) && clayMan_->mousePressed()) {
            SwitchToScreen(screen);
        }
        
        Clay_ElementDeclaration content = {};
        content.layout.sizing = clayMan_->expandXY();
        content.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        content.layout.childGap = 16;
        content.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        
    clayMan_->element(content, [this, icon, label, screen, isActive]() {
            // Icon
            Clay_ElementDeclaration iconContainer = {};
            iconContainer.layout.sizing = clayMan_->fixedSize(24, 24);
            iconContainer.layout.childAlignment = clayMan_->centerXY();
            
            clayMan_->element(iconContainer, [this, &icon, isActive]() {
                Clay_TextElementConfig iconText = {};
                iconText.textColor = isActive ? 
                    Clay_Color{ 0, 255, 150, 255 } : Clay_Color{ 180, 180, 180, 255 };
                iconText.fontId = 0;
                iconText.fontSize = 16;
                clayMan_->textElement(icon, iconText);
            });
            
            // Label
            Clay_ElementDeclaration labelContainer = {};
            // Clip text to container width to avoid overflow
            labelContainer.clip.horizontal = true;
            labelContainer.clip.vertical = false;
            labelContainer.layout.sizing = clayMan_->expandXY();
            labelContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            
            clayMan_->element(labelContainer, [this, &label, isActive]() {
                Clay_TextElementConfig labelText = {};
                labelText.textColor = isActive ? 
                    Clay_Color{ 240, 240, 240, 255 } : Clay_Color{ 160, 160, 160, 255 };
                labelText.fontId = 0;
                labelText.fontSize = 14;
                clayMan_->textElement(label, labelText);
            });
        });
    });
}

void Application::RenderScrollableMainContent()
{
    const SystemState& systemState = dataCollector_->GetSystemState();
    uint32_t sidebarWidth = 240;
    
    Clay_ElementDeclaration mainContent = {};
    mainContent.layout.sizing = clayMan_->fixedSize(clayMan_->getWindowWidth() - sidebarWidth, clayMan_->getWindowHeight());
    mainContent.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    mainContent.backgroundColor = { 0, 0, 0, 255 };
    
    clayMan_->element(mainContent, [this, &systemState]() {
        // Create scrollable container with proper configuration
    Clay_ElementDeclaration scrollContainer = {};
    scrollContainer.layout.sizing = clayMan_->expandXY();
    scrollContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    scrollContainer.layout.padding = clayMan_->padAll(24);
    scrollContainer.layout.childGap = 20;
    // Clip contents to the scroll container bounds
    scrollContainer.clip.horizontal = true;
    scrollContainer.clip.vertical = true;
        
        clayMan_->element(scrollContainer, [this, &systemState]() {
            // Render current screen content
            if (screens_.find(currentScreen_) != screens_.end()) {
                screens_[currentScreen_]->Render(clayMan_.get(), systemState);
            }
            
            // Add bottom spacer for better scrolling
            Clay_ElementDeclaration bottomSpacer = {};
            bottomSpacer.layout.sizing = clayMan_->expandXfixedY(100);
            clayMan_->element(bottomSpacer, []() {});
        });
    });
}

void Application::RenderMainContent()
{
    // Use the new scrollable version
    RenderScrollableMainContent();
}

void Application::RenderNavButton(const std::string& icon, Screen screen, bool isActive)
{
    // Use the new modern version
    std::string label;
    switch (screen) {
        case Screen::Performance: label = "Performance"; break;
        case Screen::Processes: label = "Processes"; break;
        case Screen::Network: label = "Network"; break;
        case Screen::Alerts: label = "Alerts"; break;
    }
    RenderModernNavButton(icon, label, screen, isActive);
}

void Application::RenderSidebar()
{
    // Use the new modern version
    RenderModernSidebar();
}

void Application::SwitchToScreen(Screen screen)
{
    currentScreen_ = screen;
}

void Application::Shutdown()
{
    dataCollector_.reset();
    clayMan_.reset();
    
    if (bodyFont_)
    {
        TTF_CloseFont(bodyFont_);
        bodyFont_ = nullptr;
    }
    
    if (renderer_)
    {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if (window_)
    {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

std::string Application::FormatBytes(uint64_t bytes)
{
    std::ostringstream oss;
    if (bytes >= 1024ULL * 1024 * 1024 * 1024) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024ULL * 1024 * 1024 * 1024)) << "TB";
    } else if (bytes >= 1024ULL * 1024 * 1024) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024ULL * 1024 * 1024)) << "GB";
    } else if (bytes >= 1024ULL * 1024) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024ULL * 1024)) << "MB";
    } else if (bytes >= 1024ULL) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / 1024ULL) << "KB";
    } else {
        oss << bytes << "B";
    }
    return oss.str();
}

std::string Application::FormatPercentage(float percentage)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percentage << "%";
    return oss.str();
}