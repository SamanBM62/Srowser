#include "test.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>
#include <algorithm>

FT_Library fontLibrary = nullptr;
FT_Face fontFace = nullptr;

bool initFont(const char* path)
{
    if (FT_Init_FreeType(&fontLibrary))
        return false;

    if (FT_New_Face(fontLibrary, path, 0, &fontFace))
    {
        FT_Done_FreeType(fontLibrary);
        fontLibrary = nullptr;
        return false;
    }

    return true;
}

void destroyFont()
{
    if (fontFace)
        FT_Done_Face(fontFace);

    if (fontLibrary)
        FT_Done_FreeType(fontLibrary);

    fontFace = nullptr;
    fontLibrary = nullptr;
}

// x and baselineY are measured in pixels from the top-left.
// This first version supports single-line ASCII text.
void drawText(
    const std::string& text,
    float x,
    float baselineY,
    unsigned int pixelSize)
{
    if (!fontFace || FT_Set_Pixel_Sizes(fontFace, 0, pixelSize))
        return;

    // Save the OpenGL state that this function changes.
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
                 GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

    GLint previousMatrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &previousMatrixMode);

    // Use pixel coordinates matching the current viewport.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1, 1, 1, 1); // White text.
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned char character : text)
    {
        if (FT_Load_Char(
                fontFace,
                character,
                FT_LOAD_RENDER | FT_LOAD_NO_BITMAP))
            continue;

        FT_GlyphSlot glyph = fontFace->glyph;
        const FT_Bitmap& bitmap = glyph->bitmap;

        if (bitmap.width > 0 && bitmap.rows > 0 &&
            bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
        {
            // Copy rows into tightly packed memory.
            // FreeType's pitch may include padding or be negative.
            std::vector<unsigned char> pixels(
                bitmap.width * bitmap.rows);

            for (unsigned int row = 0; row < bitmap.rows; ++row)
            {
                const unsigned char* source =
                    bitmap.buffer +
                    static_cast<std::ptrdiff_t>(row) * bitmap.pitch;

                std::copy_n(
                    source,
                    bitmap.width,
                    pixels.data() + row * bitmap.width);
            }

            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_ALPHA,
                bitmap.width, bitmap.rows,
                0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels.data());

            float left = x + glyph->bitmap_left;
            float top = baselineY - glyph->bitmap_top;
            float right = left + bitmap.width;
            float bottom = top + bitmap.rows;

            // Two triangles form the character's rectangle.
            glBegin(GL_TRIANGLES);

            glTexCoord2f(0, 0); glVertex2f(left,  top);
            glTexCoord2f(1, 0); glVertex2f(right, top);
            glTexCoord2f(1, 1); glVertex2f(right, bottom);

            glTexCoord2f(0, 0); glVertex2f(left,  top);
            glTexCoord2f(1, 1); glVertex2f(right, bottom);
            glTexCoord2f(0, 1); glVertex2f(left,  bottom);

            glEnd();
        }

        // FreeType stores advance distances in 1/64 pixel units.
        x += glyph->advance.x / 64.0f;
    }

    glDeleteTextures(1, &texture);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(previousMatrixMode);

    glPopClientAttrib();
    glPopAttrib();
}

int draw()
{
    // 1. Connect to the X11 display
    Display* display = XOpenDisplay(nullptr);

    if (!display)
    {
        std::cerr << "Failed to connect to X11 display\n";
        return 1;
    }

    // 2. Choose an OpenGL-compatible visual
    int attributes[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_RED_SIZE,   8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE,  8,
        None
    };

    XVisualInfo* visual =
        glXChooseVisual(display, DefaultScreen(display), attributes);

    if (!visual)
    {
        std::cerr << "Failed to find an OpenGL visual\n";
        XCloseDisplay(display);
        return 1;
    }

    // 3. Create the window
    Colormap colormap = XCreateColormap(
        display,
        RootWindow(display, visual->screen),
        visual->visual,
        AllocNone
    );

    XSetWindowAttributes windowAttributes{};
    windowAttributes.colormap = colormap;
    windowAttributes.event_mask =
        ExposureMask |
        StructureNotifyMask |
        KeyPressMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask;

    Window window = XCreateWindow(
        display,
        RootWindow(display, visual->screen),
        100, 100,
        1280, 720,
        0,
        visual->depth,
        InputOutput,
        visual->visual,
        CWColormap | CWEventMask,
        &windowAttributes
    );

    XStoreName(display, window, "My Custom UI Engine");

    // Allow the window manager's close button to close the program.
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDelete, 1);

    // 4. Create the OpenGL context
    GLXContext context = glXCreateContext(
        display,
        visual,
        nullptr,
        True
    );

    if (!context)
    {
        std::cerr << "Failed to create OpenGL context\n";
        XDestroyWindow(display, window);
        XFreeColormap(display, colormap);
        XFree(visual);
        XCloseDisplay(display);
        return 1;
    }

    // 5. Connect OpenGL to our window
    glXMakeCurrent(display, window, context);

    // 6. Show the window
    XMapWindow(display, window);

    // Configure the initial viewport.
    glViewport(0, 0, 1280, 720);

    // Set the background color.
    glClearColor(0.12f, 0.14f, 0.18f, 1.0f);

    bool running = initFont(
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    if (!running)
        std::cerr << "Failed to load font; check the font path\n";

    // 7. Main application loop
    while (running)
    {
        // Process window and input events.
        while (XPending(display))
        {
            XEvent event;
            XNextEvent(display, &event);

            if (event.type == ClientMessage &&
                static_cast<Atom>(event.xclient.data.l[0]) == wmDelete)
            {
                running = false;
            }

            if (event.type == ConfigureNotify)
            {
                glViewport(
                    0,
                    0,
                    event.xconfigure.width,
                    event.xconfigure.height
                );
            }

            if (event.type == ButtonPress)
            {
                std::cout << "Mouse clicked at "
                          << event.xbutton.x << ", "
                          << event.xbutton.y << '\n';
            }
        }

        // Render with OpenGL.
        glClear(GL_COLOR_BUFFER_BIT);

        drawText("Hello, OpenGL!", 20, 50, 32);

        // Display the rendered frame.
        glXSwapBuffers(display, window);
    }

    // 8. Clean up
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);

    XDestroyWindow(display, window);
    XFreeColormap(display, colormap);
    XFree(visual);
    XCloseDisplay(display);

    return 0;
}