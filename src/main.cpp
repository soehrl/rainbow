#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include "application.hpp"
#include "camera.hpp"
#include "scene.hpp"

int main(int argc, char* argv[]) {
  rainbow::Application app;

  if (argc >= 2) {
    if (!app.LoadScene(argv[1])) {
      return EXIT_FAILURE;
    }
  }

  app.Run();
}
