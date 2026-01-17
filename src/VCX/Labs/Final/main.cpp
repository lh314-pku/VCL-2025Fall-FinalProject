#include <iostream>
#include "Assets/bundled.h"  // 解决 VCX 命名空间
#include "Labs/Final/App.h"  // 引入 App 类


int main(){
    using namespace VCX;
    return Engine::RunApp<Labs::Final::App>(Engine::AppContextOptions {
        .Title      = "VCX Final Project",
        .WindowSize = { 1024, 768 },
        .FontSize   = 16,

        .IconFileNames = Assets::DefaultIcons,
        .FontFileNames = Assets::DefaultFonts,
    });
}