defmodule VectorscanTest do
  import Vectorscan
  use ExUnit.Case

  test "populate_platform" do
    {:ok, platform_info} = populate_platform()
    assert is_reference(platform_info)
    assert platform_info_to_map(platform_info) == %{cpu_features: 0, reserved1: 0, reserved2: 0, tune: 0}
  end

  test "valid_platform" do
    {:ok, valid_platform} = valid_platform()
    assert valid_platform == true
  end

  test "version" do
    # assert version() == "5.4.11 2023-11-21"
    assert version() |> is_binary()
  end
end
