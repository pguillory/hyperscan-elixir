#include <assert.h>
#include <erl_nif.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

ERL_NIF_TERM ok_atom;
ERL_NIF_TERM nil_atom;
ERL_NIF_TERM false_atom;
ERL_NIF_TERM true_atom;

void init_atoms(ErlNifEnv * env) {
  ok_atom = enif_make_atom(env, "ok");
  nil_atom = enif_make_atom(env, "nil");
  false_atom = enif_make_atom(env, "false");
  true_atom = enif_make_atom(env, "true");
}

static ErlNifFunc nif_funcs[] = {
  // {"new", 1, new_nif},
};

int load(ErlNifEnv * env, void ** priv_data, ERL_NIF_TERM load_info) {
  init_atoms(env);
  return 0;
}

ERL_NIF_INIT(Elixir.Vectorscan.NIF, nif_funcs, load, NULL, NULL, NULL)
