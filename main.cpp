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
#include<set>
#include "main.h"
#include "words.h"
#include "highscore.h"

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
double speed = SPEED;
int score = 0;
int HIGHSCORE = 0;
int scoreNeedToIncrease = 10;
double speedIncrease = SPEEDINCREASE;
int hp = HP;
bool startGameRequested = false;
bool gameOver = false;
bool howToPlayRequested = false;
bool highScoreRequested = false;
bool pauseRequested = false;
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

    TTF_Font* font = TTF_OpenFont(FONT_PATH1, FONT_SIZE1);
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

void writeHighScore(string str, int x, int y, SDL_Color color) {
    SDL_Surface* surface;
    SDL_Texture* texture;

    TTF_Font* font = TTF_OpenFont(FONT_PATH2, FONT_SIZE2);
    if (!font)
        cout << "Couldn't find/init open ttf font >> " << TTF_GetError() << endl;
    int prefix_width = 0,
        prefix_height = 0;
        surface = TTF_RenderText_Solid(font, str.c_str(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        prefix_width = surface->w;
        prefix_height = surface->h;
        SDL_Rect message_rect = { x, y, prefix_width, prefix_height };
        SDL_RenderCopy(renderer, texture, NULL, &message_rect);

    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderHighScoreMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);

    SDL_Texture* HighScoreMenu = IMG_LoadTexture(renderer, HIGHSCORE_PATH);
    if (!HighScoreMenu) {
        cout << "Failed to load game over menu texture: " << IMG_GetError() << endl;
    }
    else {

        SDL_RenderCopy(renderer, HighScoreMenu, NULL, NULL);

        SDL_DestroyTexture(HighScoreMenu);
    }
    readHighScore(HIGHSCORE);
    writeHighScore("Highest Score: " + to_string(HIGHSCORE), 400, 350, White);
    SDL_RenderPresent(renderer);
}

void handleHighScoreMenu() {
    SDL_Event e;
    const int buttonHeight = 80;
    const int buttonWidth = 155;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!soundEffect3) {
                cout << "Failed to load typing sound effect1!" << endl;
            }
            Mix_PlayChannel(-1, soundEffect3, 0);
            int mouseX, mouseY;

            SDL_GetMouseState(&mouseX, &mouseY);

            if (mouseX >= 18 && mouseX <= 18 + buttonWidth &&
                mouseY >= 608 && mouseY <= 608 + buttonHeight) {
                highScoreRequested = false;
            }

            break;
        default: break;
        }
    }
}

void renderHowToPlayMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);

    SDL_Texture* HowToPlay = IMG_LoadTexture(renderer, HOWTOPLAY_PATH);
    if (!HowToPlay) {
        cout << "Failed to load game over menu texture: " << IMG_GetError() << endl;
    }
    else {

        SDL_RenderCopy(renderer, HowToPlay, NULL, NULL);

        SDL_DestroyTexture(HowToPlay);
    }

    SDL_RenderPresent(renderer);
}

void handleHowToPlayMenu() {
    SDL_Event e;
    const int buttonHeight = 80;
    const int buttonWidth = 155;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!soundEffect3) {
                cout << "Failed to load typing sound effect1!" << endl;
            }
            Mix_PlayChannel(-1, soundEffect3, 0);
            int mouseX, mouseY;

            SDL_GetMouseState(&mouseX, &mouseY);
          
            if (mouseX >= 18 && mouseX <= 18 + buttonWidth &&
                mouseY >= 608 && mouseY <= 608 + buttonHeight) {
                howToPlayRequested = false;
            }

            break;
        default: break;
        }
    }
}

void renderPauseMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);

    SDL_Texture* PauseMenu = IMG_LoadTexture(renderer, PAUSEMENU_PATH);
    if (!PauseMenu) {
        cout << "Failed to load game over menu texture: " << IMG_GetError() << endl;
    }
    else {

        SDL_RenderCopy(renderer, PauseMenu, NULL, NULL);

        SDL_DestroyTexture(PauseMenu);
    }

    SDL_RenderPresent(renderer);
}

void handlePauseMenu() {
    SDL_Event e;
    const int buttonHeight = 97;
    const int buttonWidth = 396;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!soundEffect3) {
                cout << "Failed to load typing sound effect1!" << endl;
            }
            Mix_PlayChannel(-1, soundEffect3, 0);
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseX >= 437 && mouseX <= 437 + buttonWidth &&
                mouseY >= 244 && mouseY <= 244 + buttonHeight) {
                startGameRequested = true;
                pauseRequested = false;
            }
            if (mouseX >= 437 && mouseX <= 437 + buttonWidth &&
                mouseY >= 394 && mouseY <= 394 + buttonHeight) {
                running = false;
            }
            break;
        default: break;
        }
    }
}

void RenderGameOverMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);

    SDL_Texture* GameOverMenu = IMG_LoadTexture(renderer, GAMEOVERMENU_PATH);
    if (!GameOverMenu) {
        cout << "Failed to load game over menu texture: " << IMG_GetError() << endl;
    }
    else {

        SDL_RenderCopy(renderer, GameOverMenu, NULL, NULL);

        SDL_DestroyTexture(GameOverMenu);
    }
    writeHighScore("Current Score: " + to_string(score), 400, 100, White);
    writeHighScore("Highest Score: " + to_string(HIGHSCORE), 400, 150, White);

    SDL_RenderPresent(renderer);

}

void handleGameOverMenu() {
    SDL_Event e;
    const int buttonHeight = 80;
    const int buttonWidth = 320;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        case SDL_QUIT:    
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!soundEffect3) {
                cout << "Failed to load typing sound effect1!" << endl;
            }
            Mix_PlayChannel(-1, soundEffect3, 0);
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseX >= 456 && mouseX <= 456 + buttonWidth &&
                mouseY >= 457 && mouseY <= 457 + buttonHeight) {

                gameOver = false;
                startGameRequested = true;
                score = 0;
                hp = HP;
                speed = SPEED;
                scoreNeedToIncrease = 10;
                inputStr = "";
                wordsList.clear();
            }
            if (mouseX >= 460 && mouseX <= 460 + buttonWidth &&
                mouseY >= 576 && mouseY <= 576 + buttonHeight) {
                running = false;
            }
            break;
        default: break;
        }
    }
}

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

void handleMainMenuInput() {
    SDL_Event e;
    const int buttonHeight = 100;
    const int buttonWidth = 393;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!soundEffect3) {
                cout << "Failed to load typing sound effect1!" << endl;
            }
            Mix_PlayChannel(-1, soundEffect3, 0);
            int mouseX, mouseY;

            SDL_GetMouseState(&mouseX, &mouseY);
            // Start
            if (mouseX >= 437 && mouseX <= 437 + buttonWidth &&
                mouseY >= 246 && mouseY <= 246 + buttonHeight) {
                startGameRequested = true;
            }
            // How To Play
            if (mouseX >= 437 && mouseX <= 437 + buttonWidth &&
                mouseY >= 397 && mouseY <= 397 + buttonHeight) {
                howToPlayRequested = true;
            }
            // HighScore
            if (mouseX >= 437 && mouseX <= 437 + buttonWidth &&
                mouseY >= 565 && mouseY <= 565 + buttonHeight) {
                highScoreRequested = true;
            }
            break;
        default: break;
        }
    }
}

void update() {
    if (hp <= 0) {
        saveHighScore(score);
        readHighScore(HIGHSCORE);
        gameOver = true;
        startGameRequested = false;
        return;
    }

    // Sử dụng set để lưu trữ các vị trí đã chọn
    set<pair<int, int>> chosenPositions;

    while (wordsList.size() < 15) {
        double x = randomNumberX(0, 200);
        double y = randomNumberY(0, HEIGHT - 80);

        // Kiểm tra xem vị trí đã được chọn chưa
        if (chosenPositions.find({ (int)x, (int)y }) == chosenPositions.end()) {
            Word temp(x, y, getLine());
            wordsList.push_back(temp);

            // Thêm vị trí vào set
            chosenPositions.insert({ (int)x, (int)y });
        }
    }

    vector<Word> tempList;
    for (auto& word : wordsList) {
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
    wordsList = tempList;

    inputStr = "";
}

void input() {
        SDL_Event e;
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        SDL_StartTextInput();
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (mouseX >= 1233 && mouseX <= WIDTH &&
                    mouseY >= 0 && mouseY <= 44) {
                    pauseRequested = true;
                    startGameRequested = false;
                }

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

        for (auto& word : wordsList) {
            write(word.text, word.x, word.y, White, true);
        }

        write("[" + inputStr + "]" + " | Score: " + to_string(score) + " | HP: " + to_string(hp), 30, HEIGHT - BARHEIGHT + 10, Black);

        SDL_RenderPresent(renderer);

}

void main_loop() {
    Uint64 start = SDL_GetPerformanceCounter();
    tick2 = SDL_GetTicks();

    delta = tick2 - tick1;

    if (delta > 1000.0 / (float)FPS) {
        tick1 = SDL_GetTicks();

    
        if (startGameRequested == false && gameOver == false && howToPlayRequested == false && highScoreRequested == false && pauseRequested == false) {
            renderMainMenu();
            handleMainMenuInput();
        }
        else if (startGameRequested == false && gameOver == false && howToPlayRequested == false && highScoreRequested == false && pauseRequested == true) {
            renderPauseMenu();
            handlePauseMenu();
        }
        else if (startGameRequested == false && gameOver == false && howToPlayRequested == false && highScoreRequested == true && pauseRequested == false) {
            renderHighScoreMenu();
            handleHighScoreMenu();
        }
        else if (startGameRequested == false && gameOver == false && howToPlayRequested == true && highScoreRequested == false && pauseRequested == false) {
            renderHowToPlayMenu();
            handleHowToPlayMenu();
        }
        else if (startGameRequested == false && gameOver == true && howToPlayRequested == false && highScoreRequested == false && pauseRequested == false) {
            RenderGameOverMenu();
            handleGameOverMenu();
        }
        else if (startGameRequested == true && gameOver == false && howToPlayRequested == false && highScoreRequested == false && pauseRequested == false) {
            update();
            input();
            render();
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
    soundEffect3 = Mix_LoadWAV("resource/popup.wav");

    while (running) 
        main_loop();

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


