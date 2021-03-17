/* @file
 *
 * @author Grzegorz Gruza <gg417923@mimuw.edu.pl>
 * @copyright Grzegorz Gruza
 * @date 17.04.2020
 */

#include "gamma.h"

/* @brief tworzy zmienną typu pair_t o zadanych elementach pary
 * zakłada że poniższe parametry wejściowe są poprawne:
 * @param[in] st  – pierwszy element pary,
 * @param[in] nd  – drugi element pary.
 */
pair_t make_pair(uint32_t st, uint32_t nd){
  pair_t to_return;
  to_return.st = st;
  to_return.nd = nd;
  return to_return;
}

/* @brief Sprawdza, czy dwie zmienne typu pairt_t są równe.
 * @param[in] a  – pierwsza zmienna typu pair_t,
 * @param[in] b  – druga zmienna typu pair_t
 * Sprawdza, czy @p a.st jest równe @p a.st.
 * Sprawdza, czy @p a.nd jest równe @p a.nd
 * @return Wartość @p true zachodzi równość
 * a @p false w przeciwnym przypadku.
 */
bool equal(pair_t a, pair_t b){
  if(a.st != b.st || a.nd != b.nd) return false;
  return true;
}

/* @brief Inicjalizuje struktury parent i rank
 * Dla każdego pola, ustawia wartość rank na zero.
 * Dla każdego pola, ustawia wartość parent na parę, której pierwszym elementem
 * jest współrzędna x-owa pola, a drugim elementem jest współrzędna y-owa pola.
 * @param[in] parent  – tablica parent struktury find and union, w której
 *                      przechowywany jest bezpośredni przodeg danego pola,
 * @param[in] rank    – tablica rang elementów find and union, w której
 *                      przechowywana jest odległość od najdalszego dziecka,
 * @param[in] width   – szerokość tablic @p rank oraz @p parent,
 * @param[in] height – wysokość tablic @p rank oraz @p parent
 *                      @p players z funkcji @ref gamma_new.
 */
void fu_init(pair_t** parent, uint32_t** rank, uint32_t width, uint32_t height){
  for(uint32_t x = 0; x < width; x++){
    for(uint32_t y = 0; y < height; y++){
      parent[x][y].st = x;
      parent[x][y].nd = y;
      rank[x][y] = 0;
    }
  }
}

/* @brief Znajduje najdalszego przodka (reprezentanta)
 * Podaje liczbę wolnych pól, na których w danym stanie gry gracz @p player może
 * postawić swój pionek w następnym ruchu.
 * @param[in] parent – tabilca ojcostwa struktury find and union,
 * @param[in] p      – współrzędne pola w tablicy, dla którego chcemy znaleźć
 *                     najdalszego przodka.
 * @return Para, której przerwszym elementem jest pierwsza współrzędna
 * najdalszego przodka, a drugim elementem jest druga współrzędna tego przodka.
 */
pair_t* fu_find(pair_t** parent, pair_t* p){
  if((parent[p->st][p->nd]).st != p->st
  || (parent[p->st][p->nd]).nd != p->nd){
    *p = *(fu_find(parent, &(parent[p->st][p->nd])));
  }
  return p;
}

/* @brief Łączy dwa alementy ze sobą
 * Łączy ze sobą dwa pola struktury find and union.
 * Zmienia tablicę ojcostwa. Jako ojca reprezentanta pola o mniejszej randze
 * ustawia reprezentanta pola o większej randze.
 * Aktualizuje tablicę @p rank.
 * @param[in] parent – tabilca ojcostwa struktury find and union,
 * @param[in] rank   – tablica rang elementów struktury find and union,
 * @param[in] a      – współrzędne pierwszego pola reprezentowane jako para,
 * @param[in] b      – współrzędne drugiego pola reprezentowane jako para.
 */
void fu_union(pair_t** parent, uint32_t** rank, pair_t* a, pair_t* b){
  pair_t par_a = *fu_find(parent, a);
  pair_t par_b = *fu_find(parent, b);
  if(rank[a->st][a->nd] < rank[b->st][b->nd]){
    parent[par_a.st][par_a.nd] = par_b;
  }
  else{
    parent[par_b.st][par_b.nd] = par_a;
  }
  if(rank[a->st][a->nd] == rank[b->st][b->nd]){
    rank[a->st][a->nd]++;
  }
}

//zakłada że pola należą do tego samego gracza oraz low jest aktualny
bool low_is_parent(gamma_t *g, uint32_t x_p, uint32_t y_p,
                   uint32_t x_s, uint32_t y_s){
  if(g->low_visit_time[x_p][y_p] > g->low_visit_time[x_s][y_s])
    return false;

  if(x_s != 0
  && g->low_visit_time[x_s][y_s] > g->low_visit_time[x_s - 1][y_s]
  && g->low_visit_time[x_s - 1][y_s] > g->low_visit_time[x_p][y_p])
    return false;

  if(x_s != g->width - 1
  && g->low_visit_time[x_s][y_s] > g->low_visit_time[x_s + 1][y_s]
  && g->low_visit_time[x_s + 1][y_s] > g->low_visit_time[x_p][y_p])
    return false;

  if(y_s != 0
  && g->low_visit_time[x_s][y_s] > g->low_visit_time[x_s][y_s - 1]
  && g->low_visit_time[x_s][y_s - 1] > g->low_visit_time[x_p][y_p])
    return false;

  if(y_s != g->height - 1
  && g->low_visit_time[x_s][y_s] > g->low_visit_time[x_s][y_s + 1]
  && g->low_visit_time[x_s][y_s + 1] > g->low_visit_time[x_p][y_p])
    return false;

  return true;
}

bool low_up_to_date_area(gamma_t *g, uint32_t x, uint32_t y){
  pair_t xy = make_pair(x, y);
  pair_t rep = *fu_find(g->parent, &xy);
  return (g->low_visit_time[rep.st][rep.nd] != 0);
}

void low_set_not_up_to_date(gamma_t *g, uint32_t x, uint32_t y){
  pair_t xy = make_pair(x, y);
  pair_t *rep = fu_find(g->parent, &xy);
  g->low_visit_time[rep->st][rep->nd] = 0;
}

void low_clear(gamma_t *g, uint32_t x, uint32_t y){
  g->low_visit_time[x][y] = 0;

  if(x != 0
  && g->board[x - 1][y] == g->board[x][y]
  && g->low_visit_time[x - 1][y] != 0){
    low_clear(g, x - 1, y);
  }
  if(x != g->width - 1
  && g->board[x + 1][y] == g->board[x][y]
  && g->low_visit_time[x + 1][y] != 0){
    low_clear(g, x + 1, y);
  }
  if(y != 0
  && g->board[x][y - 1] == g->board[x][y]
  && g->low_visit_time[x][y - 1] != 0){
    low_clear(g, x, y - 1);
  }
  if(y != g->height - 1
  && g->board[x][y + 1] == g->board[x][y]
  && g->low_visit_time[x][y + 1] != 0){
    low_clear(g, x, y + 1);
  }
}

void low_compute(gamma_t *g, uint32_t x, uint32_t y, uint64_t *visit_time);

void low_compute_son(gamma_t *g, uint32_t x_p, uint32_t y_p,
                     uint64_t *visit_time, uint32_t x_s, uint32_t y_s){
  if(g->low_visit_time[x_s][y_s] == 0){ //(x_s, y_s) zostaje synem (x_p, y_p)
    low_compute(g, x_s, y_s, visit_time);
    if(g->low[x_p][y_p] > g->low[x_s][y_s]){
      g->low[x_p][y_p] = g->low[x_s][y_s];
    }
  }else{ //(x_s, y_s) ma już ojca
    if(low_is_parent(g, x_s, y_s, x_p, y_p)) //(x_s, y_s) jest ojcem (x_p, y_p)
      return;

    if(g->low[x_p][y_p] > g->low_visit_time[x_s][y_s])
      g->low[x_p][y_p] = g->low_visit_time[x_s][y_s];
  }
}

void low_compute(gamma_t *g, uint32_t x, uint32_t y, uint64_t *visit_time){
  (*visit_time)++;
  g->low_visit_time[x][y] = *visit_time;
  g->low[x][y] = *visit_time;

  if(x != 0 && g->board[x - 1][y] == g->board[x][y]){
    low_compute_son(g, x, y, visit_time, x - 1, y);
  }

  if(x != g->width - 1 && g->board[x + 1][y] == g->board[x][y]){
    low_compute_son(g, x, y, visit_time, x + 1, y);
  }

  if(y != 0 && g->board[x][y - 1] == g->board[x][y]){
    low_compute_son(g, x, y, visit_time, x, y - 1);
  }

  if(y != g->height - 1 && g->board[x][y + 1] == g->board[x][y]){
    low_compute_son(g, x, y, visit_time, x, y + 1);
  }
}

void low_update(gamma_t *g, uint32_t x, uint32_t y){
  if(low_up_to_date_area(g, x, y)){
    return;
  }
  low_clear(g, x, y);
  uint64_t visit_time = 0;
  low_compute(g, x, y, &visit_time);
}

void low_update_all(gamma_t *g){
  for(uint32_t x = 0; x < g->width; x++){
    for(uint32_t y = 0; y < g->height; y++){
      if(g->board[x][y] != 0)
        low_update(g, x, y);
    }
  }
}

bool low_bridge(gamma_t *g, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2){
  if(g->board[x1][y1] != g->board[x2][y2]
  || g->board[x1][y1] == 0
  || g->board[x2][y2] == 0)
    return false;

  if(low_is_parent(g, x1, y1, x2, y2)
  && g->low[x2][y2] == g->low_visit_time[x2][y2])
    return true;

  if(low_is_parent(g, x2, y2, x1, y1)
  && g->low[x1][y1] == g->low_visit_time[x1][y1])
    return true;

  return false;
}

bool low_articulation_point(gamma_t *g, uint32_t x, uint32_t y){
  if(g->low_visit_time[x][y] == 1){
    uint32_t sons = 0;
    if(x != 0 && g->board[x - 1][y] == g->board[x][y]) sons++;
    if(x != g->width - 1 && g->board[x + 1][y] == g->board[x][y]) sons++;
    if(y != 0 && g->board[x][y - 1] == g->board[x][y]) sons++;
    if(x != g->height - 1 && g->board[x][y + 1] == g->board[x][y]) sons++;
    return (sons > 1);
  }
  if(x != 0 && g->board[x - 1][y] == g->board[x][y]
  && low_is_parent(g, x, y, x - 1, y)
  && g->low[x - 1][y] >= g->low_visit_time[x][y])
    return true;

  if(x != g->width - 1 && g->board[x + 1][y] == g->board[x][y]
  && low_is_parent(g, x, y, x + 1, y)
  && g->low[x + 1][y] >= g->low_visit_time[x][y])
    return true;

  if(y != 0 && g->board[x][y - 1] == g->board[x][y]
  && low_is_parent(g, x, y, x, y - 1)
  && g->low[x][y - 1] >= g->low_visit_time[x][y])
    return true;

  if(y != g->height - 1 && g->board[x][y + 1] == g->board[x][y]
  && low_is_parent(g, x, y, x, y + 1)
  && g->low[x][y + 1] >= g->low_visit_time[x][y])
    return true;

  return false;
}

void gamma_delete(gamma_t *g){
  if(g == NULL) return;
  if(g->fields_next_to != NULL) free(g->fields_next_to);
  if(g->busy_fields != NULL) free(g->busy_fields);
  if(g->used_areas != NULL) free(g->used_areas);
  if(g->board != NULL){
    for(uint32_t i = 0; i < g->width; i++)
      if((g->board)[i] != NULL) free((g->board)[i]);
    free(g->board);
  }
  if(g->is_golden_used != NULL) free(g->is_golden_used);
  if(g->parent != NULL){
    for(uint32_t i = 0; i < g->width; i++)
      if((g->parent)[i] != NULL) free((g->parent)[i]);
    free(g->parent);
  }
  if(g->rank != NULL){
    for(uint32_t i = 0; i < g->width; i++)
      if((g->rank)[i] != NULL) free((g->rank)[i]);
    free(g->rank);
  }
  if(g->low != NULL){
    for(uint32_t i = 0; i < g->width; i++)
      if((g->low)[i] != NULL) free((g->low)[i]);
    free(g->low);
  }
  if(g->low_visit_time != NULL){
    for(uint32_t i = 0; i < g->width; i++)
      if((g->low_visit_time)[i] != NULL) free((g->low_visit_time)[i]);
    free(g->low_visit_time);
  }
  free(g);
}


/* @brief Alokuje tablicę jednowymiarową.
 * Tablica alokowana jest jako is_golden_used struktury gamma.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] players – liczba graczy.
 * @return Wartość @p true jeśli alokacja powiodła się,
 * a @p false w przeciwnym razie.
 */
bool gamma_alloc_golden(gamma_t *g, uint32_t players){
  g->is_golden_used = calloc(1, players*sizeof(bool));
  if(g->is_golden_used == NULL){
    gamma_delete(g);
    return false;
  }
  return true;
}

/* @brief Alokuje tablicę jednowymiarową.
 * Tablica alokowana jest jako @p used_areas struktury gamma.
 * Ustawia wartość areas na odpowiednią wartość.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] players – liczba graczy
 * @param[in] areas   – maksymalna liczba obszarów,
 *                      jakie może zająć jeden gracz.
 * @return Wartość @p true jeśli alokacja powiodła się,
 * a @p false w przeciwnym razie.
 */
bool gamma_alloc_areas(gamma_t *g, uint32_t players, uint32_t areas){
  g->areas = areas;
  g->used_areas = calloc(1, players*sizeof(uint32_t));
  if(g->used_areas == NULL){
    gamma_delete(g);
    return false;
  }
  return true;
}


/* @brief Alokuje dwie @p jednowymiarowe.
 * Tablice alokowane są jako odpowiednio @p fields_next_to
 * oraz @p busy_fields struktury gamma.
 * Ustawia wartości busy_fields_all na zero.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] players – liczba graczy
 * @return Wartość @p true jeśli alokacja powiodła się,
 * a @p false w przeciwnym razie.
 */
bool gamma_alloc_fields(gamma_t *g, uint32_t players){
  g->fields_next_to = calloc(1, players*sizeof(uint64_t));
  if(g->fields_next_to == NULL){
    gamma_delete(g);
    return false;
  }

  g->busy_fields = calloc(1, players*sizeof(uint64_t));
  if(g->busy_fields == NULL){
    gamma_delete(g);
    return false;
  }
  g->busy_fields_all = 0;
  return true;
}


/* @brief Alokuje tablicę dwuwymiarową.
 * Tablica alokowana jest jako @p board struktury gamma
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] width   – szerokość tablicy,
 * @param[in] height  – wysokość tablicy.
 * @return Wartość @p true jeśli alokacja powiodła się,
 * a @p false w przeciwnym razie.
 */
bool gamma_alloc_board(gamma_t *g, uint32_t width, uint32_t height){
  g->board = calloc(1, width*sizeof(uint32_t*));
  if(g->board == NULL){
    gamma_delete(g);
    return false;
  }
  for(uint32_t i = 0; i < width; i++){
    g->board[i] = calloc(1, height*sizeof(uint32_t));
    if(g->board[i] == NULL){
      gamma_delete(g);
      return false;
    }
  }
  return true;
}

/* @brief Alokuje tablicę dwuwymiarową.
 * Tablica alokowana jest jako @p parent struktury gamma
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] width   – szerokość tablicy,
 * @param[in] height  – wysokość tablicy.
 * @return Wartość @p true jeśli alokacja powiodła się,
 * a @p false w przeciwnym razie.
 */
bool gamma_alloc_parent(gamma_t *g, uint32_t width, uint32_t height){
  g->parent = calloc(1, width*sizeof(pair_t*));
  if(g->parent == NULL){
    gamma_delete(g);
    return false;
  }
  for(uint32_t i = 0; i < width; i++){
    g->parent[i] = calloc(1, height*sizeof(pair_t));
    if(g->parent[i] == NULL){
      gamma_delete(g);
      return false;
    }
    for(uint32_t j = 0; j < height; j++)
    g->parent[i][j] = make_pair(i, j);
  }
  return true;
}

/* @brief Alokuje dwuwymiarową tablicę.
 * Tablica alokowana jest jako @p rank struktury gamma
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] width   – szerokość tablicy,
 * @param[in] height  – wysokość tablicy.
 * @return Wartość @p true jeśli alokacja powiodła się,
 * a @p false w przeciwnym razie.
 */
bool gamma_alloc_rank(gamma_t *g, uint32_t width, uint32_t height){
  g->rank = calloc(1, width*sizeof(uint32_t*));
  if(g->rank == NULL){
    gamma_delete(g);
    return false;
  }
  for(uint32_t i = 0; i  < width; i++){
    g->rank[i] = calloc(1, height*sizeof(uint32_t));
    if(g->rank[i] == NULL){
      gamma_delete(g);
      return false;
    }
  }
  return true;
}

bool gamma_alloc_low(gamma_t *g, uint32_t width, uint32_t height){
  g->low = calloc(1, width*sizeof(uint64_t*));
  g->low_visit_time = calloc(1, width*sizeof(uint64_t*));
  if(g->low == NULL
  || g->low_visit_time == NULL){
    gamma_delete(g);
    return false;
  }
  for(uint32_t i = 0; i  < width; i++){
    g->low[i] = calloc(1, height*sizeof(uint64_t));
    g->low_visit_time[i] = calloc(1, height*sizeof(uint64_t));
    if(g->low[i] == NULL
    || g->low_visit_time[i] == NULL
  ){
      gamma_delete(g);
      return false;
    }
  }
  return true;
}

/* @brief Sprawdza poprawność argumentów funkcji gamma_new.
*/
bool gamma_new_params(uint32_t width, uint32_t height,
                  uint32_t players, uint32_t areas){
  return !(width < 1
        || height < 1
        || players < 1
        || areas == 0);
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                  uint32_t players, uint32_t areas){
  if(!gamma_new_params(width, height, players, areas)){
    return NULL;
  }

  gamma_t* g = calloc(1, sizeof(gamma_t));
  if(g == NULL) return NULL;
  g->width = width;
  g->height = height;
  g->players = players;
  if(gamma_alloc_fields(g, players) == false) return NULL;
  if(gamma_alloc_areas(g, players, areas) == false) return NULL;
  if(gamma_alloc_board(g, width, height) == false) return NULL;
  if(gamma_alloc_golden(g, players) == false) return NULL;
  if(gamma_alloc_parent(g, width, height) == false) return NULL;
  if(gamma_alloc_rank(g, width, height) == false) return NULL;
  if(gamma_alloc_low(g, width, height) == false) return NULL;
  fu_init(g->parent, g->rank, g->width, g->height);

  return g;
}

/* @brief Zlicza ile pól zajętych przez gracza @p player jest obok pola
 * Dla każdego pola będącego w sąsiedzctwie z polem [x][y]
 * sprawdza, czy jest ono zajęte przez gracza @p player.
 * Jeśli tak, to dodaje jeden do wyniku
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] player  – gracz, którego pól szukamy,
 * @param[in] x       – współrzędna x-owa pola,
 * @param[in] y       – wysokość y-owa pola.
 * @return Licza sąsiednich pól zajętych przez @p player
 */
uint32_t number_of_fields_next_to(gamma_t *g, uint32_t player, uint32_t x, uint32_t y){
  if(g->board[x][y] != 0) return 0;
  uint32_t to_return = 0;
  if(x != 0 && g->board[x-1][y] == player) to_return++;
  if(x != g->width - 1 && g->board[x+1][y] == player) to_return++;
  if(y != 0 && g->board[x][y-1] == player) to_return++;
  if(y != g->height - 1 && g->board[x][y+1] == player) to_return++;
  return to_return;
}

/* @brief Aktualizuje fields_next_to[x][y] po ruchu na pole [x][y]
 * Jeśli wartość @p add jest równa @p false, to ruch traktowany jest jako
 * "ujemny" – polega na zdjęciu z pola [x][y] pionka gracza,
 * który stoi na tym polu.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – współrzędna x-owa pola,
 * @param[in] y       – wysokość y-owa pola,
 * @param[in] add     – zmienna, która określa, czy na pole [x][y] postawiono.
 *                      przed chwilą pionek, czy za chwilę z tego pola pionek
 *                      zostanie zdjęty.
 */
void update_this_fields_next_to(gamma_t *g, uint32_t x, uint32_t y, bool add){
  uint64_t sign;
  if(add == true) sign = +1;
  else sign = -1;
  uint32_t player = g->board[x][y];

  if(x != 0 && g->board[x - 1][y] == 0
  && number_of_fields_next_to(g, player, x - 1, y) == 1){
    g->fields_next_to[player - 1] += sign;
  }
  if(x != g->width - 1 && g->board[x + 1][y] == 0
  && number_of_fields_next_to(g, player, x + 1, y) == 1){
    g->fields_next_to[player - 1] += sign;
  }
  if(y != 0 && g->board[x][y - 1] == 0
  && number_of_fields_next_to(g, player, x, y - 1) == 1){
    g->fields_next_to[player - 1] += sign;
  }
  if(y != g->height - 1 && g->board[x][y + 1] == 0
  && number_of_fields_next_to(g, player, x, y + 1) == 1){
    g->fields_next_to[player - 1] += sign;
  }
}

/* @brief Aktualizuje fields_next_to pól obok pola [x][y] po ruchu na [x][y].
 * Jeśli wartość @p add jest równa @p false, to ruch traktowany jest jako
 * "ujemny" – polega na zdjęciu z pola [x][y] pionka gracza,
 * który stoi na tym polu.
 * @param[in] g       – wskaźnik na strukturę gamma_t,
 * @param[in] x       – współrzędna x-owa pola,
 * @param[in] y       – wysokość y-owa pola,
 * @param[in] add     – zmienna, która określa, czy na pole [x][y] postawiono.
 *                      przed chwilą pionek, czy za chwilę z tego pola pionek
 *                      zostanie zdjęty.
 */
void update_around_fields_next_to(gamma_t *g, uint32_t x, uint32_t y, bool add){
  uint64_t sign;
  if(add == true) sign = +1;
  else sign = -1;

  if(x != 0 && g->board[x - 1][y] != 0){
    g->fields_next_to[g->board[x - 1][y] - 1] -= sign;
  }
  if(x != g->width - 1 && g->board[x + 1][y] != 0
  && (x == 0 || g->board[x - 1][y] != g->board[x + 1][y])){
    g->fields_next_to[g->board[x + 1][y] - 1] -= sign;
  }
  if(y != 0 && g->board[x][y - 1] != 0
  && (x == 0 || g->board[x - 1][y] != g->board[x][y - 1])
  && (x == g->width - 1 || g->board[x + 1][y] != g->board[x][y - 1])){
    g->fields_next_to[g->board[x][y - 1] - 1] -= sign;
  }
  if(y != g->height - 1 && g->board[x][y + 1] != 0
  && (x == 0 || g->board[x - 1][y] != g->board[x][y + 1])
  && (x == g->width - 1 || g->board[x + 1][y] != g->board[x][y + 1])
  && (y == 0 || g->board[x][y - 1] != g->board[x][y + 1])){
    (g->fields_next_to)[g->board[x][y + 1] - 1] -= sign;
  }
}

/* @brief Zlicza ile pól dookoła [x][y] należy do gracza @p player
* @param[in] g       – wskaźnik na strukturę gamma_t,
* @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
*                      @p players z funkcji @ref gamma_new,
* @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
*                      @p width z funkcji @ref gamma_new,
* @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
*                      @p height z funkcji @ref gamma_new.
*                      zostanie zdjęty.
* @return Licza sąsiednich pól zajętych przez gracza @p player
*/
uint32_t get_various_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y){
  uint32_t to_return = 0;
  pair_t* xy_0_0 = fu_find(g->parent, &(g->parent[x][y]));
  if(x != 0 && g->board[x - 1][y] == player){
    pair_t* xy_m_0 = fu_find(g->parent, &(g->parent[x - 1][y]));
    if(equal(*xy_0_0, *xy_m_0) == false) to_return++;
    fu_union(g->parent, g->rank, xy_0_0, xy_m_0);
  }
  if(x != g->width - 1 && g->board[x + 1][y] == player){
    pair_t* xy_p_0 = fu_find(g->parent, &(g->parent[x + 1][y]));
    if(equal(*xy_0_0, *xy_p_0) == false) to_return++;
    fu_union(g->parent, g->rank, xy_0_0, xy_p_0);
  }
  if(y != 0 && g->board[x][y - 1] == player){
    pair_t* xy_0_m = fu_find(g->parent, &(g->parent[x][y - 1]));
    if(equal(*xy_0_0, *xy_0_m) == false) to_return++;
    fu_union(g->parent, g->rank, xy_0_0, xy_0_m);
  }
  if(y != g->height - 1 && g->board[x][y + 1] == player){
    pair_t* xy_0_p = fu_find(g->parent, &(g->parent[x][y + 1]));
    if(equal(*xy_0_0, *xy_0_p) == false) to_return++;
    fu_union(g->parent, g->rank, xy_0_0, xy_0_p);
  }
  return to_return;
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y){
  if(g == NULL
  || (player == 0 || player > g->players)
  || x > g->width - 1
  || y > g->height - 1
  || g->board[x][y] != 0){
    return false;
  }

  uint32_t various_areas = get_various_areas(g, player, x, y);
  //nie zwiększa się liczba obszarów zajętych przez gracza player
  if(various_areas > 0){
    g->board[x][y] = player;
    update_around_fields_next_to(g, x, y, true);
    update_this_fields_next_to(g, x, y, true);
    g->busy_fields[player - 1]++;
    g->busy_fields_all++;
    g->used_areas[player - 1] -= (various_areas - 1);
    low_set_not_up_to_date(g, x, y);
    return true;
  }
  //zwiększa się liczba obszarów zajętych przez gracza player
  else{
    if(g->used_areas[player - 1] == g->areas){
      return false;
    }
    g->board[x][y] = player;
    update_around_fields_next_to(g, x, y, true);
    update_this_fields_next_to(g, x, y, true);
    g->busy_fields[player - 1]++;
    g->busy_fields_all++;
    g->used_areas[player - 1]++;
    g->parent[x][y] = make_pair(x, y);
    low_set_not_up_to_date(g, x, y);
    return true;
  }
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player){
  if(g == NULL
  || (1 > player || player > g->players)){
    return 0;
  }
  return g->busy_fields[player - 1];
}

/* @brief Odtwarza strukturę find and union dla danej obszaru
 * Dla każdego pola należącego do obszaru:
 * – ustawia wartość w tablicy ojcostwa @p parent na @p ancestor.
 * – poprawia tablicę rang @p rank.
 * @param[in] g         – wskaźnik na strukturę gamma_t,
 * @param[in] x         – współrzędna x-owa pola,
 * @param[in] y         – wysokość y-owa pola,
 * @param[in] ancestor  – nowy reprezentant spójnej,
 *                        do której należy pole [x][y]
 */
void gamma_fu_recreate(gamma_t *g, uint32_t x, uint32_t y, pair_t ancestor){
  if(!equal(make_pair(x, y), ancestor)) g->rank[x][y] = 0;
  if(!equal((g->parent[x][y]), ancestor)){
    g->parent[x][y] = ancestor;
    if(x != 0 && g->board[x][y] == g->board[x-1][y])
      gamma_fu_recreate(g, x-1, y, ancestor);
    if(x != g->width - 1 && g->board[x][y] == g->board[x+1][y])
      gamma_fu_recreate(g, x+1, y, ancestor);
    if(y != 0 && g->board[x][y] == g->board[x][y-1])
      gamma_fu_recreate(g, x, y-1, ancestor);
    if(y != g->height - 1 && g->board[x][y] == g->board[x][y+1])
      gamma_fu_recreate(g, x, y+1, ancestor);
  }
}


/* @brief Liczy ile nowych obszarów powstanie po usunięciu pola [x][y]
 * @param[in] g                 – wskaźnik na strukturę gamma_t,
 * @param[in] x                 – współrzędna x-owa pola,
 * @param[in] y                 – wysokość y-owa pola,
 * @param[in] previous_player   – gracz, którego pionek został usunięty.
 * @return Liczba obszarów, które powstaną.
 */

pair_t number_of_neighbours_and_bridges(gamma_t *g, uint32_t x, uint32_t y){
  uint32_t neighbours = 0, bridges = 0;
  if(x != 0 && g->board[x - 1][y] == g->board[x][y]){
    neighbours++;
    if(low_bridge(g, x, y, x - 1, y)) bridges++;
  }
  if(x != g->width - 1 && g->board[x + 1][y] == g->board[x][y]){
    neighbours++;
    if(low_bridge(g, x, y, x + 1, y)) bridges++;
  }
  if(y != 0 && g->board[x][y - 1] == g->board[x][y]){
    neighbours++;
    if(low_bridge(g, x, y, x, y - 1)) bridges++;
  }
  if(y != g->height - 1 && g->board[x][y + 1] == g->board[x][y]){
    neighbours++;
    if(low_bridge(g, x, y, x, y + 1)) bridges++;
  }
  return make_pair(neighbours, bridges);
}

uint64_t number_of_new_areas(gamma_t *g, uint32_t x, uint32_t y){
  uint64_t visit_time = 0;
  low_clear(g, x, y);
  low_compute(g, x, y, &visit_time);
  pair_t neighbours_and_bridges = number_of_neighbours_and_bridges(g, x, y);
  uint32_t neighbours = neighbours_and_bridges.st;
  uint32_t bridges = neighbours_and_bridges.nd;
  if(neighbours == bridges){
    return bridges;
  }else{
    if(neighbours == 4 && bridges == 0){
      if(low_articulation_point(g, x, y)) return 2;
      else return 1;
    }else{
      return bridges + 1;
    }
  }
}

void gamma_fu_recreate_neighbours(gamma_t *g, uint32_t x, uint32_t y,
                                  uint32_t previous_player){
  if(x != 0 && g->board[x - 1][y] == previous_player){
    gamma_fu_recreate(g, x - 1, y, make_pair(-1, -1));
    gamma_fu_recreate(g, x - 1, y, make_pair(x - 1, y));
    low_set_not_up_to_date(g, x - 1, y);
  }
  if(x != g->width - 1 && g->board[x + 1][y] == previous_player){
    gamma_fu_recreate(g, x + 1, y, make_pair(-1, -1));
    gamma_fu_recreate(g, x + 1, y, make_pair(x + 1, y));
    low_set_not_up_to_date(g, x + 1, y);
  }
  if(y != 0 && g->board[x][y - 1] == previous_player){
    gamma_fu_recreate(g, x, y - 1, make_pair(-1, -1));
    gamma_fu_recreate(g, x, y - 1, make_pair(x, y - 1));
    low_set_not_up_to_date(g, x, y - 1);
  }
  if(y != g->height - 1 && g->board[x][y + 1] == previous_player){
    gamma_fu_recreate(g, x, y + 1, make_pair(-1, -1));
    gamma_fu_recreate(g, x, y + 1, make_pair(x, y + 1));
    low_set_not_up_to_date(g, x, y + 1);
  }
}

bool gamma_golden_move_check(gamma_t *g, uint32_t player, uint32_t previous_player,
                             uint32_t x, uint32_t y, int64_t new_areas){
  if(g->used_areas[player - 1] == g->areas){
    bool next_to_area = false;
    if(x != 0 && g->board[x - 1][y] == player) next_to_area = true;
    if(x != g->width - 1 && g->board[x + 1][y] == player) next_to_area = true;
    if(y != 0 && g->board[x][y - 1] == player) next_to_area = true;
    if(y != g->height - 1 && g->board[x][y + 1] == player) next_to_area = true;
    if(!next_to_area) return false;
  }
  if(g->used_areas[previous_player - 1] + new_areas > g->areas) return false;

  return true;
}

bool gamma_golden_possible(gamma_t *g, uint32_t player){
  if(g == NULL
  || 1 > player || player > g->players){
    return false;
  }
  if(g->busy_fields_all == g->busy_fields[player - 1]
  || g->is_golden_used[player - 1] == true){
    return false;
  }

  low_update_all(g);
  uint32_t previous_player;
  int64_t new_areas;
  for(uint32_t x = 0; x < g->width; x++){
    for(uint32_t y = 0; y < g->height; y++){
      if(g->board[x][y] == 0) continue;
      if(g->board[x][y] == player) continue;
      previous_player = g->board[x][y];
      new_areas = number_of_new_areas(g, x, y) - 1;
      if(gamma_golden_move_check(g, player, previous_player, x, y, new_areas))
        return true;
    }
  }
  return false;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player){
  if(g == NULL
  || (1 > player || player > g->players)){
    return 0;
  }

  if(g->used_areas[player - 1] != g->areas){
    return ((uint64_t)g->height) * ((uint64_t)g->width) - g->busy_fields_all;
  }
  return g->fields_next_to[player - 1];
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y){
  if(g == NULL
  || x > g->width - 1
  || y > g->height - 1
  || g->board[x][y] == 0
  || g->board[x][y] == player
  || (player == 0 || player > g->players)
  || g->is_golden_used[player - 1] == true){
    return false;
  }
  uint32_t previous_player = g->board[x][y];
  int64_t new_areas = number_of_new_areas(g, x, y) - 1;

  if(!gamma_golden_move_check(g, player, previous_player, x, y, new_areas)){
    return false;
  }
  update_this_fields_next_to(g, x, y, false);
  update_around_fields_next_to(g, x, y, false);
  g->board[x][y] = 0;

  gamma_fu_recreate_neighbours(g, x, y, previous_player);
  g->parent[x][y] = make_pair(x, y);
  gamma_move(g, player, x, y);

  g->used_areas[previous_player - 1] += new_areas;
  g->is_golden_used[player - 1] = true;
  g->busy_fields[previous_player - 1]--;
  g->busy_fields_all--;
  return true;
}

bool add_letter(uint64_t *index, uint64_t *size, char* *board, char l){
  if(*index >= *size - 1){
    *size *= 2;
    *board = realloc((*board), (*size)*sizeof(char));
    if(board == NULL){
      free(board);
      return false;
    }
  }
  (*board)[*index] = l;
  (*board)[*index + 1] = '\0';
  (*index)++;
  return true;
}

char* gamma_board(gamma_t *g){
  if(g == NULL) return NULL;
  uint64_t size = (g->width + 1) * (g->height + 1);
  char* board = malloc(size * sizeof(char));
  if(board == NULL) return NULL;
  uint64_t index = 0;

  for(uint32_t y = g->height - 1; y < UINT32_MAX; y--){
    for(uint32_t x = 0; x < g->width; x++){
      if(g->board[x][y] == 0) {
        if(!add_letter(&index, &size, &board, '.'))return NULL;
      }else{
        if(g->board[x][y] > 9 && !add_letter(&index, &size, &board, ' '))
          return false;
        for(uint32_t d = 1000 * 1000 * 1000; d > 0; d /= 10 ){
          if(d > g->board[x][y]) continue;
          if(!add_letter(&index, &size, &board, '0' + ((g->board[x][y])/d)%10))
            return NULL;
        }
        if(g->board[x][y] > 9 && !add_letter(&index, &size, &board, ' '))
          return NULL;
      }
    }
    if(!add_letter(&index, &size, &board, '\n')) return NULL;
  }
  if(!add_letter(&index, &size, &board, '\0')) return NULL;
  if(index < size) board = realloc(board, index*sizeof(char));
  return board;
}
