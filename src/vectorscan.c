#include <assert.h>
#include <erl_nif.h>
#include <hs.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

ERL_NIF_TERM ok_atom;
ERL_NIF_TERM error_atom;
ERL_NIF_TERM nil_atom;
ERL_NIF_TERM false_atom;
ERL_NIF_TERM true_atom;

void init_atoms(ErlNifEnv * env) {
  ok_atom = enif_make_atom(env, "ok");
  error_atom = enif_make_atom(env, "error");
  nil_atom = enif_make_atom(env, "nil");
  false_atom = enif_make_atom(env, "false");
  true_atom = enif_make_atom(env, "true");
}

ERL_NIF_TERM make_binary_const(ErlNifEnv * env, const char * string) {
  const int size = strlen(string);
  ERL_NIF_TERM result;
  unsigned char * data = enif_make_new_binary(env, size, &result);
  memcpy(data, string, size);
  return result;
}

const char * error_name(hs_error_t error) {
  switch (error) {
  case HS_SUCCESS: return "HS_SUCCESS";
  case HS_INVALID: return "HS_INVALID";
  case HS_NOMEM: return "HS_NOMEM";
  case HS_SCAN_TERMINATED: return "HS_SCAN_TERMINATED";
  case HS_COMPILER_ERROR: return "HS_COMPILER_ERROR";
  case HS_DB_VERSION_ERROR: return "HS_DB_VERSION_ERROR";
  case HS_DB_PLATFORM_ERROR: return "HS_DB_PLATFORM_ERROR";
  case HS_DB_MODE_ERROR: return "HS_DB_MODE_ERROR";
  case HS_BAD_ALIGN: return "HS_BAD_ALIGN";
  case HS_BAD_ALLOC: return "HS_BAD_ALLOC";
  case HS_SCRATCH_IN_USE: return "HS_SCRATCH_IN_USE";
  case HS_ARCH_ERROR: return "HS_ARCH_ERROR";
  case HS_INSUFFICIENT_SPACE: return "HS_INSUFFICIENT_SPACE";
  case HS_UNKNOWN_ERROR: return "HS_UNKNOWN_ERROR";
  default: return "UNKNOWN_ERROR";
  }
}

ERL_NIF_TERM error_name_binary(ErlNifEnv * env, hs_error_t error) {
  const char * name = error_name(error);
  return make_binary_const(env, name);
}

ERL_NIF_TERM error_name_atom(ErlNifEnv * env, hs_error_t error) {
  const char * name = error_name(error);
  return enif_make_atom(env, name);
}

struct platform_info_resource {
  hs_platform_info_t * platform_info;
};

ErlNifResourceType * platform_info_resource_type;

void free_platform_info_resource(ErlNifEnv * env, void * obj) {
  struct platform_info_resource * platform_info_resource = (struct platform_info_resource *) obj;
  free(platform_info_resource->platform_info);
  platform_info_resource->platform_info = NULL;
}

int open_platform_info_resource_type(ErlNifEnv * env) {
  ErlNifResourceFlags tried;
  platform_info_resource_type = enif_open_resource_type(env, NULL, "column", free_platform_info_resource, ERL_NIF_RT_CREATE, &tried);
  return platform_info_resource_type != NULL;
}

ERL_NIF_TERM make_platform_info_resource(ErlNifEnv * env, hs_platform_info_t * platform_info) {
  struct platform_info_resource * platform_info_resource = enif_alloc_resource(platform_info_resource_type, sizeof(struct platform_info_resource));
  printf("make_platform_info_resource %p\r\n", platform_info_resource);
  platform_info_resource->platform_info = platform_info;
  return enif_make_resource(env, platform_info_resource);
}

ERL_NIF_TERM get_platform_info_resource(ErlNifEnv * env, ERL_NIF_TERM arg, struct platform_info_resource ** platform_info_resource) {
  return enif_get_resource(env, arg, platform_info_resource_type, (void **) platform_info_resource);
}

static ERL_NIF_TERM populate_platform_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  if (argc != 0) {
    return enif_make_badarg(env);
  }

  hs_platform_info_t * platform_info = malloc(sizeof(hs_platform_info_t));
  hs_error_t error = hs_populate_platform(platform_info);
  if (error != HS_SUCCESS) {
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
  ERL_NIF_TERM result = make_platform_info_resource(env, platform_info);
  return enif_make_tuple2(env, ok_atom, result);
}

static ERL_NIF_TERM platform_info_to_map_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  struct platform_info_resource * platform_info_resource;

  if (argc != 1 ||
      !get_platform_info_resource(env, argv[0], &platform_info_resource)) {
    return enif_make_badarg(env);
  }

  ERL_NIF_TERM keys[4] = {
    enif_make_atom(env, "tune"),
    enif_make_atom(env, "cpu_features"),
    enif_make_atom(env, "reserved1"),
    enif_make_atom(env, "reserved2")
  };

  ERL_NIF_TERM values[4] = {
    enif_make_int(env, platform_info_resource->platform_info->tune),
    enif_make_int64(env, platform_info_resource->platform_info->cpu_features),
    enif_make_int64(env, platform_info_resource->platform_info->reserved1),
    enif_make_int64(env, platform_info_resource->platform_info->reserved2)
  };

  ERL_NIF_TERM result;

  if (!enif_make_map_from_arrays(env, keys, values, 4, &result)) {
    return enif_make_badarg(env);
  }

  return result;
}

static ERL_NIF_TERM valid_platform_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  if (argc != 0) {
    return enif_make_badarg(env);
  }

  hs_error_t error = hs_valid_platform();

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, true_atom);
  case HS_ARCH_ERROR:
    return enif_make_tuple2(env, ok_atom, false_atom);
  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM version_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  if (argc != 0) {
    return enif_make_badarg(env);
  }

  const char * version = hs_version();
  return make_binary_const(env, version);
}

static ErlNifFunc nif_funcs[] = {
  {"populate_platform", 0, populate_platform_nif},
  {"platform_info_to_map", 1, platform_info_to_map_nif},
  {"valid_platform", 0, valid_platform_nif},
  {"version", 0, version_nif},
};

int load(ErlNifEnv * env, void ** priv_data, ERL_NIF_TERM load_info) {
  init_atoms(env);

  if (!open_platform_info_resource_type(env)) {
    return 1;
  }

  return 0;
}

ERL_NIF_INIT(Elixir.Vectorscan, nif_funcs, load, NULL, NULL, NULL)
