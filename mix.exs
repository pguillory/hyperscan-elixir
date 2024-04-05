defmodule Hyperscan.MixProject do
  use Mix.Project

  def project do
    [
      package: package(),
      app: :hyperscan,
      version: "0.1.0",
      elixir: "~> 1.16",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_targets: ["nifs"],
      make_clean: ["clean"],
      deps: deps()
    ]
  end

  defp package do
    [
      description: "Bindings for Hyperscan, a high performance multiple-regex matching library",
      licenses: ["MIT"],
      maintainers: ["pguillory@gmail.com"],
      links: %{
        "GitHub" => "https://github.com/pguillory/hyperscan-elixir"
      }
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [
      {:elixir_make, ">= 0.0.0", runtime: false},
      {:ex_doc, ">= 0.0.0", only: :dev, runtime: false}
    ]
  end
end
