defmodule Vectorscan do
  @on_load :init

  def init do
    priv_dir = :code.priv_dir(:vectorscan)
    nif_path = Path.join(priv_dir, "vectorscan")
    :ok = :erlang.load_nif(nif_path, 0)
  end

  # def new(_capacity), do: exit(:nif_not_loaded)
end
