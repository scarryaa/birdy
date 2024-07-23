#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace Editor {
    struct Position {
        int line;
        int column;
    };

    class Document {
    public:
        virtual ~Document() = default;
        virtual std::string GetText() const = 0;
        virtual void Insert(const Position& pos, const std::string& text) = 0;
        virtual void Delete(const Position& start, const Position& end) = 0;
    };

    class View {
    public:
        virtual ~View() = default;
        virtual void Draw() = 0;
        virtual void HandleInput(/* input event */) = 0;
    };

    class Platform {
    public:
        virtual ~Platform() = default;
        virtual void CreateWindow(int width, int height) = 0;
        virtual void PumpEvents() = 0;
    };
}

// Windows implementation
#ifdef _WIN32
#include <windows.h>
namespace Editor {
    class WindowsPlatform : public Platform {
    public:
        void CreateWindow(int width, int height) override {
            // Windows-specific window creation
            WNDCLASS wc = {0};
            wc.lpfnWndProc = WindowProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = L"EditorWindowClass";
            RegisterClass(&wc);

            hwnd = CreateWindowEx(
                0, L"EditorWindowClass", L"Editor",
                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                width, height, NULL, NULL, GetModuleHandle(NULL), NULL
            );

            if (hwnd == NULL) {
                throw std::runtime_error("Failed to create window");
            }

            ShowWindow(hwnd, SW_SHOW);
        }

        void PumpEvents() override {
            MSG msg;
            while (GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

    private:
        HWND hwnd;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            switch (uMsg) {
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_PAINT:
                    std::cout << "Paint event\n";
                    break;
                case WM_KEYDOWN:
                    std::cout << "Key press event\n";
                    break;
                case WM_LBUTTONDOWN:
                    std::cout << "Mouse button press event\n";
                    break;
                case WM_LBUTTONUP:
                    std::cout << "Mouse button release event\n";
                    break;
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    };
}
#endif

// macOS implementation
#ifdef __APPLE__
#import <Cocoa/Cocoa.h>

@interface EditorWindow : NSWindow
@end

@implementation EditorWindow
- (void)keyDown:(NSEvent *)event {
    std::cout << "Key press event\n";
}
- (void)mouseDown:(NSEvent *)event {
    std::cout << "Mouse button press event\n";
}
- (void)mouseUp:(NSEvent *)event {
    std::cout << "Mouse button release event\n";
}
@end

namespace Editor {
    class MacPlatform : public Platform {
    public:
        void CreateWindow(int width, int height) override {
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

            NSRect frame = NSMakeRect(100, 100, width, height);
            window = [[EditorWindow alloc] initWithContentRect:frame
                                                    styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
            [window makeKeyAndOrderFront:nil];

            [NSApp activateIgnoringOtherApps:YES];
        }

        void PumpEvents() override {
            [NSApp run];
        }

    private:
        EditorWindow* window;
    };
}
#endif

// Linux implementation
#ifdef __linux__
#include <X11/Xlib.h>
namespace Editor {
    class LinuxPlatform : public Platform {
    public:
        void CreateWindow(int width, int height) override {
            display = XOpenDisplay(nullptr);
            if (display == nullptr) {
                throw std::runtime_error("Failed to open X display");
            }
            int screen = DefaultScreen(display);
            window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));
            XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
            XMapWindow(display, window);
            XFlush(display);
        }

        void PumpEvents() override {
            XEvent event;
            while (true) {
                XNextEvent(display, &event);
                if (event.type == Expose) {
                    std::cout << "Expose event\n";
                } else if (event.type == KeyPress) {
                    std::cout << "Key press event\n";
                    break; // Exit the loop on key press
                } else if (event.type == ButtonPress) {
                    std::cout << "Mouse button press event at (" << event.xbutton.x << ", " << event.xbutton.y << ")\n";
                } else if (event.type == ButtonRelease) {
                    std::cout << "Mouse button release event\n";
                }
            }
            XCloseDisplay(display);
        }

    private:
        Display* display;
        Window window;
    };
}
#endif
