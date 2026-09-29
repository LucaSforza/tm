#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rax.h>

// assume that every alphabet have as character 'blank' equal to '\x00'
typedef struct {
  char *characters;
  size_t cap;
} Alphabet;

#define ALPHABET_MAX_LENGHT (1024)

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

Alphabet alph_from_cstr(const char *cstr) {
  size_t cap = strnlen(cstr, ALPHABET_MAX_LENGHT);
  char *buf = malloc(cap + 1);
  memset(buf, 0, cap + 1);
  strcpy(buf, cstr);
  return (Alphabet){.characters = buf, .cap = cap};
}

typedef size_t StateId;

// 0 is the starting point of the turing machine. 1 is the halt
// state. "q_0" is the string of the inizial state. "q_halt" is the string of
// the final state
typedef struct {
  rax *name_to_id; // "q_accept" -> StateId

  const char **id_to_name; // StateId -> "q_accept"
  size_t num_states;
  size_t max_states;
} StateTable;

static inline bool st_is_valid(StateTable *_this) {
  return _this != NULL && _this->id_to_name != NULL;
}

void st_add_state(StateTable *_this, const char *state_name) {
  if (!st_is_valid(_this)) {
    eprintf("State table not fully initialized");
    exit(1);
  }
  if (_this->num_states >= _this->max_states) {
    eprintf("ERROR: out of states");
    exit(1);
  }

  size_t new_state_id = _this->num_states++;
  void *data = malloc(sizeof(size_t));
  memcpy(data, &new_state_id, sizeof(size_t));

  int error = raxInsert(_this->name_to_id, state_name,
                        strnlen(state_name, ALPHABET_MAX_LENGHT), data, NULL);
  if (error != 0) {
    eprintf("ERROR: internal error of rax tree\n");
    exit(1);
  }
  _this->id_to_name[new_state_id] = state_name;
}

StateTable new_state_table(size_t max_states) {
  if (max_states < 1) {
    eprintf("ERROR: cannot have < 1 states\n");
    exit(1);
  }

  StateTable result = {0};

  result.max_states = max_states;

  result.name_to_id = raxNew();

  result.id_to_name = malloc(sizeof(const char *) * max_states);

  memset(result.id_to_name, 0, max_states);

  st_add_state(&result, "q_0");

  st_add_state(&result, "q_halt");

  return result;
}

const char *st_get_state_from_id(StateTable *_this, StateId id) {
  if (!st_is_valid(_this)) {
    eprintf("State table not fully initialized\n");
    exit(1);
  }

  if (id > _this->max_states) {
    eprintf("StateId out of range\n");
    exit(1);
  }

  return _this->id_to_name[id];
}

StateId st_get_id_from_state(StateTable *_this, const char *name) {
  if (!st_is_valid(_this)) {
    eprintf("State table not fully initialized\n");
    exit(1);
  }
  return *(StateId *)raxFind(_this->name_to_id, name, strlen(name));
}

int main(void) {
  Alphabet a = alph_from_cstr("abcdef");
  printf("%s\n", a.characters);
}
