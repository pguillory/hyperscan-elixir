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
end
