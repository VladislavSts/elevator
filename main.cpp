#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>
#include <algorithm>

class elevator
{
private:
    enum direction_e { UP, DOWN, STOPPED };

    int current_floor;                      // текущий этаж
    direction_e direction;                  // направление движения 
    std::vector<int> floors_on_the_move;    // хранение этажей, на которые направляется лифт по ходу движения
    std::vector<int> other_floors;          // хранение этажей, на которые направляется лифт против хода движения (по завершению текущего направления)

public:
    explicit elevator() : current_floor(1), direction(STOPPED) {}

    // запрос этажа
    void request_floor(int floor)
    {
        if (floor == current_floor) {
            std::cout << "Lift is already on floor " << current_floor << ". Doors opened" << std::endl;
            return;
        }

        if (direction == STOPPED) {
            direction = (floor > current_floor) ? UP : DOWN;
        }

        if ((direction == UP && floor >= current_floor) || (direction == DOWN && floor <= current_floor)) {
            insert_into_destination_floors(floor); // по направлению движения
        }
        else {
            insert_into_other_floors(floor); // направление движения обратное
        }             
    }

    // добавление этажа в вектор floors_on_the_move
    void insert_into_destination_floors(int floor)
    {
        if (std::find(floors_on_the_move.begin(), floors_on_the_move.end(), floor) == floors_on_the_move.end()) {
            floors_on_the_move.push_back(floor); // добавляем этаж, если его там еще нет
        }
    }

    // добавление этажа в вектор other_floors
    void insert_into_other_floors(int floor) {
        if (std::find(other_floors.begin(), other_floors.end(), floor) == other_floors.end()) {
            other_floors.push_back(floor); // добавляем этаж, если его там еще нет
        }
    }

    // сортировка этажей для поэтапного движения по ним в зависимости от направления движения
    void sort_destination_floors()
    {
        if (direction == UP) {
            std::sort(floors_on_the_move.begin(), floors_on_the_move.end()); // сортировка в порядке возрастания
            std::sort(other_floors.begin(), other_floors.end());
        }
        else if (direction == DOWN) {
            std::sort(floors_on_the_move.begin(), floors_on_the_move.end(), std::greater<>()); // сортировка в порядке убывания
            std::sort(other_floors.begin(), other_floors.end(), std::greater<>());
        }
    }

    // описывает движение лифта
    void move()
    {
        int floor_destination = 0; // этаж, на который нужно попасть

        if (!floors_on_the_move.empty())
        {
            sort_destination_floors(); // сортируем этажи в соответствии с направлением движения

            floor_destination = floors_on_the_move.front(); // получаем нужный этаж (предварительно отсортированный по направлению движения)

            if (current_floor < floor_destination)
            {
                current_floor++;

                if (current_floor == floor_destination) {
                    direction = STOPPED;
                    floors_on_the_move.erase(floors_on_the_move.begin()); // приехали на нужный этаж, удаляем его из списка
                    status();
                }
                else
                    direction = UP;
            }
            else if (current_floor > floor_destination)
            {
                current_floor--;

                if (current_floor == floor_destination) {
                    direction = STOPPED;
                    floors_on_the_move.erase(floors_on_the_move.begin());
                    status();
                }
                else
                    direction = DOWN;
            }
        }
        else if (!other_floors.empty())
        {
            sort_destination_floors();

            floor_destination = other_floors.front();

            if (floor_destination > current_floor)
            {
                current_floor++;

                if (current_floor == floor_destination) {
                    direction = STOPPED;
                    other_floors.erase(other_floors.begin());
                    status();
                }
                else
                    direction = UP;
            }
            else if (floor_destination < current_floor)
            {
                current_floor--;

                if (current_floor == floor_destination) {
                    direction = STOPPED;
                    other_floors.erase(other_floors.begin());
                    status();
                }
                else
                    direction = DOWN;
            }
        }

        if (direction != STOPPED)
            status();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // отображение текущего направления и текущего этажа (или остановки на этаже)
    void status()
    {
        switch (direction)
        {
        case UP:
            std::cout << "Cabin on floor up " << current_floor << ". Going up." << std::endl;
            break;
        case DOWN:
            std::cout << "Cabin on floor down " << current_floor << ". Going down." << std::endl;
            break;
        case STOPPED:
            std::cout << "Cabin on floor " << current_floor << ". Stopped. Doors opened." << std::endl;
            break;
        }
    }
};


int main()
{
    int max_floors;
    std::cout << "Enter the number of floors in the building: ";
    std::cin >> max_floors;

    // обработка некорректного ввода количества этажей
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid entered! Try again! >> ";
        std::cin >> max_floors;
    }

    elevator elevator;

    std::string command;
    int floor;

    while (true)
    {
        elevator.move();

        if (_kbhit())
        {
            std::cout << ">>";
            std::cin >> command;

            if (command[0] == 'F') 
            {
                floor = std::stoi(command.substr(1));

                if (floor > max_floors || floor <= 0)
                    std::cout << "Invalid floor!"<<std::endl;
                else
                    elevator.request_floor(floor);
            }
            else if (command[0] == 'C') 
            {
                floor = std::stoi(command.substr(1));

                if (floor > max_floors || floor <= 0)
                    std::cout << "Invalid floor!" << std::endl;
                else
                    elevator.request_floor(floor);
            }
            else {
                std::cout << "Invalid command. Please enter a valid command." << std::endl;
            }
        }
    }

    return 0;
}
