/* @file
 *
 * @author Grzegorz Gruza <gg417923@mimuw.edu.pl>
 * @copyright Grzegorz Gruza
 * @date 17.05.2020
 */

#include "interactive.h"

static struct termios old, curr;

/* @brief Pobiera znak bez wypisywania go.
 * @return pobrany znak.
 */
char getch() {
    if (tcgetattr(0, &old) == -1) exit(1);
    curr = old;
    curr.c_lflag &= ~ECHO;
    curr.c_lflag &= ~ICANON;
    if (tcsetattr(0, TCSANOW, &curr) == -1) exit(1);
    char ch = getchar();
    if (tcsetattr(0, TCSANOW, &old) == -1) exit(1);
    return ch;
}

/* @brief Czyści wszystkie widoczne linijki.
 */
void clear_all() {
    printf("\e[1;1H\e[2J");
}

/* @brief Ukrywa kursor.
 * Sprawia, że kursor przestaje być wyświetlany.
 */
void hide_cursor() {
    printf("\e[?25l");
}

/* @brief Pokazuje kursor kursor.
 * Sprawia, że kursor zaczyna być wyświetlany.
 */
void show_cursor() {
    printf("\e[?25h");
}

/* @brief Przenosi kursor we wskazane miejsce.
 * Kursor zmiena swoją lokalizację na nową - wskazaną przez @p col oraz @p row.
 * @param[in] col       – kolumna,
 * @param[in] row       – wiersz.
 */
void move_to(uint32_t col, uint32_t row) {
    printf("\x1b[%d;%df", row, col);
}

/* @brief Czyści linię.
 * Usuwa znaki z linii, na której obecnie znajduje się kursor.
 */
void clear_line() {
    printf("\033[0K");
}

/* @brief Przesuwa kusor na pozyję gracza o współrzędnych x oraz y.
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void move_to_player_position(uint32_t x, uint32_t y, uint32_t length) {
    move_to(1 + x * (length + 1), 1 + y);
}

/* @brief Przesuwa kusor w miejsce wypisywania statystyk gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 */
void move_to_player_stats(gamma_t *g) {
    move_to(0, g->height + 2);
}

/* @brief Wypisuje nr gracza, który nie jest obecnie podświetlany.
 * Nadpisuje tekst - wypisuje nr gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_normal(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    if (g->board[x][y] == 0)
        printf("%*s", length, ".");
    else
        printf("%*" PRIu32, length, g->board[x][y]);
    if (g->players > 9 && x != g->width - 1)
        printf(" ");
}

/* @brief Wypisuje nr gracza, który jest obecnie podświetlany.
 * Zamienia kolor tła z kolorem czcionki,
 * nadpisuje tekst - wypisuje nr gracza,
 * oraz zmienia kolor tła i czcionki na początkowe.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_reversed(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    printf("\033[7m");
    if (g->board[x][y] == 0)
        printf("%*s", length, ".");
    else
        printf("%*" PRIu32, length, g->board[x][y]);
    printf("\033[0m");
    if (g->players > 9 && x != g->width - 1)
        printf(" ");
}

/* @brief Wypisuje nr gracza, po błędnym ruchu.
 * Zmienia kolor tła na czerwony i nadpisuje tekst - wypisuje nr gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_red(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    printf("\033[41m\033[37m");
    if (g->board[x][y] == 0)
        printf("%*s", length, ".");
    else
        printf("%*" PRIu32, length, g->board[x][y]);
    printf("\033[0m");
    if (g->players > 9 && x != g->width - 1)
        printf(" ");
}

/* @brief Wypisuje nr gracza, po błędnym ruchu.
 * Zmienia kolor tła na zielony i nadpisuje tekst - wypisuje nr gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_green(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    printf("\033[42m\033[37m");
    if (g->board[x][y] == 0)
        printf("%*s", length, ".");
    else
        printf("%*" PRIu32, length, g->board[x][y]);
    printf("\033[0m");
    if (g->players > 9 && x != g->width - 1)
        printf(" ");
}

/* @brief Wypisuje na planszy gracza, który jest obecnie podświetlany.
 * Przesuwa kursor w odpowiednie miejsce na planszy,
 * zamienia kolor tła z kolorem czcionki,
 * nadpisuje znajdujący się w tym miejscu tekst - wstawia w to miejsce nr gracza
 * oraz zmienia kolor tła i czcionki na początkowe.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void set_reversed_player(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    move_to_player_position(x, y, length);
    print_reversed(g, x, y, length);
}


/* @brief Wypisuje na planszy gracza, który nie jest obecnie podświetlany.
 * Przesuwa kursor w odpowiednie miejsce na planszy i nadpisuje
 * znajdujący się w tym miejscu tekst - wstawia w to miejsce numer gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void set_normal_player(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    move_to_player_position(x, y, length);
    print_normal(g, x, y, length);
}

/* @brief Wypisuje (czerwone tło) na planszy gracza, który wykonał błędny ruch.
 * Przesuwa kursor w odpowiednie miejsce na planszy i nadpisuje
 * znajdujący się w tym miejscu tekst - wstawia w to miejsce numer gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void set_red_player(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    move_to_player_position(x, y, length);
    print_red(g, x, y, length);
}

/* @brief Wypisuje (zielone tło) na planszy gracza, który wykonał błędny ruch.
 * Przesuwa kursor w odpowiednie miejsce na planszy i nadpisuje
 * znajdujący się w tym miejscu tekst - wstawia w to miejsce numer gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void set_green_player(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    move_to_player_position(x, y, length);
    print_green(g, x, y, length);
}

/* @brief Podświetla pole gracza, który wykonał błędy ruch.
 * Podświetla pole gracza, który wykonał błędny ruch na czerwono,
 * a po 0.3 sekundy zamienia spowrotem kolor na odwrócony.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void bad_move(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    set_red_player(g, x, y, length);
    printf("\a\n");
    if (tcgetattr(0, &old) == -1) exit(1);

    curr = old;
    curr.c_lflag &= ~ECHO;
    curr.c_lflag &= ICANON;
    curr.c_lflag &= ECHOK;
    if (tcsetattr(0, TCSANOW, &curr) == -1) exit(1);

    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 30000000;
    if (nanosleep(&tim, &tim2) < 0) {
        exit(1);
    } else {
        set_reversed_player(g, x, y, length);
    }
    if (tcsetattr(0, TCSANOW, &old) == -1) exit(1);
    fflush(stdin);
}

/* @brief Podświetla pole gracza, który wykonał poprawny ruch.
 * Podświetla pole gracza, który wykonał poprawny ruch na zielono,
 * a po 0.3 sekundy zamienia spowrotem kolor na odwrócony.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa gracza,
 * @param[in] y       – pozycja y-owa gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void good_move(gamma_t *g, uint32_t x, uint32_t y, uint32_t length) {
    set_green_player(g, x, y, length);
    printf("\n");
    if (tcgetattr(0, &old) == -1) exit(1);

    curr = old;
    curr.c_lflag &= ~ECHO;
    curr.c_lflag &= ICANON;
    curr.c_lflag &= ECHOK;
    if (tcsetattr(0, TCSANOW, &curr) == -1) exit(1);

    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 30000000;
    if (nanosleep(&tim, &tim2) < 0) {
        exit(1);
    } else {
        set_reversed_player(g, x, y, length);
    }
    if (tcsetattr(0, TCSANOW, &old) == -1) exit(1);
    fflush(stdin);
}

/* @brief Wypisuje nr gracza i dotyczące go atrybuty.
 * Przesuwa kursor w odpowiednie miejsce, czyści inijkę oraz wypisuje:
 * numer gracza @p player, gamma_busy_fields i gamma_free_fields dla @p player
 * oraz 'G' jeśli gamma_golden_possible dla @p player zwróciło @p true.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] player  – numer gracza,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_player_during_game(gamma_t *g, uint32_t player, uint32_t length) {
    move_to_player_stats(g);
    if (g->players > 9) length++;
    uint32_t total_length = (length * g->width) / 2;
    if (total_length < 18) total_length = 18;

    clear_line();
    printf("%*s", total_length, "Player: ");
    printf("\e[1m%u\n\e[0m", player);
    clear_line();
    printf("%*s", total_length, "Busy fields: ");
    printf("\e[1m%lu\n\e[0m", gamma_busy_fields(g, player));
    clear_line();
    printf("%*s", total_length, "Available fields: ");
    printf("\e[1m%lu\n\e[0m", gamma_free_fields(g, player));
    clear_line();
    printf("%*s", total_length, "Golden move: ");
    if (gamma_golden_possible(g, player)) printf("\e[1mYes\e[0m");
    else printf("\e[1mNo\e[0m");
}


/* @brief Wypisuje nr graczy i dotyczące go atrybuty.
 * Przesuwa kursor w odpowiednie miejsce, czyści inijkę
 * oraz dla każdego gracza, który brał udział w rozgrywce wypisuje:
 * numer gracza oraz gamma_busy_fields wywołane dla gracza.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_players_end_of_game(gamma_t *g, uint32_t length) {
    move_to_player_stats(g);
    if (g->players > 9) length++;
    uint32_t total_length = (length * g->width) / 2;
    if (total_length < 18) total_length = 18;

    printf("%*s", total_length, "Player  ");
    printf("Result\n");

    uint32_t max_busy = 0;
    for (uint32_t p = 1; p <= g->players; p++) {
        if (gamma_busy_fields(g, p) > max_busy)
            max_busy = gamma_busy_fields(g, p);
    }
    for (uint32_t p = 1; p <= g->players; p++) {
        clear_line();
        printf("%*" PRIu32, total_length - 2, p);
        printf("  ");
        if (gamma_busy_fields(g, p) == max_busy)
            printf("%lu 👑\n", gamma_busy_fields(g, p));
        else
            printf("%lu\n", gamma_busy_fields(g, p));
    }
    clear_line();
    printf("\n");
}

/* @brief Wypisuje planszę dla obecnego stanu gry.
 * Czyści wszysko, przesuwa kursor na początek oraz wypisuje planszę,
 * tak aby na pozycji (1, 1) rozpoczynał się napis oznaczający gracza (0, 0).
 * Zera wypisuje jako '.'.
 * Jeśli jest więcej niż 9 graczy, to między kolumnami wypisuje spacje.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] act_x   – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] act_y   – pozycja y-owa aktualnie podświetlanego pola,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void print_gamma(gamma_t *g, uint32_t length, uint32_t act_x, uint32_t act_y) {
    clear_all();
    for (uint32_t y = 0; y < g->height; y++) {
        for (uint32_t x = 0; x < g->width; x++) {
            if (x == act_x && y == act_y) print_reversed(g, x, y, length);
            else print_normal(g, x, y, length);
        }
        printf("\n");
    }
}

/* @brief Zmienia nr gracza na pierwszego kolejnego, który może wykonać ruch.
 * Znajduje pierwszego w kolejności rozgrywki gracza po graczu @p p,
 * dla którego gamma_free_fields zwróciło dodatnią liczbę
 * lub gamma_golden_possible zwróciło @p true.
 * Jeśli taki gracz nie istnieje, to ustawia @p p na @p 0;
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[out] p      – gracz, który aktualnie wykonuje ruch.
 */
void next_player(gamma_t *g, uint32_t *p) {
    uint32_t start_p = *p;
    if (*p == g->players) *p = 1;
    else (*p)++;

    while (gamma_free_fields(g, *p) == 0 && !gamma_golden_possible(g, *p)) {
        if (*p == g->players) *p = 1;
        else (*p)++;

        if (*p == start_p
            && gamma_free_fields(g, *p) == 0
            && !gamma_golden_possible(g, *p)) {
            *p = 0;
            return;
        }
    }
}

/* @brief Kończy działanie interactive_mode.
 * Wyłącza podświetlanie aktywnego gracza, wypisuje końcowy stan graczy,
 * usuwa @p g, pokazuje kursor oraz kończy działanie programu z kodem @p 0
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami,
 * @param[in] x       – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] y       – pozycja y-owa aktualnie podświetlanego pola.
 */
void end_interactive_game(gamma_t *g, uint32_t length, uint32_t x, uint32_t y) {
    set_normal_player(g, x, y, length);
    print_players_end_of_game(g, length);
    gamma_delete(g);
    show_cursor();
    exit(0);
}

/* @brief Przesuwa podświetlenie pola o jedno pole w górę (o ile się da)
 * Sprawdza, czy istnieje pole znajdujące się jedno pole w górę.
 * Jeśli tak, to usuwa podświetlenie z obecnego pola i podświetla pole powyżej.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] y       – pozycja y-owa aktualnie podświetlanego pola,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void try_arrow_up(gamma_t *g, uint32_t *x, uint32_t *y, uint32_t length) {
    if (*y != 0) {
        set_normal_player(g, *x, *y, length);
        (*y)--;
        set_reversed_player(g, *x, *y, length);
    }
}

/* @brief Przesuwa podświetlenie pola o jedno pole w dół (o ile się da)
 * Sprawdza, czy istnieje pole znajdujące się jedno pole w dół.
 * Jeśli tak, to usuwa podświetlenie z obecnego pola i podświetla pole poniżej.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] y       – pozycja y-owa aktualnie podświetlanego pola,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void try_arrow_down(gamma_t *g, uint32_t *x, uint32_t *y, uint32_t length) {
    if (*y != g->height - 1) {
        set_normal_player(g, *x, *y, length);
        (*y)++;
        set_reversed_player(g, *x, *y, length);
    }
}

/* @brief Przesuwa podświetlenie pola o jedno pole w prawo (o ile się da)
 * Sprawdza, czy istnieje pole znajdujące się jedno pole w prawo.
 * Jeśli tak, to usuwa podświetlenie z obecnego pola i podświetla pole z prawej.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] y       – pozycja y-owa aktualnie podświetlanego pola,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void try_arrow_right(gamma_t *g, uint32_t *x, uint32_t *y, uint32_t length) {
    if (*x != g->width - 1) {
        set_normal_player(g, *x, *y, length);
        (*x)++;
        set_reversed_player(g, *x, *y, length);
    }
}

/* @brief Przesuwa podświetlenie pola o jedno pole w lewo (o ile się da)
 * Sprawdza, czy istnieje pole znajdujące się jedno pole w lewo.
 * Jeśli tak, to usuwa podświetlenie z obecnego pola i podświetla pole z lewej.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] y       – pozycja y-owa aktualnie podświetlanego pola,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void try_arrow_left(gamma_t *g, uint32_t *x, uint32_t *y, uint32_t length) {
    if (*x != 0) {
        set_normal_player(g, *x, *y, length);
        (*x)--;
        set_reversed_player(g, *x, *y, length);
    }
}

/* @brief Sprawdza czy ostatnio wczytana sekwencja znaków jest strzałką.
 * Zakłada, że dwa znaki temu wpisano ESC, a znak temu '['.
 * Sprawdza, czy obecnie wczytany znak to A, B, C lub D. Jeśli tak, to wywołuje
 * odpowiednio: try_arrow_up, try_arrow_down, try_arrow_right, try_arrow_left.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa aktualnie podświetlanego pola,
 * @param[in] y       – pozycja y-owa aktualnie podświetlanego pola,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami,
 * @param[in] ch      – ostatnio wczytany znak.
 */
void try_arrows(gamma_t *g, uint32_t *x, uint32_t *y, uint32_t length, char ch) {
    if (ch == 'A') try_arrow_up(g, x, y, length);
    else if (ch == 'B') try_arrow_down(g, x, y, length);
    else if (ch == 'C') try_arrow_right(g, x, y, length);
    else if (ch == 'D') try_arrow_left(g, x, y, length);
}

/* @brief Wykonuje złoty ruch (o ile się da)
 * Wywołuje @p gamma_goldem_move i w wyniku @p true aktualizuje odpowiednie pole.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa ruchu,
 * @param[in] y       – pozycja y-owa ruchu,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 * @param[in] p       – gracz, który próbuje wykonać ruch.
 */
void try_GM(gamma_t *g, uint32_t x, uint32_t y, uint32_t length, uint32_t *p) {
    if (gamma_golden_move(g, *p, x, y)) {
        good_move(g, x, y, length);
        next_player(g, p);
        if (*p == 0) end_interactive_game(g, length, x, y);
        print_player_during_game(g, *p, length);
    } else {
        bad_move(g, x, y, length);
    }
}

/* @brief Wykonuje zwykły ruch (o ile się da)
 * Wywołuje @p gamma_goldem_move i w wyniku @ true aktualizuje odpowiednie pole.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – pozycja x-owa ruchu,
 * @param[in] y       – pozycja y-owa ruchu,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 * @param[in] p       – gracz, który próbuje wykonać ruch.
 */
void try_M(gamma_t *g, uint32_t x, uint32_t y, uint32_t length, uint32_t *p) {
    if (gamma_move(g, *p, x, y)) {
        good_move(g, x, y, length);
        next_player(g, p);
        if (*p == 0) end_interactive_game(g, length, x, y);
        print_player_during_game(g, *p, length);
    } else {
        bad_move(g, x, y, length);
    }
}

/* @brief Sprawdza, czy terminal ma odpowiedni rozmiar
 * Jeśli terminal jest zbyt mayły, to kończy rozgrywkę i wypisuje komunikat.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] length  – długość, jaką ma zająć napis gracza razem ze spacjami.
 */
void check_win_size(gamma_t *g, uint32_t length) {
    struct winsize max;
    ioctl(0, TIOCGWINSZ, &max);
    length++;
    if (max.ws_col < g->width * length
        || max.ws_col < 18 + length
        || max.ws_row < g->height + 5) {
        printf("\033[91m\033[47m");
        printf("Za mały terminal!");
        printf("\033[0m\n");
        exit(1);
    }
}

void interactive_mode(gamma_t *g) {
    uint32_t act_player = 1;

    uint32_t length = snprintf(NULL, 0, "%" PRIu32, g->players);
    if (g->players <= 9) length--;
    check_win_size(g, length);

    uint32_t act_x = (g->width - 1) / 2;
    uint32_t act_y = (g->height) / 2;

    hide_cursor();
    print_gamma(g, length, act_x, act_y);
    print_player_during_game(g, 1, length);

    bool entered_escape = false;
    bool entered_bracket = false;
    char ch;

    while (true) {
        ch = getch();
        if (ch == '\e') {//pierwszy znak strzałki
            entered_escape = true;
            entered_bracket = false;
        } else if (ch == '[' && entered_escape == true) {//drugi znak strzałki
            if (entered_bracket == false) entered_bracket = true;
            else entered_escape = false;
        } else {
            if (ch == 4) {//Ctrl + D
                end_interactive_game(g, length, act_x, act_y);
            } else if (ch == ' ') {//zwykły ruch
                try_M(g, act_x, act_y, length, &act_player);
            } else if (ch == 'G' || ch == 'g') {//złoty ruch
                try_GM(g, act_x, act_y, length, &act_player);
            } else if (entered_escape && entered_bracket) {//trzeci znak strzałki
                try_arrows(g, &act_x, &act_y, length, ch);
            } else if (ch == 'C' || ch == 'c') {//pomijamy ruch
                next_player(g, &act_player);
                print_player_during_game(g, act_player, length);
            }
            entered_escape = false;
            entered_bracket = false;
        }
    }
}
