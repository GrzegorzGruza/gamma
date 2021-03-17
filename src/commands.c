/* @file
 *
 * @author Grzegorz Gruza <gg417923@mimuw.edu.pl>
 * @copyright Grzegorz Gruza
 * @date 17.05.2020
 */

#include "commands.h"

bool only_spaces(char* buffer, uint64_t index){
  for(uint64_t i = 0; i < index; i++){
    if(!isspace(buffer[i]))
      return false;
  }
  return true;
}

bool good_letter(char letter){
  if(isspace(letter)) return true;
  if('0' <= letter && letter <= '9') return true;
  return false;
}

/* @brief Sprawdza, czy słowo ma na początk 0
 * @param[in] buffer       – tablica znaków, którą sprawdzamy
 * @return @p true jeśli na początku jest zeru, a @p false w przeciwnym razie.
 */
bool zero_is_first_digit(char* buffer){
  for(uint32_t i = 0; i < strlen(buffer); i++){
    if(isspace(buffer[i])) continue;
    else if(buffer[i] != '0') return false;
    else return true;
  }
  return false;
}

/* @brief Interpretuje tabilcę char jako argumenty typu uint32_t.
 * @param[in] buffer       – tablica znaków, w której szukamy argumentów
 * @param[in] argc         – oczekiwana liczba argumentów
 * @param[in] argv         – tablica, do wypełnienia argumentami
 * @return @p true jeśli buffer da się zinterpretować jako dokładnie
 * argv argumentów typu uint32_t, a @p false w przeciwnym razie.
 */
bool complete_arguments(char* buffer, uint32_t argc, uint32_t* argv){
  if(strlen(buffer) > 1
  && !isspace(buffer[1])){
    return false;
  }
  (buffer)[0] = ' ';

  int64_t arg;
  for(uint32_t i = 0; i < argc; i++){
    if(strlen(buffer) == 0){
      return false;
    }
    if(zero_is_first_digit(buffer)){
      arg = strtoul(buffer, &buffer, 10);
      if(arg < 0 || arg > UINT32_MAX) return false;
    }
    else{
      arg = strtoul(buffer, &buffer, 10);
      if(arg <= 0 || arg > UINT32_MAX) return false;
    }
    argv[i] = (uint32_t)arg;
  }
  if(!only_spaces(buffer, strlen(buffer))) return false;

  return true;
}

void try_command_I(gamma_t **g, char* buffer, uint32_t* argv, uint64_t line){
  if(complete_arguments(buffer, 4, argv)
  && (*g = gamma_new(argv[0], argv[1], argv[2], argv[3])) != NULL){
    printf("OK %lu\n", line);
    if(argv != NULL) free(argv);
    if(buffer != NULL) free(buffer);
    interactive_mode(*g);
  }
  else{
    fprintf(stderr, "ERROR %lu\n", line);
  }
}

void try_command_B(gamma_t **g, char* buffer, uint32_t* argv,
                   uint64_t line, bool *batch_mode){
  if(complete_arguments(buffer, 4, argv)
  && (*g = gamma_new(argv[0], argv[1], argv[2], argv[3])) != NULL){
    printf("OK %lu\n", line);
    *batch_mode = true;
  }
  else{
    fprintf(stderr, "ERROR %lu\n", line);
  }
}

void try_command_m(gamma_t *g, char* buffer, uint32_t* argv, uint64_t line){
  if(complete_arguments(buffer, 3, argv)){
    if(gamma_move(g, argv[0], argv[1], argv[2])) printf("1\n");
    else printf("0\n");
  }
  else{
    fprintf(stderr, "ERROR %lu\n", line);
  }
}

void try_command_g(gamma_t *g, char* buffer, uint32_t* argv, uint64_t line){
  if(complete_arguments(buffer, 3, argv)){
    if(gamma_golden_move(g, argv[0], argv[1], argv[2])) printf("1\n");
    else printf("0\n");
  }
  else{
    fprintf(stderr, "ERROR %lu\n", line);
  }
}

void try_command_b(gamma_t *g, char* buffer, uint32_t* argv, uint64_t line){
  if(complete_arguments(buffer, 1, argv))
    printf("%lu\n", gamma_busy_fields(g, argv[0]));
  else
    fprintf(stderr, "ERROR %lu\n", line);
}

void try_command_f(gamma_t *g, char* buffer, uint32_t* argv, uint64_t line){
  if(complete_arguments(buffer, 1, argv))
    printf("%lu\n", gamma_free_fields(g, argv[0]));
  else
    fprintf(stderr, "ERROR %lu\n", line);
}

void try_command_q(gamma_t *g, char* buffer, uint32_t* argv, uint64_t line){
  if(complete_arguments(buffer, 1, argv))
    printf("%u\n", gamma_golden_possible(g, argv[0]));
  else
    fprintf(stderr, "ERROR %lu\n", line);
}

void try_command_p(gamma_t *g, char* buffer, uint32_t* argv, uint64_t line){
  if(!complete_arguments(buffer, 0, argv)){
    fprintf(stderr, "ERROR %lu\n", line);
    return;
  }
  char* board = gamma_board(g);
  if(g == NULL){
    fprintf(stderr, "ERROR %lu\n", line);
  }
  else{
    printf("%s", board);
    free(board);
  }
}
