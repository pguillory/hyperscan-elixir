defmodule Vectorscan do
  @on_load :init

  def init do
    priv_dir = :code.priv_dir(:vectorscan)
    nif_path = Path.join(priv_dir, "vectorscan")
    :ok = :erlang.load_nif(nif_path, 0)
  end

  def populate_platform(), do: exit(:nif_not_loaded)
  def platform_info_to_map(_platform_info), do: exit(:nif_not_loaded)
  def valid_platform(), do: exit(:nif_not_loaded)
  def version(), do: exit(:nif_not_loaded)
  def flag(_name), do: exit(:nif_not_loaded)
  def mode(_name), do: exit(:nif_not_loaded)
  def compile(_expression, _flags, _mode, _platform), do: exit(:nif_not_loaded)
  def compile_multi(_expression_list, _flags_list, _id_list, _mode, _platform), do: exit(:nif_not_loaded)
  def expression_info(_expression, _flags), do: exit(:nif_not_loaded)
  def alloc_scratch(_db), do: exit(:nif_not_loaded)
  def realloc_scratch(_db, _scratch), do: exit(:nif_not_loaded)
  def clone_scratch(_scratch), do: exit(:nif_not_loaded)
  def scratch_size(_scratch), do: exit(:nif_not_loaded)
  def match(_db, _string, _scratch), do: exit(:nif_not_loaded)
  def match_multi(_db, _string, _scratch), do: exit(:nif_not_loaded)
end
