/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 *
 * @author Héctor Martín Álvarez
 * @date Aug 04 2025
 * @brief Basic Pong-style mini-game with a single paddle using SDL2.
 * @see https://wiki.libsdl.org/SDL2/
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

constexpr uint16_t kWindowWidth = 800;
constexpr uint16_t kWindowHeight = 600;
constexpr int kPaddleWidth = 100;
constexpr int kPaddleHeight = 20;
constexpr int kPaddleSpeed = 10;
constexpr int kBallSize = 20;
constexpr int kBallSpeed = 5;
constexpr int kFontSize = 24;

int main(int argc, char* argv[]) {

  // --help command
  if (argc > 1 && std::string(argv[1]) == "--help") {
    std::cout << "How to play " << std::string(argv[0] + 2) << std::endl;
    std::cout << "Commands:\n";
    std::cout << "  ← / →     Move the paddle\n";
    std::cout << "  M         Change between light/dark mode\n";
    std::cout << "  P         Pause/resume game\n";
    std::cout << "  ESC       Leave the game\n";
    return 0;
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Error initializing video: " << SDL_GetError() << std::endl;
    return 1;
  }

  if (TTF_Init() != 0) {
    std::cerr << "Error initializing TTF: " << TTF_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Pong (but with one paddle)",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        kWindowWidth, kWindowHeight, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  /*
  SDL_Surface* icon = SDL_LoadBMP("icon.bmp");
  if (!icon) {
    std::cerr << "Error loading icon: " << SDL_GetError() << std::endl;
  } else {
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
  }
  */

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  TTF_Font* font = TTF_OpenFont("OpenSans-Bold.ttf", kFontSize);
  if (!font) {
    std::cerr << "Error loading font: " << TTF_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Rect paddle = {
    kWindowWidth / 2 - kPaddleWidth / 2,
    kWindowHeight - kPaddleHeight - 10,
    kPaddleWidth,
    kPaddleHeight
  };
  SDL_Rect ball = {
    kWindowWidth / 2 - kBallSize / 2,
    kWindowHeight / 2 - kBallSize / 2,
    kBallSize,
    kBallSize
  };


  int ball_dx = kBallSpeed;
  int ball_dy = -kBallSpeed;

  bool running{true};
  bool dark_mode{true};
  bool paused{false};
  unsigned hits{0};
  SDL_Event event;

  // Main loop
  while(running) {
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
        dark_mode = !dark_mode;
      } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
        paused = !paused;
      }
    }

    if (!paused) {
      const Uint8* keystate = SDL_GetKeyboardState(nullptr);
      if (keystate[SDL_SCANCODE_LEFT]) {
        paddle.x -= kPaddleSpeed;
        if (paddle.x < 0) paddle.x = 0;
      } else if (keystate[SDL_SCANCODE_RIGHT]) {
        paddle.x += kPaddleSpeed;
        if (paddle.x + paddle.w > kWindowWidth) {
          paddle.x = kWindowWidth - paddle.w;
        }
      }

      // Ball movement
      ball.x += ball_dx;
      ball.y += ball_dy;

      // Wall ball bounce
      if (ball.x <= 0 || ball.x + ball.w >= kWindowWidth) {
        ball_dx = -ball_dx;
      }
      if (ball.y <= 0) {
        ball_dy = -ball_dy;
      }

      // Paddle & ball collision
      if (SDL_HasIntersection(&ball, &paddle)) {
        ++hits;
        ball_dy = -kBallSpeed;
        ball.y = paddle.y - ball.h;
      }

      if (ball.y > kWindowHeight) {
        std::cout << "You lost!\n";
        running = false;
      }
    }

    // Render
    if (dark_mode) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderFillRect(renderer, &paddle);
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderFillRect(renderer, &ball);
    } else {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderFillRect(renderer, &paddle);
      SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
      SDL_RenderFillRect(renderer, &ball);
    }

    SDL_Color text_color = dark_mode ? SDL_Color{255, 255, 255, 255} : SDL_Color{0, 0, 0, 255};
    std::string text_string = "Hit number: " + std::to_string(hits);
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text_string.c_str(), text_color);
    
    if (text_surface) {
      SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
      SDL_Rect text_rect = {10, 10, text_surface->w, text_surface->h};
      SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);
      SDL_DestroyTexture(text_texture);
    } else {
      std::cerr << "Error creating surface: " << SDL_GetError() << std::endl;
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }

    SDL_FreeSurface(text_surface);

    SDL_RenderPresent(renderer);
    SDL_Delay(16); // For 60 FPS
  }

  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
