defmodule Hyperscan do
  @on_load :init

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
  Returns true if the current system architecture supports , otherwise false.
  """
  def valid_platform(), do: exit(:nif_not_loaded)

  @doc """
  Returns the compiled version of , as a string.
  """
  def version(), do: exit(:nif_not_loaded)

  @doc """
  Returns the integer value of the  flag constant with the given
  name. Names follow the pattern HS_FLAG_*. The value is used by the compile
  and compile_multi functions. To use multiple flags simultaneously, combine
  their values with Bitwise.bor/2.

  Raises :badarg if it is not a valid flag name.
  """
  def flag(_name), do: exit(:nif_not_loaded)

  @doc """
  Returns the integer value of the  mode constant with the given
  name. Names follow the pattern HS_MODE_*.

  Raises :badarg if it is not a valid mode name.
  """
  def mode(_name), do: exit(:nif_not_loaded)

  @doc """
  Compile a regular expression into a database.

  - `expression` is a regular expression, without the leading/trailing `/` characters or flags.
  - `flags` is an integer. Use 0 for no flags, or bitwise-or together one or more results of the flags/1 function.
  - `mode` is an integer similar to `flags`.
  - `platform`. Either the result of `populate_platform` or nil.

  See the docs for more information:
  http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile
  """
  def compile(_expression, _flags, _mode, _platform), do: exit(:nif_not_loaded)

  @doc """
  Compile a regular expression into a database.

  - `expression` is a regular expression, without the leading/trailing `/` characters or flags.
  - `flags` is an integer. Use 0 for no flags, or bitwise-or together one or more results of the flags/1 function.
  - `mode` is an integer similar to `flags`.
  - `platform`. Either the result of `populate_platform` or nil.

  See the docs for more information:
  http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile
  """
  def compile_multi(_expression_list, _flags_list, _id_list, _mode, _platform), do: exit(:nif_not_loaded)

  @doc """
  Returns a map with debugging info about a regular expression.
  """
  def expression_info(_expression, _flags), do: exit(:nif_not_loaded)

  @doc """
  Allocate a scratch memory buffer for use by  when running the
  regular expression.
  """
  def alloc_scratch(_db), do: exit(:nif_not_loaded)

  @doc """
  Re-allocate a scratch memory buffer. If the scratch was allocated for a
  different database and is too small for the given one, it will be
  reallocated. Returns :ok.
  """
  def realloc_scratch(_db, _scratch), do: exit(:nif_not_loaded)
  def clone_scratch(_scratch), do: exit(:nif_not_loaded)
  def scratch_size(_scratch), do: exit(:nif_not_loaded)

  @doc """
  Test whether the given compiled regex matches a string. Requires a scratch
  buffer allocated by alloc_scratch/1.
  """
  def match(_db, _string, _scratch), do: exit(:nif_not_loaded)

  @doc """
  Test whether multiple compiled regexes match a string. The regex should be
  compiled by compile_multi. Returns a list of expression IDs that matched
  the string. Note that the order of the returned IDs is undefined.
  """
  def match_multi(_db, _string, _scratch), do: exit(:nif_not_loaded)
end
