#include "Graphics.h"

int main() {
    setlocale(LC_ALL, "English");
    srand(static_cast<unsigned int>(time(0)));

    Board board;
    Graphics graphics(board);
    graphics.run();

    return 0;
}