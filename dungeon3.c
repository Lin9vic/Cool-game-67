#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define WIDTH 12
#define HEIGHT 9
#define LOCATIONS_COUNT 3

typedef struct {
    char name[30];
    char map[HEIGHT][WIDTH];
    int player_x;
    int player_y;
    bool chest_taken;
    bool talked_to_npc;
    bool door_open;
    int connections[3];
    int connections_count;
} Location;

Location locations[LOCATIONS_COUNT];
int current_location = 0;

// КАРТЫ ЛОКАЦИЙ (без @, только каркас)
char map0[HEIGHT][WIDTH] = {
    "############",
    "#........#.#",
    "#....C.....#",
    "#..........#",
    "#.....N....#",
    "#..........#",
    "#........D.#",
    "#..........#",
    "############"
};

char map1[HEIGHT][WIDTH] = {
    "############",
    "#......N...#",
    "#.....#....#",
    "#.......C..#",
    "#..........#",
    "#....D.....#",
    "#..........#",
    "#..........#",
    "############"
};

char map2[HEIGHT][WIDTH] = {
    "############",
    "#..........#",
    "#....C.....#",
    "#..........#",
    "#..........#",
    "#....D.....#",
    "#..........#",
    "#..........#",
    "############"
};

void init_locations() {
    strcpy(locations[0].name, "Входная комната");
    memcpy(locations[0].map, map0, sizeof(map0));
    locations[0].player_x = 6;
    locations[0].player_y = 2;
    locations[0].chest_taken = false;
    locations[0].talked_to_npc = false;
    locations[0].door_open = false;
    locations[0].connections_count = 2;
    locations[0].connections[0] = 1;
    locations[0].connections[1] = 2;

    strcpy(locations[1].name, "Библиотека");
    memcpy(locations[1].map, map1, sizeof(map1));
    locations[1].player_x = 5;
    locations[1].player_y = 4;
    locations[1].chest_taken = false;
    locations[1].talked_to_npc = false;
    locations[1].door_open = false;
    locations[1].connections_count = 2;
    locations[1].connections[0] = 0;
    locations[1].connections[1] = 2;

    strcpy(locations[2].name, "Сокровищница");
    memcpy(locations[2].map, map2, sizeof(map2));
    locations[2].player_x = 5;
    locations[2].player_y = 4;
    locations[2].chest_taken = false;
    locations[2].talked_to_npc = false;
    locations[2].door_open = false;
    locations[2].connections_count = 1;
    locations[2].connections[0] = 0;
}

void draw_map() {
    Location* loc = &locations[current_location];

    printf("\n========================================\n");
    printf("    %s\n", loc->name);
    printf("========================================\n\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == loc->player_x && j == loc->player_y) {
                printf("@");
            }
            else {
                printf("%c", loc->map[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

bool can_move(int new_x, int new_y) {
    Location* loc = &locations[current_location];

    if (new_x < 0 || new_x >= HEIGHT || new_y < 0 || new_y >= WIDTH) {
        return false;
    }
    if (loc->map[new_x][new_y] == '#') {
        return false;
    }
    return true;
}
void change_location(int target_loc) {
    if (target_loc < 0 || target_loc >= LOCATIONS_COUNT) {
        return;
    }

    current_location = target_loc;

    printf("\n ВЫ ПЕРЕШЛИ В: %s \n", locations[current_location].name);

    // Небольшая задержка для читаемости
    printf("   Нажмите Enter для продолжения...");
    getchar();
    getchar(); // очищаем буфер
}

void handle_cell(int x, int y) {
    Location* loc = &locations[current_location];
    char cell = loc->map[x][y];

    if (cell == 'C') {
        if (!loc->chest_taken) {
            loc->chest_taken = true;
            printf("\n ВЫ НАШЛИ СУНДУК!\n");
            if (current_location == 2) {
                printf("   Внутри: Золотой ключ! Теперь дверь откроется!\n");
            }
            else {
                printf("   Внутри: Немного монет!\n");
            }
            loc->map[x][y] = '.';
        }
        else {
            printf("\n Сундук пуст...\n");
        }
    }

    else if (cell == 'N') {
        if (!loc->talked_to_npc) {
            loc->talked_to_npc = true;
            printf("\n NPC: 'Приветствую, путник!'\n");
            if (current_location == 0) {
                printf("   NPC: 'Чтобы найти выход, найди сундук и иди в сокровищницу!'\n");
            }
            else if (current_location == 1) {
                printf("   NPC: 'В сокровищнице тебя ждёт ключ от выхода!'\n");
            }
            loc->map[x][y] = '.';
        }
        else {
            printf("\n NPC: 'Мы уже говорили. Удачи в пути!'\n");
        }
    }

    else if (cell == 'D') {
        // Финальная локация
        if (current_location == 2) {
            if (loc->chest_taken) {
                printf("\n ДВЕРЬ ОТКРЫТА! ВЫ ПОБЕДИЛИ!\n");
                printf(" ПОЗДРАВЛЯЮ С ПРОХОЖДЕНИЕМ ИГРЫ! \n");
                exit(0);
            }
            else {
                printf("\n Дверь заперта. Нужно найти сундук в сокровищнице!\n");
            }
        }
        else {
            printf("\n Вы подошли к двери...\n");
            printf("   Куда хотите пойти?\n");

            for (int i = 0; i < loc->connections_count; i++) {
                int target = loc->connections[i];
                printf("      %d. %s\n", i + 1, locations[target].name);
            }
            printf("      0. Остаться здесь\n");
            printf("   Ваш выбор: ");

            int choice;
            scanf("%d", &choice);

            if (choice >= 1 && choice <= loc->connections_count) {
                int target_loc = loc->connections[choice - 1];
                change_location(target_loc);
            }
            else {
                printf("\n Вы решили остаться.\n");
            }
        }
    }
}

void show_status() {
    Location* loc = &locations[current_location];
    printf("\n СТАТУС:\n");
    printf("   Сундук: %s\n", loc->chest_taken ? " взят" : " не найден");
    printf("   NPC: %s\n", loc->talked_to_npc ? " поговорили" : " не поговорили");

    if (current_location == 2 && !loc->chest_taken) {
        printf("    Найди сундук (C), чтобы получить ключ!\n");
    }
    if (current_location == 2 && loc->chest_taken) {
        printf("    Иди к двери (D), чтобы выйти!\n");
    }
}
void show_exits() {
    Location* loc = &locations[current_location];
    if (loc->connections_count > 0 && current_location != 2) {
        printf("\n ДВЕРИ ВЕДУТ:\n");
        for (int i = 0; i < loc->connections_count; i++) {
            printf("   → %s\n", locations[loc->connections[i]].name);
        }
    }
}

int main() {
    char input;

    init_locations();

    printf("    ПОДЗЕМЕЛЬЕ - 3 ЛОКАЦИИ \n");
    printf("Управление: W=вверх, S=вниз, A=влево, D=вправо, Q=выход\n");
    printf("Цель: найти сундук (C), поговорить с NPC (N) и найти выход (D)\n");

    while (1) {
        draw_map();
        show_status();
        show_exits();
        if (current_location == 0) {
            printf("\n Подсказка: найди сундук (C) и поговори с NPC (N), затем иди к двери (D)!\n");
        }
        else if (current_location == 1) {
            printf("\n Подсказка: здесь есть дверь в сокровищницу (D)!\n");
        }

        printf("\nКуда идти? ");
        scanf(" %c", &input);

        Location* loc = &locations[current_location];
        int new_x = loc->player_x;
        int new_y = loc->player_y;

        switch (input) {
        case 'w': case 'W': new_x--; break;
        case 's': case 'S': new_x++; break;
        case 'a': case 'A': new_y--; break;
        case 'd': case 'D': new_y++; break;
        case 'q': case 'Q':
            printf("\n До свидания!\n");
            return 0;
        default:
            printf("\n Неверная клавиша! Используйте W/A/S/D или Q\n");
            continue;
        }

        if (can_move(new_x, new_y)) {
            // Перемещение
            loc->player_x = new_x;
            loc->player_y = new_y;

            // Обработка клетки (может вызвать переход)
            handle_cell(new_x, new_y);
        }
        else {
            printf("\n Там стена! Не пройти.\n");
        }
    }

    return 0;
}