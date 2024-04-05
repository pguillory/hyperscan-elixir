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

char * null_terminate(ErlNifBinary bin) {
  char * string = malloc(bin.size + 1);
  memcpy(string, bin.data, bin.size);
  string[bin.size] = 0;
  return string;
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
  hs_free_database(database_resource->db);
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
// scratch_resource
//******************************************************************************

struct scratch_resource {
  hs_scratch_t * scratch;
};

ErlNifResourceType * scratch_resource_type;

void free_scratch_resource(ErlNifEnv * env, void * obj) {
  struct scratch_resource * scratch_resource = (struct scratch_resource *) obj;
  hs_free_scratch(scratch_resource->scratch);
  scratch_resource->scratch = NULL;
}

int open_scratch_resource_type(ErlNifEnv * env) {
  ErlNifResourceFlags tried;
  scratch_resource_type = enif_open_resource_type(env, NULL, "scratch", free_scratch_resource, ERL_NIF_RT_CREATE, &tried);
  return scratch_resource_type != NULL;
}

ERL_NIF_TERM make_scratch_resource(ErlNifEnv * env, hs_scratch_t * scratch) {
  struct scratch_resource * scratch_resource = enif_alloc_resource(scratch_resource_type, sizeof(struct scratch_resource));
  scratch_resource->scratch = scratch;
  return enif_make_resource(env, scratch_resource);
}

int get_scratch_resource(ErlNifEnv * env, ERL_NIF_TERM arg, hs_scratch_t ** scratch) {
  struct scratch_resource * scratch_resource;
  if (!enif_get_resource(env, arg, scratch_resource_type, (void **) &scratch_resource)) {
    return 0;
  }
  *scratch = scratch_resource->scratch;
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

ERL_NIF_TERM compile_error_to_term(ErlNifEnv * env, hs_compile_error_t * compile_error) {
  ERL_NIF_TERM message = make_binary_const(env, compile_error->message);
  ERL_NIF_TERM expression_id = enif_make_int(env, compile_error->expression);
  hs_error_t error = hs_free_compile_error(compile_error);

  switch (error) {
  case HS_SUCCESS:
    break;

  default:
    printf("hs_free_compile_error failed with %s\r\n", error_name(error));
    break;
  }

  return enif_make_tuple2(env, error_atom, enif_make_tuple2(env, message, expression_id));
}

static ERL_NIF_TERM compile_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary expression_bin;
  unsigned int flags;
  unsigned int mode;
  hs_platform_info_t * maybe_platform_info;

  if (argc != 4 ||
      !enif_inspect_binary(env, argv[0], &expression_bin) ||
      !enif_get_uint(env, argv[1], &flags) ||
      !enif_get_uint(env, argv[2], &mode) ||
      !maybe_get_platform_info_resource(env, argv[3], &maybe_platform_info)) {
    return enif_make_badarg(env);
  }

  // Expression must be null terminated.
  char * expression = malloc(expression_bin.size + 1);
  memcpy(expression, expression_bin.data, expression_bin.size);
  expression[expression_bin.size] = 0;

  // printf("expression: %s\r\n", expression);
  // printf("flags: %i\r\n", flags);
  // printf("mode: %i\r\n", mode);
  // printf("platform_info: %p\r\n", platform_info);

  hs_database_t * db;
  hs_compile_error_t * compile_error;
  hs_error_t error = hs_compile(expression, flags, mode, maybe_platform_info, &db, &compile_error);
  free(expression);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, make_database_resource(env, db));

  case HS_COMPILER_ERROR:
    return compile_error_to_term(env, compile_error);

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM compile_multi_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM result;

  unsigned int num_expressions;
  unsigned int num_flags;
  unsigned int num_ids;
  unsigned int mode;
  hs_platform_info_t * maybe_platform_info;

  if (argc != 5 ||
      !enif_get_list_length(env, argv[0], &num_expressions) ||
      !enif_get_list_length(env, argv[1], &num_flags) ||
      !enif_get_list_length(env, argv[2], &num_ids) ||
      num_expressions != num_flags ||
      num_expressions != num_ids ||
      !enif_get_uint(env, argv[3], &mode) ||
      !maybe_get_platform_info_resource(env, argv[4], &maybe_platform_info)) {
    result = enif_make_badarg(env);
    goto compile_multi_nif_return;
  }

  ErlNifBinary expression_bin;
  char ** expression_array = calloc(num_expressions, sizeof(* expression_array));
  unsigned int * flags_array = calloc(num_flags, sizeof(* flags_array));
  unsigned int * id_array = calloc(num_ids, sizeof(* id_array));

  ERL_NIF_TERM expression_head, expression_tail = argv[0];
  ERL_NIF_TERM flags_head, flags_tail = argv[1];
  ERL_NIF_TERM ids_head, ids_tail = argv[2];

  for (int i = 0; i < num_expressions; i++) {
    if (!enif_get_list_cell(env, expression_tail, &expression_head, &expression_tail) ||
        !enif_get_list_cell(env, flags_tail, &flags_head, &flags_tail) ||
        !enif_get_list_cell(env, ids_tail, &ids_head, &ids_tail) ||
        !enif_inspect_binary(env, expression_head, &expression_bin) ||
        !enif_get_uint(env, flags_head, &flags_array[i]) ||
        !enif_get_uint(env, ids_head, &id_array[i])) {
      result = enif_make_badarg(env);
      goto compile_multi_nif_free_and_return;
    }
    expression_array[i] = null_terminate(expression_bin);
  }

  // Sanity check.
  if (!enif_is_empty_list(env, expression_tail) ||
      !enif_is_empty_list(env, flags_tail) ||
      !enif_is_empty_list(env, ids_tail)) {
    result = enif_make_badarg(env);
    goto compile_multi_nif_free_and_return;
  }

  hs_database_t * db;
  hs_compile_error_t * compile_error;
  hs_error_t error = hs_compile_multi((const char *const *) expression_array, flags_array, id_array, num_expressions, mode, maybe_platform_info, &db, &compile_error);

  switch (error) {
  case HS_SUCCESS:
    result = enif_make_tuple2(env, ok_atom, make_database_resource(env, db));
    break;

  case HS_COMPILER_ERROR:
    result = compile_error_to_term(env, compile_error);
    break;

  default:
    result = enif_make_tuple2(env, error_atom, error_name_atom(env, error));
    break;
  }

compile_multi_nif_free_and_return:
  for (int i = 0; i < num_expressions; i++) {
    if (expression_array[i])
      free(expression_array[i]);
  }
  free(expression_array);
  free(flags_array);
  free(id_array);

compile_multi_nif_return:
  return result;
}

ERL_NIF_TERM expr_info_to_map(ErlNifEnv * env, hs_expr_info_t * expr_info) {
  ERL_NIF_TERM keys[5] = {
    enif_make_atom(env, "min_width"),
    enif_make_atom(env, "max_width"),
    enif_make_atom(env, "unordered_matches"),
    enif_make_atom(env, "matches_at_eod"),
    enif_make_atom(env, "matches_only_at_eod"),
  };

  ERL_NIF_TERM values[5] = {
    enif_make_uint(env, expr_info->min_width),
    enif_make_uint(env, expr_info->max_width),
    expr_info->unordered_matches ? true_atom : false_atom,
    expr_info->matches_at_eod ? true_atom : false_atom,
    expr_info->matches_only_at_eod ? true_atom : false_atom,
  };

  ERL_NIF_TERM result;

  if (!enif_make_map_from_arrays(env, keys, values, 5, &result)) {
    return enif_make_badarg(env);
  }

  return result;
}

static ERL_NIF_TERM expression_info_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary expression_bin;
  unsigned int flags;

  if (argc != 2 ||
      !enif_inspect_binary(env, argv[0], &expression_bin) ||
      !enif_get_uint(env, argv[1], &flags)) {
    return enif_make_badarg(env);
  }

  // Expression must be null terminated.
  char * expression = null_terminate(expression_bin);

  hs_expr_info_t * expr_info;
  hs_compile_error_t * compile_error;
  hs_error_t error = hs_expression_info(expression, flags, &expr_info, &compile_error);
  free(expression);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, expr_info_to_map(env, expr_info));

  case HS_COMPILER_ERROR:
    return compile_error_to_term(env, compile_error);

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM database_info_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;

  if (argc != 1 ||
      !get_database_resource(env, argv[0], &db)) {
    return enif_make_badarg(env);
  }

  char * info;
  hs_error_t error = hs_database_info(db, &info);

  switch (error) {
  case HS_SUCCESS:
    break;

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }

  ERL_NIF_TERM result = enif_make_tuple2(env, ok_atom, make_binary_const(env, info));
  free(info);
  return result;
}

static ERL_NIF_TERM database_size_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;

  if (argc != 1 ||
      !get_database_resource(env, argv[0], &db)) {
    return enif_make_badarg(env);
  }

  size_t database_size;
  hs_error_t error = hs_database_size(db, &database_size);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, enif_make_uint64(env, database_size));

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM serialize_database_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;

  if (argc != 1 ||
      !get_database_resource(env, argv[0], &db)) {
    return enif_make_badarg(env);
  }

  char * data;
  size_t size;
  hs_error_t error = hs_serialize_database(db, &data, &size);
  ERL_NIF_TERM result;
  unsigned char * data2 = enif_make_new_binary(env, size, &result);
  memcpy(data2, data, size);
  free(data);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, result);

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM deserialize_database_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary binary;

  if (argc != 1 ||
      !enif_inspect_binary(env, argv[0], &binary)) {
    return enif_make_badarg(env);
  }

  hs_database_t * db;
  hs_error_t error = hs_deserialize_database((char *) binary.data, binary.size, &db);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, make_database_resource(env, db));

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM alloc_scratch_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;

  if (argc != 1 ||
      !get_database_resource(env, argv[0], &db)) {
    return enif_make_badarg(env);
  }

  hs_scratch_t * scratch = NULL;

  hs_error_t error = hs_alloc_scratch(db, &scratch);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, make_scratch_resource(env, scratch));

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM realloc_scratch_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;
  hs_scratch_t * scratch;

  if (argc != 2 ||
      !get_database_resource(env, argv[0], &db) ||
      !get_scratch_resource(env, argv[1], &scratch)) {
    return enif_make_badarg(env);
  }

  hs_error_t error = hs_alloc_scratch(db, &scratch);

  switch (error) {
  case HS_SUCCESS:
    return ok_atom;

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM clone_scratch_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_scratch_t * scratch;

  if (argc != 1 ||
      !get_scratch_resource(env, argv[0], &scratch)) {
    return enif_make_badarg(env);
  }

  hs_scratch_t * scratch2;
  hs_error_t error = hs_clone_scratch(scratch, &scratch2);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, make_scratch_resource(env, scratch2));

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

static ERL_NIF_TERM scratch_size_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_scratch_t * scratch;

  if (argc != 1 ||
      !get_scratch_resource(env, argv[0], &scratch)) {
    return enif_make_badarg(env);
  }

  size_t scratch_size;
  hs_error_t error = hs_scratch_size(scratch, &scratch_size);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, enif_make_uint64(env, scratch_size));

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

int match_callback(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *context) {
  return 1;
}

static ERL_NIF_TERM match_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;
  ErlNifBinary string;
  hs_scratch_t * scratch;

  if (argc != 3 ||
      !get_database_resource(env, argv[0], &db) ||
      !enif_inspect_binary(env, argv[1], &string) ||
      !get_scratch_resource(env, argv[2], &scratch)) {
    return enif_make_badarg(env);
  }

  int flags = 0;
  void * context = NULL;
  hs_error_t error = hs_scan(db, (char *) string.data, string.size, flags, scratch, match_callback, context);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, false_atom);

  case HS_SCAN_TERMINATED:
    return enif_make_tuple2(env, ok_atom, true_atom);

  default:
    return enif_make_tuple2(env, error_atom, error_name_atom(env, error));
  }
}

struct match_multi_context {
  ErlNifEnv * env;
  ERL_NIF_TERM result;
};

int match_multi_callback(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void * void_context) {
  struct match_multi_context * context = (struct match_multi_context *) void_context;
  ERL_NIF_TERM id_term = enif_make_uint(context->env, id);
  context->result = enif_make_list_cell(context->env, id_term, context->result);
  return 0;
}

static ERL_NIF_TERM match_multi_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  hs_database_t * db;
  ErlNifBinary string;
  hs_scratch_t * scratch;

  if (argc != 3 ||
      !get_database_resource(env, argv[0], &db) ||
      !enif_inspect_binary(env, argv[1], &string) ||
      !get_scratch_resource(env, argv[2], &scratch)) {
    return enif_make_badarg(env);
  }

  struct match_multi_context context;
  context.env = env;
  context.result = enif_make_list(env, 0);
  void * void_context = &context;

  int flags = 0;
  hs_error_t error = hs_scan(db, (char *) string.data, string.size, flags, scratch, match_multi_callback, void_context);

  switch (error) {
  case HS_SUCCESS:
    return enif_make_tuple2(env, ok_atom, context.result);

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
  {"compile_multi", 5, compile_multi_nif},
  {"expression_info", 2, expression_info_nif},
  {"database_info", 1, database_info_nif},
  {"database_size", 1, database_size_nif},
  {"serialize_database", 1, serialize_database_nif},
  {"deserialize_database", 1, deserialize_database_nif},
  {"alloc_scratch", 1, alloc_scratch_nif},
  {"realloc_scratch", 2, realloc_scratch_nif},
  {"clone_scratch", 1, clone_scratch_nif},
  {"scratch_size", 1, scratch_size_nif},
  {"match", 3, match_nif},
  {"match_multi", 3, match_multi_nif},
};

int load(ErlNifEnv * env, void ** priv_data, ERL_NIF_TERM load_info) {
  init_atoms(env);

  if (!open_platform_info_resource_type(env) ||
      !open_database_resource_type(env) ||
      !open_scratch_resource_type(env)) {
    return 1;
  }

  return 0;
}

ERL_NIF_INIT(Elixir.Hyperscan, nif_funcs, load, NULL, NULL, NULL)
