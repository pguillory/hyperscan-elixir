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

//******************************************************************************
// platform_info_resource
//******************************************************************************

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
  platform_info_resource_type = enif_open_resource_type(env, NULL, "platform_info", free_platform_info_resource, ERL_NIF_RT_CREATE, &tried);
  return platform_info_resource_type != NULL;
}

ERL_NIF_TERM make_platform_info_resource(ErlNifEnv * env, hs_platform_info_t * platform_info) {
  struct platform_info_resource * platform_info_resource = enif_alloc_resource(platform_info_resource_type, sizeof(struct platform_info_resource));
  platform_info_resource->platform_info = platform_info;
  return enif_make_resource(env, platform_info_resource);
}

int get_platform_info_resource(ErlNifEnv * env, ERL_NIF_TERM arg, hs_platform_info_t ** platform_info) {
  struct platform_info_resource * platform_info_resource;

  if (!enif_get_resource(env, arg, platform_info_resource_type, (void **) &platform_info_resource)) {
    return 0;
  }
  *platform_info = platform_info_resource->platform_info;
  return 1;
}

int maybe_get_platform_info_resource(ErlNifEnv * env, ERL_NIF_TERM arg, hs_platform_info_t ** platform_info) {
  if (arg == nil_atom) {
    *platform_info = NULL;
    return 1;
  }

  return get_platform_info_resource(env, arg, platform_info);
}

//******************************************************************************
// database_resource
//******************************************************************************

struct database_resource {
  hs_database_t * db;
};

ErlNifResourceType * database_resource_type;

void free_database_resource(ErlNifEnv * env, void * obj) {
  struct database_resource * database_resource = (struct database_resource *) obj;
  free(database_resource->db);
  database_resource->db = NULL;
}

int open_database_resource_type(ErlNifEnv * env) {
  ErlNifResourceFlags tried;
  database_resource_type = enif_open_resource_type(env, NULL, "database", free_database_resource, ERL_NIF_RT_CREATE, &tried);
  return database_resource_type != NULL;
}

ERL_NIF_TERM make_database_resource(ErlNifEnv * env, hs_database_t * db) {
  struct database_resource * database_resource = enif_alloc_resource(database_resource_type, sizeof(struct database_resource));
  database_resource->db = db;
  return enif_make_resource(env, database_resource);
}

int get_database_resource(ErlNifEnv * env, ERL_NIF_TERM arg, hs_database_t ** db) {
  struct database_resource * database_resource;
  if (!enif_get_resource(env, arg, database_resource_type, (void **) &database_resource)) {
    return 0;
  }
  *db = database_resource->db;
  return 1;
}

//******************************************************************************
// NIFs
//******************************************************************************

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
  hs_platform_info_t * platform_info;

  if (argc != 1 ||
      !get_platform_info_resource(env, argv[0], &platform_info)) {
    return enif_make_badarg(env);
  }

  ERL_NIF_TERM keys[4] = {
    enif_make_atom(env, "tune"),
    enif_make_atom(env, "cpu_features"),
    enif_make_atom(env, "reserved1"),
    enif_make_atom(env, "reserved2")
  };

  ERL_NIF_TERM values[4] = {
    enif_make_int(env, platform_info->tune),
    enif_make_int64(env, platform_info->cpu_features),
    enif_make_int64(env, platform_info->reserved1),
    enif_make_int64(env, platform_info->reserved2)
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

int bin_equals_string(ErlNifBinary name_bin, const char * string) {
  return (strlen(string) == name_bin.size)
      && (0 == memcmp(string, name_bin.data, name_bin.size));
}

static ERL_NIF_TERM flag_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary name_bin;

  if (argc != 1 ||
      !enif_inspect_binary(env, argv[0], &name_bin)) {
    return enif_make_badarg(env);
  }

  if (bin_equals_string(name_bin, "HS_FLAG_CASELESS")) return enif_make_int(env, HS_FLAG_CASELESS);
  if (bin_equals_string(name_bin, "HS_FLAG_DOTALL")) return enif_make_int(env, HS_FLAG_DOTALL);
  if (bin_equals_string(name_bin, "HS_FLAG_MULTILINE")) return enif_make_int(env, HS_FLAG_MULTILINE);
  if (bin_equals_string(name_bin, "HS_FLAG_SINGLEMATCH")) return enif_make_int(env, HS_FLAG_SINGLEMATCH);
  if (bin_equals_string(name_bin, "HS_FLAG_ALLOWEMPTY")) return enif_make_int(env, HS_FLAG_ALLOWEMPTY);
  if (bin_equals_string(name_bin, "HS_FLAG_UTF8")) return enif_make_int(env, HS_FLAG_UTF8);
  if (bin_equals_string(name_bin, "HS_FLAG_UCP")) return enif_make_int(env, HS_FLAG_UCP);
  if (bin_equals_string(name_bin, "HS_FLAG_PREFILTER")) return enif_make_int(env, HS_FLAG_PREFILTER);
  if (bin_equals_string(name_bin, "HS_FLAG_SOM_LEFTMOST")) return enif_make_int(env, HS_FLAG_SOM_LEFTMOST);
  if (bin_equals_string(name_bin, "HS_FLAG_COMBINATION")) return enif_make_int(env, HS_FLAG_COMBINATION);
  if (bin_equals_string(name_bin, "HS_FLAG_QUIET")) return enif_make_int(env, HS_FLAG_QUIET);
  return enif_make_badarg(env);
}

static ERL_NIF_TERM mode_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary name_bin;

  if (argc != 1 ||
      !enif_inspect_binary(env, argv[0], &name_bin)) {
    return enif_make_badarg(env);
  }

  if (bin_equals_string(name_bin, "HS_MODE_BLOCK")) return enif_make_int(env, HS_MODE_BLOCK);
  if (bin_equals_string(name_bin, "HS_MODE_NOSTREAM")) return enif_make_int(env, HS_MODE_NOSTREAM);
  if (bin_equals_string(name_bin, "HS_MODE_STREAM")) return enif_make_int(env, HS_MODE_STREAM);
  if (bin_equals_string(name_bin, "HS_MODE_VECTORED")) return enif_make_int(env, HS_MODE_VECTORED);
  if (bin_equals_string(name_bin, "HS_MODE_SOM_HORIZON_LARGE")) return enif_make_int(env, HS_MODE_SOM_HORIZON_LARGE);
  if (bin_equals_string(name_bin, "HS_MODE_SOM_HORIZON_MEDIUM")) return enif_make_int(env, HS_MODE_SOM_HORIZON_MEDIUM);
  if (bin_equals_string(name_bin, "HS_MODE_SOM_HORIZON_SMALL")) return enif_make_int(env, HS_MODE_SOM_HORIZON_SMALL);
  return enif_make_badarg(env);
}

static ERL_NIF_TERM compile_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary expression_bin;
  unsigned int flags;
  unsigned int mode;
  hs_platform_info_t * platform_info;

  if (argc != 4 ||
      !enif_inspect_binary(env, argv[0], &expression_bin) ||
      !enif_get_uint(env, argv[1], &flags) ||
      !enif_get_uint(env, argv[2], &mode) ||
      !maybe_get_platform_info_resource(env, argv[3], &platform_info)) {
    return enif_make_badarg(env);
  }

  if (expression_bin.size == 0) {
    return enif_make_tuple2(env, error_atom, make_binary_const(env, "Empty expression"));
  }

  if (expression_bin.data[expression_bin.size - 1] != 0) {
    return enif_make_tuple2(env, error_atom, make_binary_const(env, "Expression must be null terminated"));
  }

  char * expression = (char *) expression_bin.data;

  // printf("expression: %s\r\n", expression);
  // printf("flags: %i\r\n", flags);
  // printf("mode: %i\r\n", mode);
  // printf("platform_info: %p\r\n", platform_info);

  hs_database_t * db;
  hs_compile_error_t * compile_error;
  hs_error_t error = hs_compile(expression, flags, mode, platform_info, &db, &compile_error);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, make_database_resource(env, db));

  case HS_COMPILER_ERROR:
    return enif_make_tuple2(env, error_atom, enif_make_tuple2(env, make_binary_const(env, compile_error->message), enif_make_int(env, compile_error->expression)));

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ErlNifFunc nif_funcs[] = {
  {"populate_platform", 0, populate_platform_nif},
  {"platform_info_to_map", 1, platform_info_to_map_nif},
  {"valid_platform", 0, valid_platform_nif},
  {"version", 0, version_nif},
  {"flag", 1, flag_nif},
  {"mode", 1, mode_nif},
  {"compile", 4, compile_nif},
};

int load(ErlNifEnv * env, void ** priv_data, ERL_NIF_TERM load_info) {
  init_atoms(env);

  if (!open_platform_info_resource_type(env) ||
      !open_database_resource_type(env)) {
    return 1;
  }

  return 0;
}

ERL_NIF_INIT(Elixir.Vectorscan, nif_funcs, load, NULL, NULL, NULL)
