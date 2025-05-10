#include <SFML/Graphics.hpp>
#include "Board.h"

const int TILE_SIZE = 60;                           // Размер клетки

class Graphics {
public:
    bool gameOver = false;                          // Флаг окончания игры
    Graphics(Board& board); 
    void run();                                     // Основной метод для запуска графики
    void showMessage(string message);               // Метод для отображения сообщения о победе
    void resetGameState();
    int selectedX = -1, selectedY = -1;             // Выбранная шашка
    int moveAttempts = 0;                           // Счетчик попыток выбора одной и той же шашки
    bool showError = false;                         // Флаг для отображения ошибки
    sf::Clock errorClock;                           // Таймер для скрытия ошибки

private:
    void drawBoard(int selectedX, int selectedY);   // Отрисовка доски
    void updateInfoText();                          // Обновление текстовой информации

    Board& board;                                   // Ссылка на объект доски
    sf::RenderWindow window;                        // Окно для отрисовки
    sf::Texture darkTexture;                        // Текстура для темных клеток
    sf::Texture lightTexture;                       // Текстура для светлых клеток
    sf::Font font;                                  // Шрифт для текста
    sf::Text infoText;                              // Текст с информацией о текущем состоянии игры
    sf::Text errorText;                             // Текст для отображения ошибок
};
