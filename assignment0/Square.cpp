// HW 0 - Moving Square
// Starter code for the first homework assignment.
// This code assumes SDL2 and OpenGL are both properly installed on your system

#include "glad/glad.h"  //Include order can matter here
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cmath>
#include <cstdio>

// Globals to store the state of the square (position, width, and angle)
float g_pos_x = 0.0f;
float g_pos_y = 0.0f;
float g_size = 0.25f;
float g_angle =
    M_PI_4;  // Set corners at 45-degree angles from the center of the center.

static const int V_SZ = 8;
float vertices[V_SZ];

#ifdef SCREEN_HEIGHT
int screen_height = SCREEN_HEIGHT;
#else
int screen_height = 800;
#endif

#ifdef SCREEN_WIDTH
int screen_width = SCREEN_WIDTH;
#else
int screen_width = 800;
#endif

float aspect = screen_width / (float)screen_height;  // aspect ratio (needs to
                                                     // be updated if the window
                                                     // is resized)

float g_mouse_down = false;

void mouseClicked(float mx, float my);
void mouseDragged(float mx, float my);

bool g_bTranslate = false;
bool g_bRotate = false;
bool g_bScale = false;

//////////////////////////
///  Begin your code here
/////////////////////////

// These variables are global to allow redrawing the screen outside of main
GLint uniColor;
SDL_Window *window;

// My functions
float *getRelativeXY(float m_x, float m_y);
void redrawScreen();
void changeColor();

void updateVertices() {
  // Distance from center of square to corner
  float radius = g_size * std::sqrt(2);

  vertices[0] = g_pos_x + (radius * std::cos(g_angle));  // Top right x
  vertices[1] = g_pos_y + (radius * std::sin(g_angle));  // Top right y

  vertices[2] = g_pos_x + (radius * std::sin(g_angle));  // Bottom right x
  vertices[3] = g_pos_y - (radius * std::cos(g_angle));  // Bottom right y

  vertices[4] = g_pos_x - (radius * std::sin(g_angle));  // Top left x
  vertices[5] = g_pos_y + (radius * std::cos(g_angle));  // Top left y

  vertices[6] = g_pos_x - (radius * std::cos(g_angle));  // Bottom left x
  vertices[7] = g_pos_y - (radius * std::sin(g_angle));  // Bottom left y

  for (int i = 0; i < V_SZ; i += 2) {
    vertices[i] /= aspect;
  }
}

void mouseClicked(float m_x, float m_y) {
  g_bTranslate = false;
  g_bRotate = false;
  g_bScale = false;

  float t = 0.1;  // edge/corner threshold, portion of square size

  float *relative = getRelativeXY(m_x, m_y);
  float x = relative[0];
  float y = relative[1];
  free(relative);

  printf("\nMouse clicked: ");
  printf("Relative x, y: %f, %f\n", x, y);

  if (x > 1 or x < -1 or y > 1 or y < -1) {
    printf("Outside of square\n");
    return;
  }
  if (x > 1 - t or x < t - 1) {
    if (y > 1 - t or y < t - 1) {
      printf("Corner of square\n");
      g_bRotate = true;
      return;
    }
  }
  if (x > 1 - t or x < t - 1 or y < t - 1 or y > 1 - t) {
    printf("Edge of square\n");
    g_bScale = true;
    return;
  }

  printf("Inside of square\n");
  g_bTranslate = true;

  // Smooth transition to new position
  float increment = 0.1;
  for (float p = 0.; p < 1.; p += increment) {
    g_pos_x += p * (m_x - g_pos_x);
    g_pos_y += p * (m_y - g_pos_y);
    updateVertices();
    redrawScreen();
  }
}

void mouseDragged(float m_x, float m_y) {
  // printf("Dragging through %f, %f\n", m_x, m_y);
  // printf("g_pos_x, g_pos_y = %f, %f\n", g_pos_x, g_pos_y);
  // printf("g_size = %f\n", g_size);

  if (g_bTranslate) {
    g_pos_x = m_x;
    g_pos_y = m_y;
  } else if (g_bScale) {
    float *relative = getRelativeXY(m_x, m_y);
    g_size = fmax(fabs(g_size * relative[1]), fabs(g_size * relative[0]));
    free(relative);
  } else if (g_bRotate) {
    float x_diff = m_x - g_pos_x;
    float y_diff = m_y - g_pos_y;
    g_angle = std::atan(y_diff / x_diff);
  } else {
    return;
  }

  updateVertices();
}

// Returns the X and Y coordinates of the mouse relative to the square on the
// screen. The square is un-transoformed to be a square at the origin with size
// 1 and angle pi/4, where the corners are at 45-degree angles from the origin.
float *getRelativeXY(float m_x, float m_y) {
  float x_ = (m_x - g_pos_x) / g_size;
  float y_ = (m_y - g_pos_y) / g_size;

  float angle = -1 * (g_angle + M_PI_4);
  float sin_angle = std::sin(angle);
  float cos_angle = std::cos(angle);

  // Taken from
  // https://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d
  float x = x_ * cos_angle - y_ * sin_angle;
  float y = x_ * sin_angle + y_ * cos_angle;
  float *r = (float *)malloc((sizeof(float) * 2));
  r[0] = x;
  r[1] = y;
  return r;
}

void redrawScreen() {
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
               GL_DYNAMIC_DRAW);  // upload vertices to vbo

  // Clear the screen to black
  glClearColor(.2f, 0.4f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  SDL_GL_SwapWindow(window);  // Double buffering
}

// Changes the color of the square
void changeColor() {
  enum Color { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
  static Color color = ORANGE;
  printf("Changing color\n");
  switch (color) {
    case RED:
      glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);
      color = ORANGE;
      break;
    case ORANGE:
      glUniform3f(uniColor, 1.0f, 0.5f, 0.0f);
      color = YELLOW;
      break;
    case YELLOW:
      glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);
      color = GREEN;
      break;
    case GREEN:
      glUniform3f(uniColor, 0.0f, 1.0f, 0.0f);
      color = BLUE;
      break;
    case BLUE:
      glUniform3f(uniColor, 0.0f, 0.0f, 1.0f);
      color = PURPLE;
      break;
    case PURPLE:
      glUniform3f(uniColor, 1.0f, 0.0f, 1.0f);
      color = RED;
  }
}

/////////////////////////////
/// ... you can ignore any code below (for now)
////////////////////////////

// Shader sources
const GLchar *vertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "void main() {"
    "   gl_Position = vec4(position, 0.0, 1.0);"
    "}";

const GLchar *fragmentSource =
    "#version 150 core\n"
    "uniform vec3 triangleColor;"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(triangleColor, 1.0);"
    "}";

bool fullscreen = false;

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);  // Initialize Graphics (for OpenGL)

  // Ask SDL to get a recent version of OpenGL (3.2 or greater)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  // Create a window (offsetx, offsety, width, height, flags)
  window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width,
                            screen_height, SDL_WINDOW_OPENGL);

  // The above window cannot be resized which makes some code slightly easier.
  // Below show how to make a full screen window or allow resizing
  // SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0,
  // screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
  // SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100,
  // screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
  // SDL_Window* window = SDL_CreateWindow("My OpenGL
  // Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL);
  // //Boarderless window "fake" full screen

  updateVertices();  // set initial position of the square to match it's state

  // Create a context to draw in
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // OpenGL functions using glad library
  if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    printf("OpenGL loaded\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
  } else {
    printf("ERROR: Failed to initialize OpenGL context.\n");
    return -1;
  }

  // Build a Vertex Array Object. This stores the VBO and attribute mappings in
  // one object
  GLuint vao;
  glGenVertexArrays(1, &vao);  // Create a VAO
  glBindVertexArray(vao);  // Bind the above created VAO to the current context

  // Allocate memory on the graphics card to store geometry (vertex buffer
  // object)
  GLuint vbo;
  glGenBuffers(1, &vbo);               // Create 1 buffer called vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);  // Set the vbo as the active array buffer
                                       // (Only one buffer can be active at a
                                       // time)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
               GL_DYNAMIC_DRAW);  // upload vertices to vbo
  // GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW =
  // geometry changes infrequently  GL_STREAM_DRAW = geom. changes frequently.
  // This effects which types of GPU memory is used

  // Load the vertex Shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  // Let's double check the shader compiled
  GLint status;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char buffer[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
    printf("Vertex Shader Compile Failed. Info:\n\n%s\n", buffer);
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  // Double check the shader compiled
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char buffer[512];
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
    printf("Fragment Shader Compile Failed. Info:\n\n%s\n", buffer);
  }

  // Join the vertex and fragment shaders together into one program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");  // set output
  glLinkProgram(shaderProgram);                          // run the linker

  glUseProgram(
      shaderProgram);  // Set the active shader (only one can be used at a time)

  // Tell OpenGL how to set fragment shader input
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  // Attribute, vals/attrib., type, normalized?, stride, offset
  // Binds to VBO current GL_ARRAY_BUFFER
  glEnableVertexAttribArray(posAttrib);

  uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
  glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

  // Event Loop (Loop forever processing each event as fast as possible)
  SDL_Event windowEvent;
  bool done = false;
  while (!done) {
    while (SDL_PollEvent(
        &windowEvent)) {  // Process input events (e.g., mouse & keyboard)
      if (windowEvent.type == SDL_QUIT) done = true;
      // List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch
      // many special keys  Scancode referes to a keyboard position, keycode
      // referes to the letter (e.g., EU keyboards)
      if (windowEvent.type == SDL_KEYUP &&
          windowEvent.key.keysym.sym == SDLK_ESCAPE)
        done = true;  // Exit event loop
      if (windowEvent.type == SDL_KEYUP &&
          windowEvent.key.keysym.sym == SDLK_f)  // If "f" is pressed
        fullscreen = !fullscreen;

      // Change square color when Space is pressed
      if (windowEvent.type == SDL_KEYUP &&
          windowEvent.key.keysym.sym == SDLK_SPACE)
        changeColor();

      SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN
                                                 : 0);  // Set to full screen
    }

    int mx, my;
    if (SDL_GetMouseState(&mx, &my) &
        SDL_BUTTON(SDL_BUTTON_LEFT)) {  // Is the mouse down?

      SDL_GetWindowSize(window, &screen_width, &screen_height);
      aspect = screen_width / (float)screen_height;
      // printf("aspect = %f\n", aspect);

      float m_x = (2 * mx / (float)screen_width - 1) * aspect;
      float m_y = 1 - 2 * my / (float)screen_height;
      // printf("\nMouse at %f, %f\n", m_x, m_y);

      if (g_mouse_down == false) {
        mouseClicked(m_x, m_y);
      } else {
        mouseDragged(m_x, m_y);
      }
      g_mouse_down = true;
    } else {
      g_mouse_down = false;
    }

    redrawScreen();
  }

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &vbo);

  glDeleteVertexArrays(1, &vao);

  // Clean Up
  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}
