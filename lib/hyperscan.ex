defmodule Hyperscan do
  @on_load :init

  @doc false
  def init do
    priv_dir = :code.priv_dir(:hyperscan)
    nif_path = Path.join(priv_dir, "hyperscan")
    :ok = :erlang.load_nif(nif_path, 0)
  end

  @doc false
  def populate_platform(), do: exit(:nif_not_loaded)

  @doc false
  def platform_info_to_map(_platform_info), do: exit(:nif_not_loaded)

  @doc """
  Check if the current system architecture supports Hyperscan.
  """
  def valid_platform(), do: exit(:nif_not_loaded)

  @doc """
  Returns the compiled Hyperscan version as a string.
  """
  def version(), do: exit(:nif_not_loaded)

  @doc """
  Look up a flag constant by name.

  Returns the integer value of the Hyperscan flag constant with the given
  name. Names follow the pattern `HS_FLAG_*`. See the [docs] for available flag
  names.

  [docs]: https://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_CASELESS

  The value is used by the compile and compile_multi functions. To use
  multiple flags simultaneously, combine their values with Bitwise.bor/2.

  Raises :badarg if it is not a valid flag name.

  # Example

      iex> Hyperscan.flag("HS_FLAG_CASELESS")
      1
  """
  def flag(_name), do: exit(:nif_not_loaded)

  @doc """
  Look up a mode constant by name.

  Returns the integer value of the Hyperscan mode constant with the given
  name. Names follow the pattern `HS_MODE_*`. See the [docs] for available mode
  names.

  [docs]: https://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_MODE_BLOCK

  The value is used by the compile and compile_multi functions. To use
  multiple modes simultaneously, combine their values with Bitwise.bor/2.

  Raises :badarg if it is not a valid mode name.

  # Example

      iex> Hyperscan.mode("HS_MODE_BLOCK")
      1
  """
  def mode(_name), do: exit(:nif_not_loaded)

  @doc """
  Compile a regular expression into a database.

  - `expression` is a regular expression, without the leading and trailing `/` characters or flags.
  - `flags` is an integer. Use 0 for no flags, or bitwise-or together one or more results of the flag/1 function.
  - `mode` is an integer similar to `flags`.

  Returns `{:ok, db}` where db represents the compiled regex, or `{:error, reason}` on error.

  See the [docs] for more information:

  [docs]: http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile

  # Example

      iex> flags = Hyperscan.flag("HS_FLAG_CASELESS")
      iex> mode = Hyperscan.mode("HS_MODE_BLOCK")
      iex> {:ok, _db} = Hyperscan.compile("foo", flags, mode)
  """
  def compile(expression, flags, mode) do
    platform = nil
    compile(expression, flags, mode, platform)
  end

  @doc false
  def compile(_expression, _flags, _mode, _platform), do: exit(:nif_not_loaded)

  @doc """
  Compile multiple regular expressions into a database.

  - `expression_list` is a list of regular expressions, without the leading
    and trailing `/` characters or flags.
  - `flags_list` is a list of flags, one for each expression in
    `expression_list`. To use multiple flags for a single expression,
    bitwise-or them togther. Zero means no flags.
  - `id_list` is a list of arbitrary integers, with one value for each
    expression in `expression_list`. An ID will be returned from
    match_multi/3 if its corresponding expression matches the input string.
  - `mode` is an integer returned by mode/1.

  See the [docs] for more information:

  [docs]: http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_multi

  # Example

      iex> flags = Hyperscan.flag("HS_FLAG_CASELESS")
      iex> mode = Hyperscan.mode("HS_MODE_BLOCK")
      iex> {:ok, db} = Hyperscan.compile_multi(["foo", "bar"], [flags, flags], [1, 2], mode)
      iex> {:ok, scratch} = Hyperscan.alloc_scratch(db)
      iex> Hyperscan.match_multi(db, "Foo", scratch)
      {:ok, [1]}
      iex> Hyperscan.match_multi(db, "Bar", scratch)
      {:ok, [2]}
      iex> Hyperscan.match_multi(db, "Baz", scratch)
      {:ok, []}
  """
  def compile_multi(expression_list, flags_list, id_list, mode) do
    platform = nil
    compile_multi(expression_list, flags_list, id_list, mode, platform)
  end

  @doc false
  def compile_multi(_expression_list, _flags_list, _id_list, _mode, _platform), do: exit(:nif_not_loaded)

  @doc """
  Returns a map with debugging info about a regular expression.
  """
  def expression_info(_expression, _flags), do: exit(:nif_not_loaded)

  @doc false
  def database_info(_db), do: exit(:nif_not_loaded)

  @doc false
  def database_size(_db), do: exit(:nif_not_loaded)

  @doc false
  def serialize_database(_db), do: exit(:nif_not_loaded)

  @doc false
  def deserialize_database(_binary), do: exit(:nif_not_loaded)

  @doc """
  Allocate a scratch memory buffer for running the regex.
  """
  def alloc_scratch(_db), do: exit(:nif_not_loaded)

  @doc """
  Re-allocate a scratch memory buffer.

  If the scratch was allocated for a different database and is too small for
  the given one, it will be reallocated. Either way, the scratch is now safe
  to use for the given database. Returns :ok.
  """
  def realloc_scratch(_db, _scratch), do: exit(:nif_not_loaded)

  @doc """
  Create a new scratch buffer of the same size.
  """
  def clone_scratch(_scratch), do: exit(:nif_not_loaded)

  @doc """
  Check the size of a scratch buffer.
  """
  def scratch_size(_scratch), do: exit(:nif_not_loaded)

  @doc """
  Test whether the given compiled regex matches a string.

  Requires a scratch buffer allocated by alloc_scratch/1.
  """
  def match(_db, _string, _scratch), do: exit(:nif_not_loaded)

  @doc """
  Test whether multiple compiled regexes match a string.

  The regex should be compiled by compile_multi. Returns a list of expression
  IDs that matched the string. Note that the order of the returned IDs is
  undefined.

  See the docs for compile_multi/4 for more info.
  """
  def match_multi(_db, _string, _scratch), do: exit(:nif_not_loaded)
end
