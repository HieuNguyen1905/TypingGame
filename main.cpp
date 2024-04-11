#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include<SDL_mixer.h>
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
double speed = SPEED;
int score = 0;
int scoreNeedToIncrease = 10;
double speedIncrease = SPEEDINCREASE;
int hp = HP;

class Word {
public:
    double x, y;
    string text;
    Word(double x, double y, string text) {
        this->x = x;
        this->y = y;
        this->text = text;
    }
};

vector<Word> wordsList;

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

//void renderMainMenu() {
//    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//    SDL_RenderClear(renderer);
//
//    write("Main Menu", WIDTH / 2 - 100, 50, White);
//    write("1. Start Game", WIDTH / 2 - 100, 150, White);
//    write("2. Instructions", WIDTH / 2 - 100, 200, White);
//    write("3. Exit", WIDTH / 2 - 100, 250, White);
//
//    SDL_RenderPresent(renderer); 
//}

void renderMainMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);

    // Load hình ảnh cho menu
    SDL_Texture* mainMenuTexture = IMG_LoadTexture(renderer, MAINMENU_PATH);
    if (!mainMenuTexture) {
        cout << "Failed to load main menu texture: " << IMG_GetError() << endl;
    }
    else {
        // Vẽ hình ảnh lên renderer
        SDL_RenderCopy(renderer, mainMenuTexture, NULL, NULL);
        // Dọn dẹp texture sau khi sử dụng
        SDL_DestroyTexture(mainMenuTexture);
    }

    SDL_RenderPresent(renderer);
}

//void renderMainMenu() {
//    const int BUTTON_WIDTH = 200; // Ð?nh nghia chi?u r?ng c?a các nút
//    const int BUTTON_HEIGHT = 50; // Ð?nh nghia chi?u cao c?a các nút
//    // Clear the renderer
//    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//    SDL_RenderClear(renderer);
//
//    // Render background image
//    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
//    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
//    // Load button images
//    SDL_Texture* startButtonTexture = IMG_LoadTexture(renderer, "startbutton.jpg");
//    SDL_Texture* instructionsButtonTexture = IMG_LoadTexture(renderer, "howtoplay.jpg");
//    SDL_Texture* exitButtonTexture = IMG_LoadTexture(renderer, "highscore.jpg");
//
//
//    // Render buttons
//    SDL_Rect startButtonRect = { (WIDTH - BUTTON_WIDTH) / 2, 150, BUTTON_WIDTH, BUTTON_HEIGHT };
//    SDL_Rect instructionsButtonRect = { (WIDTH - BUTTON_WIDTH) / 2, 200, BUTTON_WIDTH, BUTTON_HEIGHT };
//    SDL_Rect exitButtonRect = { (WIDTH - BUTTON_WIDTH) / 2, 250, BUTTON_WIDTH, BUTTON_HEIGHT };
//
//    SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect);
//    SDL_RenderCopy(renderer, instructionsButtonTexture, NULL, &instructionsButtonRect);
//    SDL_RenderCopy(renderer, exitButtonTexture, NULL, &exitButtonRect);
//
//    // Clean up button textures
//    SDL_DestroyTexture(startButtonTexture);
//    SDL_DestroyTexture(instructionsButtonTexture);
//    SDL_DestroyTexture(exitButtonTexture);
//
//    // Present the renderer
//    SDL_RenderPresent(renderer);
//}

void RenderGameOverMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    write("Replay", WIDTH / 2 - 100, 150, White);

    write(" Exit", WIDTH / 2 - 100, 250, White);

    SDL_RenderPresent(renderer); 
}

bool startGameRequested = false;


void handleMainMenuInput() {
    TTF_Font* font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    SDL_Event e;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    const int buttonHeight = 80;
    const int buttonWidth = 320;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            int mouseX, mouseY;

            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseX >= 470 && mouseX <= 470 + buttonWidth &&
                mouseY >= 313 && mouseY <= 313 + buttonHeight) {

                startGameRequested = true;
            }
            break;
        default: break;
        }
    }
}

bool gameOver = false;

void update() {
    if (hp <= 0) {
       // save(wpm, (float)tick2 / 1000.0);
        gameOver = true;
        running = false;
 //       return;
 //      cout << "You lost!!!" << endl << "Your score: " << score << endl;
    }

 //   cout << speed << endl;
    while (wordsList.size() < 15) {
        Word temp(randomNumberX(0, 150), randomNumberY(0, HEIGHT - 100), getLine());
        wordsList.push_back(temp);
    }

    vector<Word> tempList;
    for (auto &word : wordsList) {
        if (word.x > WIDTH) {
            hp--;
            continue;
        }
        word.x += speed;
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
            inputStr = "";
            continue;
        }
        tempList.push_back(word);
    }
    if (!soundEffect2) {
        cout << "Failed to load typing sound effect2!" << endl;
    }
    Mix_PlayChannel(-1, soundEffect2, 0);
    if (score == scoreNeedToIncrease) {
        speed += speedIncrease;
        scoreNeedToIncrease += 10;
    }
 //   cout << speed << endl;
    wordsList = tempList;

    inputStr = "";
}

void input() {
  //  if (gameOver) return;
    SDL_Event e;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    SDL_StartTextInput();
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_KEYDOWN:
            if (!soundEffect1) {
                cout << "Failed to load typing sound effect1!" << endl;
            }
            Mix_PlayChannel(-1, soundEffect1, 0);
 
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


void render() {

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
    SDL_RenderFillRect(renderer, &statusBar);

    for (auto &word : wordsList) {
        write(word.text, word.x, word.y, White, true);
    }

    write("[" + inputStr + "]" + " | Score: " + to_string(score) + " | HP: " + to_string(hp), 30, HEIGHT - BARHEIGHT + 10, Black);
    // + " | wpm: " + to_string(wpm)
    SDL_RenderPresent(renderer);
}



void main_loop() {
    Uint64 start = SDL_GetPerformanceCounter();
    tick2 = SDL_GetTicks();
 //   cout << tick2 << endl;
    delta = tick2 - tick1;
 //   cout << delta << endl;
    if (delta > 1000.0 / (float)FPS) {
        tick1 = SDL_GetTicks();
        //       cout << tick1 << endl;
        if (startGameRequested) {
            update();
            input();
            render();
        }
        else if (gameOver) {
            RenderGameOverMenu();
        }
        else {
                renderMainMenu();
                handleMainMenuInput();
            }
        }
}

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        cout << "Failed at SDL_Init()" << endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0)
        cout << "Failed at SDL_CreateWindowAndRenderer()" << endl;
    if (TTF_Init() < 0)
        cout << "Failed at TTF_Init" << TTF_GetError() << endl;

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        cout << "Failed at IMG_Init" << IMG_GetError() << endl;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
    }

    running = true;

    SDL_SetWindowTitle(window, "Typing Game");
    SDL_Surface* image = IMG_Load(BACKGROUND_PATH);
    imgTexture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    if (!image) {
        cout << "Failed at IMG_Load" << IMG_GetError() << endl;
    }
    soundEffect1 = Mix_LoadWAV("resource/typing.wav");
    soundEffect2 = Mix_LoadWAV("resource/trueword.wav");

    while (running) 
        main_loop();

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


