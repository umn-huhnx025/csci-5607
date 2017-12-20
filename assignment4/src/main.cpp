#include "glad/glad.h"  //Include order can matter here

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "map.h"

int screenWidth = 800;
int screenHeight = 600;
float timePast = 0;

Map* m;
glm::vec3 camPos, viewPoint;
float lookAngle;
static const float frustNear = 0.25;
static const float frustFar = 10;
static const float frustAngle = 3.14 / 6.;

bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

// srand(time(NULL));
float rand01() { return rand() / (float)RAND_MAX; }

void drawGeometry(int shaderProgram, int numVertsFloor, int numVertsWall,
                  int numVertsKey);

int main(int argc, char* argv[]) {
  if (argc < 2)
    m = new Map("maps/2.txt");
  else
    m = new Map(argv[1]);

  SDL_Init(SDL_INIT_VIDEO);  // Initialize Graphics (for OpenGL)

  // Ask SDL to get a recent version of OpenGL (3 or greater)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);//

  // Create a window (offsetx, offsety, width, height, flags)
  SDL_Window* window = SDL_CreateWindow("A-Maze-Balls", 0, 0, screenWidth,
                                        screenHeight, SDL_WINDOW_OPENGL);

  // Create a context to draw in
  SDL_GLContext context = SDL_GL_CreateContext(window);

  SDL_bool captureMouse = SDL_TRUE;
  SDL_SetRelativeMouseMode(captureMouse);

  // Load OpenGL extentions with GLAD
  if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    printf("\nOpenGL loaded\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n\n", glGetString(GL_VERSION));
  } else {
    printf("ERROR: Failed to initialize OpenGL context.\n");
    return -1;
  }

  // Load floor model
  std::ifstream modelFile;
  modelFile.open("models/floor.txt");
  int numLines = 0;
  modelFile >> numLines;
  float* modelFloor = new float[numLines];
  for (int i = 0; i < numLines; i++) {
    modelFile >> modelFloor[i];
  }
  int numVertsFloor = numLines / 8;
  modelFile.close();

  // Load Model 1
  modelFile.open("models/cube.txt");
  numLines = 0;
  modelFile >> numLines;
  float* model1 = new float[numLines];
  for (int i = 0; i < numLines; i++) {
    modelFile >> model1[i];
  }
  int numVertsWall = numLines / 8;
  modelFile.close();

  // Load Model 2
  modelFile.open("models/sphere.txt");
  numLines = 0;
  modelFile >> numLines;
  float* model2 = new float[numLines];
  for (int i = 0; i < numLines; i++) {
    modelFile >> model2[i];
  }
  int numVertsKey = numLines / 8;
  modelFile.close();

  // SJG: I load each model in a different array, then concatenate everything in
  // one big array
  //     There are better options, but this works.
  // Concatenate model arrays
  float* modelData =
      new float[(numVertsFloor + numVertsWall + numVertsKey) * 8];
  std::copy(modelFloor, modelFloor + numVertsFloor * 8, modelData);
  std::copy(model1, model1 + numVertsWall * 8, modelData + numVertsFloor * 8);
  std::copy(model2, model2 + numVertsKey * 8,
            modelData + (numVertsWall + numVertsFloor) * 8);
  int totalNumVerts = numVertsWall + numVertsKey + numVertsFloor;

  //// Allocate Texture 0 (Gravel) ///////
  SDL_Surface* surface = SDL_LoadBMP("textures/gravel.bmp");
  if (surface == NULL) {  // If it failed, print the error
    printf("Error: \"%s\"\n", SDL_GetError());
    return 1;
  }
  GLuint tex0;
  glGenTextures(1, &tex0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex0);

  // What to do outside 0-1 range
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load the texture into memory
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGR,
               GL_UNSIGNED_BYTE, surface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  SDL_FreeSurface(surface);
  //// End Allocate Texture ///////

  //// Allocate Texture 1 (Brick) ///////
  SDL_Surface* surface1 = SDL_LoadBMP("textures/brick.bmp");
  if (surface == NULL) {  // If it failed, print the error
    printf("Error: \"%s\"\n", SDL_GetError());
    return 1;
  }
  GLuint tex1;
  glGenTextures(1, &tex1);

  // Load the texture into memory
  glActiveTexture(GL_TEXTURE1);

  glBindTexture(GL_TEXTURE_2D, tex1);
  // What to do outside 0-1 range
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // How to filter
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w, surface1->h, 0, GL_BGR,
               GL_UNSIGNED_BYTE, surface1->pixels);

  glGenerateMipmap(GL_TEXTURE_2D);

  SDL_FreeSurface(surface1);
  //// End Allocate Texture ///////

  // Build a Vertex Array Object. This stores the VBO and attribute mappings in
  // one object
  GLuint vao;
  glGenVertexArrays(1, &vao);  // Create a VAO
  glBindVertexArray(vao);  // Bind the above created VAO to the current context

  // Allocate memory on the graphics card to store geometry (vertex buffer
  // object)
  GLuint vbo[1];
  glGenBuffers(1, vbo);                   // Create 1 buffer called vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);  // Set the vbo as the active array
                                          // buffer (Only one buffer can be
                                          // active at a time)
  glBufferData(GL_ARRAY_BUFFER, totalNumVerts * 8 * sizeof(float), modelData,
               GL_STATIC_DRAW);  // upload vertices to vbo
  // GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW =
  // geometry changes infrequently  GL_STREAM_DRAW = geom. changes frequently.
  // This effects which types of GPU memory is used

  int texturedShader =
      InitShader("src/shaders/vertexTex.glsl", "src/shaders/fragmentTex.glsl");

  int phongShader =
      InitShader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

  // Tell OpenGL how to set fragment shader input
  GLint posAttrib = glGetAttribLocation(texturedShader, "position");
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  // Attribute, vals/attrib., type, normalized?, stride, offset
  // Binds to VBO current GL_ARRAY_BUFFER
  glEnableVertexAttribArray(posAttrib);

  // GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
  // glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
  // (void*)(3*sizeof(float)));  glEnableVertexAttribArray(colAttrib);

  GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
  glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(5 * sizeof(float)));
  glEnableVertexAttribArray(normAttrib);

  GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(3 * sizeof(float)));

  GLint uniView = glGetUniformLocation(texturedShader, "view");
  GLint uniProj = glGetUniformLocation(texturedShader, "proj");

  glBindVertexArray(0);  // Unbind the VAO in case we want to create a new one

  glEnable(GL_DEPTH_TEST);

  camPos = glm::vec3(m->start_x * m->scale, m->start_y * m->scale, 0.f);
  viewPoint = glm::vec3(m->goal_x * m->scale, m->goal_y * m->scale, 0.f);
  glm::vec3 up(0.0f, 0.0f, 1.0f);

  lookAngle = std::atan((viewPoint.y - camPos.y) / (viewPoint.x - camPos.x));
  float angleInc = 3.14f / 16.;
  float moveInc = 0.1;

  // printf("camera: %f %f %f\nview pt: %f %f %f\nlookAngle: %f\n",
  // camPos.x,
  //        camPos.y, camPos.z, viewPoint.x, viewPoint.y, viewPoint.z,
  //        lookAngle);

  // Event Loop (Loop forever processing each event as fast as possible)
  SDL_Event windowEvent;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&windowEvent)) {  // inspect all events in the queue
      if (windowEvent.type == SDL_QUIT) quit = true;
      // List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch
      // many special keys  Scancode referes to a keyboard position, keycode
      // referes to the letter (e.g., EU keyboards)
      if (windowEvent.type == SDL_KEYUP &&
          windowEvent.key.keysym.sym == SDLK_ESCAPE)
        SDL_SetRelativeMouseMode(SDL_FALSE);
      if (windowEvent.type == SDL_KEYUP &&
          windowEvent.key.keysym.sym == SDLK_f) {
        fullscreen = !fullscreen;
        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
      }

      // WASD
      float newX = camPos.x;
      float newY = camPos.y;
      if (windowEvent.type == SDL_KEYDOWN &&
          (windowEvent.key.keysym.sym == SDLK_UP ||
           windowEvent.key.keysym.sym == SDLK_w)) {
        newX += moveInc * std::sin(lookAngle);
        newY += moveInc * std::cos(lookAngle);
        if (!m->WillCollide(newX, newY)) {
          camPos.x = newX;
          camPos.y = newY;
          viewPoint.x += moveInc * std::sin(lookAngle);
          viewPoint.y += moveInc * std::cos(lookAngle);
        }
      }
      if (windowEvent.type == SDL_KEYDOWN &&
          (windowEvent.key.keysym.sym == SDLK_DOWN ||
           windowEvent.key.keysym.sym == SDLK_s)) {
        newX -= moveInc * std::sin(lookAngle);
        newY -= moveInc * std::cos(lookAngle);
        if (!m->WillCollide(newX, newY)) {
          camPos.x = newX;
          camPos.y = newY;
          viewPoint.x -= moveInc * std::sin(lookAngle);
          viewPoint.y -= moveInc * std::cos(lookAngle);
        }
      }
      if (windowEvent.type == SDL_KEYDOWN &&
          windowEvent.key.keysym.sym == SDLK_d) {
        newX += moveInc * std::cos(lookAngle);
        newY -= moveInc * std::sin(lookAngle);
        if (!m->WillCollide(newX, newY)) {
          camPos.x = newX;
          camPos.y = newY;
          viewPoint.x += moveInc * std::cos(lookAngle);
          viewPoint.y -= moveInc * std::sin(lookAngle);
        }
      }
      if (windowEvent.type == SDL_KEYDOWN &&
          windowEvent.key.keysym.sym == SDLK_a) {
        newX -= moveInc * std::cos(lookAngle);
        newY += moveInc * std::sin(lookAngle);
        if (!m->WillCollide(newX, newY)) {
          camPos.x = newX;
          camPos.y = newY;
          viewPoint.x -= moveInc * std::cos(lookAngle);
          viewPoint.y += moveInc * std::sin(lookAngle);
        }
      }

      if (!SDL_GetRelativeMouseMode()) {  // Turn with left/right arrow keys
        if (windowEvent.type == SDL_KEYDOWN &&
            windowEvent.key.keysym.sym ==
                SDLK_LEFT) {  // If "down key" is pressed
          lookAngle -= angleInc;
          viewPoint.x = camPos.x + moveInc * std::sin(lookAngle);
          viewPoint.y = camPos.y + moveInc * std::cos(lookAngle);
        }
        if (windowEvent.type == SDL_KEYDOWN &&
            windowEvent.key.keysym.sym ==
                SDLK_RIGHT) {  // If "down key" is pressed
          lookAngle += angleInc;
          viewPoint.x = camPos.x + moveInc * std::sin(lookAngle);
          viewPoint.y = camPos.y + moveInc * std::cos(lookAngle);
        }
      } else {
        int mx_, my_;
        SDL_GetRelativeMouseState(&mx_, &my_);
        float mx = std::clamp(mx_, -50, 50);
        float mouseScale = 500.;

        // printf("Mouse at %d %d\n", mx, my);
        lookAngle += mx / mouseScale;
        viewPoint.x = camPos.x + moveInc * std::sin(lookAngle);
        viewPoint.y = camPos.y + moveInc * std::cos(lookAngle);
      }
    }

    // Clear the screen to default color
    glClearColor(.2f, 0.4f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(texturedShader);

    glm::mat4 view = glm::lookAt(camPos, viewPoint, up);
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 proj = glm::perspective(
        frustAngle, screenWidth / (float)screenHeight, frustNear,
        frustFar);  // FOV, aspect, near, far
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

    glBindVertexArray(vao);

    drawGeometry(texturedShader, numVertsFloor, numVertsWall, numVertsKey);

    SDL_GL_SwapWindow(window);  // Double buffering

    if (m->gameOver) {
      if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over",
                                   "You win!", window)) {
        exit(1);
      }
      quit = true;
    }
  }

  // Clean Up
  delete m;
  glDeleteProgram(phongShader);
  glDeleteBuffers(1, vbo);
  glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}

void drawGeometry(int shaderProgram, int numVertsFloor, int numVertsWall,
                  int numVertsKey) {
  GLint uniModel = glGetUniformLocation(shaderProgram, "model");
  GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
  GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");

  timePast = SDL_GetTicks() / 1000.f;

  // Draw floor
  glm::mat4 model;
  model = glm::translate(
      model, glm::vec3((m->width / 2. - 0.5) * m->scale,
                       (m->height / 2. - 0.5) * m->scale, -0.5 * m->scale));
  model = glm::scale(
      model, glm::vec3(m->width / 2. * m->scale, m->height / 2. * m->scale, 1));
  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

  glUniform1i(uniTexID, 0);  // Set texture ID to use (-1 = no texture)
  glDrawArrays(GL_TRIANGLES, 0, numVertsFloor);

  // Draw walls
  glUniform1i(uniTexID, 1);  // Set texture ID to use
  for (auto w : m->walls) {
    model = glm::mat4();
    model =
        glm::translate(model, glm::vec3(w->x * m->scale, w->y * m->scale, 0));
    model = glm::scale(model, glm::vec3(m->scale, m->scale, m->scale));
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVertsFloor,
                 numVertsWall);  //(Primitive Type, Start Vertex, End Vertex)
  }

  // Draw Doors and Keys
  glm::vec3 colVec;
  glUniform1i(uniTexID, -1);  // Set texture ID to use
  for (auto k : m->keys) {
    if (k) {
      // Each door/key pair shares the same color
      colVec = k->color;
      glUniform3fv(uniColor, 1, glm::value_ptr(colVec));

      // Door
      if (!k->door->open) {
        // Don't draw open doors
        model = glm::mat4();
        model = glm::translate(
            model, glm::vec3(k->door->x * m->scale, k->door->y * m->scale, 0));
        model = glm::scale(model, glm::vec3(m->scale, m->scale, m->scale));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, numVertsFloor, numVertsWall);
      } else {
        // Animate open door
        model = glm::mat4();
        model = glm::translate(
            model, glm::vec3(k->door->x * m->scale, k->door->y * m->scale,
                             timePast / 50.));
        model = glm::rotate(model, timePast * 3.14f / 2,
                            glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(m->scale, m->scale, m->scale));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, numVertsFloor, numVertsWall);
      }

      // Key
      if (k == m->heldKey) {
        // Render held key in front of player
        model = glm::mat4();
        float d = 0.5;
        model = glm::translate(
            model, glm::vec3(camPos.x + d * std::sin(lookAngle),
                             camPos.y + d * std::cos(lookAngle), 0));
        model = glm::scale(
            model, glm::vec3(m->scale / 8., m->scale / 8., m->scale / 8.));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, numVertsFloor + numVertsWall, numVertsKey);
      } else if (!k->door->open) {
        // Render other keys at their positions in the map
        model = glm::mat4();
        model = glm::translate(model,
                               glm::vec3(k->x * m->scale, k->y * m->scale, 0));
        model = glm::scale(
            model, glm::vec3(m->scale / 2., m->scale / 2., m->scale / 2.));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, numVertsFloor + numVertsWall, numVertsKey);
      }
    }
  }

  // Goal marker
  colVec = glm::vec3(1, 1, 1);
  glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
  model = glm::mat4();
  model = glm::translate(
      model, glm::vec3(m->goal_x * m->scale, m->goal_y * m->scale, 0));
  model =
      glm::scale(model, glm::vec3(m->scale / 4., m->scale / 4., m->scale / 4.));
  model = glm::rotate(model, timePast * 3.14f / 2, glm::vec3(0.0f, 1.0f, 1.0f));
  model = glm::rotate(model, timePast * 3.14f / 4, glm::vec3(1.0f, 0.0f, 0.0f));
  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
  glDrawArrays(GL_TRIANGLES, numVertsFloor, numVertsWall);
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile) {
  FILE* fp;
  long length;
  char* buffer;

  // open the file containing the text of the shader code
  fp = fopen(shaderFile, "r");

  // check for errors in opening the file
  if (fp == NULL) {
    printf("can't open shader source file %s\n", shaderFile);
    return NULL;
  }

  // determine the file size
  fseek(fp, 0, SEEK_END);  // move position indicator to the end of the file;
  length = ftell(fp);      // return the value of the current position

  // allocate a buffer with the indicated number of bytes, plus one
  buffer = new char[length + 1];

  // read the appropriate number of bytes from the file
  fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
  fread(buffer, 1, length, fp);  // read all of the bytes

  // append a NULL character to indicate the end of the string
  buffer[length] = '\0';

  // close the file
  fclose(fp);

  // return the string
  return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName) {
  GLuint vertex_shader, fragment_shader;
  GLchar *vs_text, *fs_text;
  GLuint program;

  // check GLSL version
  // printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Create shader handlers
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  // Read source code from shader files
  vs_text = readShaderSource(vShaderFileName);
  fs_text = readShaderSource(fShaderFileName);

  // error check
  if (vs_text == NULL) {
    printf("Failed to read from vertex shader file %s\n", vShaderFileName);
    exit(1);
  } else if (DEBUG_ON) {
    printf("Vertex Shader:\n=====================\n");
    printf("%s\n", vs_text);
    printf("=====================\n\n");
  }
  if (fs_text == NULL) {
    printf("Failed to read from fragent shader file %s\n", fShaderFileName);
    exit(1);
  } else if (DEBUG_ON) {
    printf("\nFragment Shader:\n=====================\n");
    printf("%s\n", fs_text);
    printf("=====================\n\n");
  }

  // Load Vertex Shader
  const char* vv = vs_text;
  glShaderSource(vertex_shader, 1, &vv, NULL);  // Read source
  glCompileShader(vertex_shader);               // Compile shaders

  // Check for errors
  GLint compiled;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    printf("Vertex shader failed to compile:\n");
    if (DEBUG_ON) {
      GLint logMaxSize, logLength;
      glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
      printf("printing error message of %d bytes\n", logMaxSize);
      char* logMsg = new char[logMaxSize];
      glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
      printf("%d bytes retrieved\n", logLength);
      printf("error message: %s\n", logMsg);
      delete[] logMsg;
    }
    exit(1);
  }

  // Load Fragment Shader
  const char* ff = fs_text;
  glShaderSource(fragment_shader, 1, &ff, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

  // Check for Errors
  if (!compiled) {
    printf("Fragment shader failed to compile\n");
    if (DEBUG_ON) {
      GLint logMaxSize, logLength;
      glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
      printf("printing error message of %d bytes\n", logMaxSize);
      char* logMsg = new char[logMaxSize];
      glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
      printf("%d bytes retrieved\n", logLength);
      printf("error message: %s\n", logMsg);
      delete[] logMsg;
    }
    exit(1);
  }

  // Create the program
  program = glCreateProgram();

  // Attach shaders to program
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  // Link and set program to use
  glLinkProgram(program);

  return program;
}
