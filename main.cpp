#ifdef __EMSCRIPTEN__
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <emscripten.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#endif
#include <iostream>
#include<string>
#include <array>
#include <charconv>
#include <vector>
#include <tuple>
#include <utility>
#include "main.h"
#include "words.h"
#include"save.h"

using namespace std;

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font;
SDL_Color color;
SDL_Color White = { 255, 255, 255 };
SDL_Color Red = { 255, 0, 0 };
SDL_Color Black = { 0, 0, 0 };
SDL_Texture* imgTexture;
unsigned int tick1 = SDL_GetTicks();
unsigned int tick2 = SDL_GetTicks();
bool running;
double delta;
SDL_Rect statusBar = { 0, HEIGHT - BARHEIGHT, WIDTH, BARHEIGHT };

string inputStr = "";
string gameState = "intro";
int speed = SPEED;
int score = 0;
int hp = HP;
float wpm;

class Word {
public:
    int x, y;
    string text;
    Word(int x, int y, string text) {
        this->x = x;
        this->y = y;
        this->text = text;
    }
};

vector<Word> wordsList;

void update() {
    if (hp <= 0) {
       // save(wpm, (float)tick2 / 1000.0);
#ifndef __EMSCRIPTEN__
        running = false;
        cout << "You lost, wpm: " + to_string(wpm) << endl;
#else
        emscripten_cancel_main_loop();
        running = false;
        cout << "You lost, wpm: " + to_string(wpm) << endl;
#endif
    }

    wpm = (float)score / (float)((float)tick2 / 60000.0);
    speed += ACC;

    while (wordsList.size() < 15) {
        Word temp(randomNumber(0, 150), randomNumber(0, HEIGHT - 100), getLine());
        wordsList.push_back(temp);
    }

    vector<Word> tempList;
    for (auto &word : wordsList) {
        if (word.x > WIDTH) {
            hp--;
            continue;
        }
        word.x += floor((float)SPEED / 1000.0);
        tempList.push_back(word);
    }
    wordsList = tempList;
}

void checkInput() {
    vector<Word> tempList;
    bool found = false;
    for (auto &word : wordsList) {
        if (word.text == inputStr && !found) {
            found = true;
            score++;
            continue;
        }
        tempList.push_back(word);
    }
    wordsList = tempList;

    inputStr = "";
}

void input() {
    SDL_Event e;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    SDL_StartTextInput();
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
            case SDLK_RETURN:
                checkInput();
                break;
            case SDLK_BACKSPACE:
                if (inputStr.length() > 0)
                    inputStr.pop_back();
                break;
            default:
                break;
            }
            break;
        case SDL_QUIT:
            running = false;
            break;
        case SDL_TEXTINPUT:
            if (*e.text.text == ' ') {
                checkInput();
            }
            else {
                inputStr += e.text.text;
                break;
            }
        default: break;
        }
    }
}

void write(string str, int x, int y, SDL_Color color, bool colorTyped = false) {
    SDL_Surface* surface;
    SDL_Texture* texture;

    TTF_Font* font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font)
        cout << "Couldn't find/init open ttf font >> " << TTF_GetError() << endl;
    int prefix_width = 0,
        prefix_height = 0;
    if (colorTyped && inputStr != "" && inputStr != str) {
        if (str.rfind(inputStr, 0) == 0) {
            str = str.substr(inputStr.length());
            surface = TTF_RenderText_Solid(font, inputStr.c_str(), Red);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            prefix_width = surface->w;
            prefix_height = surface->h;
            SDL_Rect message_rect = { x, y, prefix_width, prefix_height };
            SDL_RenderCopy(renderer, texture, NULL, &message_rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
    if (inputStr == str)
        color = Red;
    surface = TTF_RenderText_Solid(font, str.c_str(), color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    int text_width = surface->w;
    int text_height = surface->h;
    SDL_Rect message_rect = { x + prefix_width, y, text_width, text_height };
    SDL_RenderCopy(renderer, texture, NULL, &message_rect);

    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
    SDL_RenderFillRect(renderer, &statusBar);

    for (auto &word : wordsList) {
        write(word.text, word.x, word.y, White, true);
    }

    write("[" + inputStr + "]" + " | Score: " + to_string(score) + " | wpm: " + to_string(wpm) + " | HP: " + to_string(hp), 30, HEIGHT - BARHEIGHT + 10, Black);

    SDL_RenderPresent(renderer);
}

void main_loop() {
    Uint64 start = SDL_GetPerformanceCounter();
    tick2 = SDL_GetTicks();

    delta = tick2 - tick1;

    if (delta > 1000.0 / (float)FPS) {
        tick1 = SDL_GetTicks();
        update();
        input();
        render();
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        cout << "Failed at SDL_Init()" << endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0)
        cout << "Failed at SDL_CreateWindowAndRenderer()" << endl;
    if (TTF_Init() < 0)
        cout << "Failed at TTF_Init" << TTF_GetError() << endl;
    //#ifndef __EMSCRIPTEN__
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        cout << "Failed at IMG_Init" << IMG_GetError() << endl;
    //#endif

    running = true;

    SDL_SetWindowTitle(window, "WPM");
    SDL_Surface* image = IMG_Load(BG_PATH);
    imgTexture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    if (!image) {
        cout << "Failed at IMG_Load" << IMG_GetError() << endl;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (running)
        main_loop();
#endif
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
