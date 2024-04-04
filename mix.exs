defmodule Vectorscan.MixProject do
  use Mix.Project

  def project do
    [
      app: :vectorscan,
      version: "0.1.0",
      elixir: "~> 1.16",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_targets: ["nifs"],
      make_clean: ["clean"],
      deps: deps()
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [
      {:elixir_make, ">= 0.0.0", runtime: false}
    ]
  end
end
