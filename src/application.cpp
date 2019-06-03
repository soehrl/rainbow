#include "application.hpp"

#include <chrono>
#include <iostream>

#include <glm/geometric.hpp>

namespace rainbow {

Application::Application() {
  if (SDL_CreateWindowAndRenderer(512, 512, 0, &window_, &renderer_) < 0) {
    std::cerr << "Failed to create window!" << std::endl;
    throw std::runtime_error("Failed to create SDL window");
  }
}

Application::~Application() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

bool Application::LoadScene(const std::string& filename) {
  if (scene_.Load(filename)) {
    RenderPreview();
    return true;
  } else {
    std::string error_string = "Failed to load scene: `" + filename + "`";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Loading failed",
                             error_string.c_str(), window_);
    std::cerr << error_string << std::endl;
    return false;
  }
}

void Application::Run() {
  SDL_Event event;
  while (!quit_application_) {
    if (interactive_mode_) {
      while (SDL_PollEvent(&event)) {
        ProcessEvent(event);
      }
      const auto current_time = std::chrono::steady_clock::now();
      Update(current_time - last_update_);
      last_update_ = current_time;

    } else {
      if (SDL_WaitEvent(&event)) {
        ProcessEvent(event);
      }
    }

    if (redraw_preview_) {
      RenderPreview();
      redraw_preview_ = false;
    }
  }
}

void Application::ProcessEvent(const SDL_Event& event) {
  switch (event.type) {
    case SDL_QUIT:
      quit_application_ = true;
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        EnterInteractiveMode();
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        LeaveInteractiveMode();
      }
      break;

    case SDL_MOUSEMOTION:
      if (interactive_mode_) {
        camera_.Rotate(event.motion.xrel * 0.01f, event.motion.yrel * 0.01f);
        redraw_preview_ = true;
      }
      break;
  }
}

void Application::RenderPreview() {
  int window_width;
  int window_height;
  SDL_GetWindowSize(window_, &window_width, &window_height);

  const auto view_rays =
      camera_.ComputeViewRays(glm::uvec2{window_width, window_height});
  -SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  const auto t0 = std::chrono::steady_clock::now();
  int x = 0;
  int y = window_height - 1;
  for (const auto& view_ray : view_rays) {
    const auto hitpoint = scene_.ShootRay(view_ray);
    if (hitpoint) {
      aiColor3D color;
      hitpoint->material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
      SDL_SetRenderDrawColor(renderer_, color.r * 255, color.g * 255,
                             color.b * 255, 255);
      SDL_RenderDrawPoint(renderer_, x, y);
    }
    ++x;
    if (x == window_width) {
      x = 0;
      --y;
    }
  }
  SDL_GL_SwapWindow(window_);

  const auto t1 = std::chrono::steady_clock::now();
  std::cout
      << "RenderPreview(): "
      << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
      << "ms" << std::endl;
}

void Application::EnterInteractiveMode() {
  interactive_mode_ = true;
  last_update_ = std::chrono::steady_clock::now();
}

void Application::Update(std::chrono::duration<float> elapsed_time) {
  glm::vec3 move_vector{0.0f, 0.0f, 0.0f};
  const Uint8* keys = SDL_GetKeyboardState(nullptr);

  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 forward;
  camera_.GetAxisVectors(&right, &up, &forward);

  if (keys[SDL_SCANCODE_W]) {
    move_vector += forward;
  }
  if (keys[SDL_SCANCODE_S]) {
    move_vector -= forward;
  }
  if (keys[SDL_SCANCODE_D]) {
    move_vector += right;
  }
  if (keys[SDL_SCANCODE_A]) {
    move_vector -= right;
  }
  if (keys[SDL_SCANCODE_E]) {
    move_vector += up;
  }
  if (keys[SDL_SCANCODE_Q]) {
    move_vector -= up;
  }

  float length = glm::length(move_vector);
  if (length > std::numeric_limits<float>::epsilon()) {
    std::cout << "Length: " << length << std::endl;
    camera_.Move(move_vector * (elapsed_time.count() / length));
    redraw_preview_ = true;
  }
}

void Application::LeaveInteractiveMode() { interactive_mode_ = false; }

}  // namespace rainbow