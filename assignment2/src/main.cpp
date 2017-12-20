#include "scene.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  Scene* scene;

  if (argc > 1) {
    std::ifstream input(argv[1]);
    if (input) {
      scene = new Scene(input);
    } else {
      fprintf(stderr, "Error opening %s\n", argv[1]);
      exit(1);
    }
  } else {
    scene = new Scene(std::cin);
  }

  scene->RayTrace();

  return 0;
}
