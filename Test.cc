#include <SDL2/SDL.h>
//#include <SDL_image.h>
#include <iostream>

int main(){
    std::cout << "Hello";
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Quit();
    return 0;
}