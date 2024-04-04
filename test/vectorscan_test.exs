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
    assert version() |> is_binary()
  end

  test "compile" do
    {:ok, db} = compile("asdf", 0, mode("HS_MODE_BLOCK"), nil)
    assert db |> is_reference()
  end

  test "various scratch functions" do
    {:ok, db} = compile("a", 0, mode("HS_MODE_BLOCK"), nil)
    {:ok, scratch} = alloc_scratch(db)
    :ok = realloc_scratch(db, scratch)
    {:ok, _scratch2} = clone_scratch(scratch)
    {:ok, _size} = scratch_size(scratch)
  end

  test "match" do
    {:ok, db} = compile("a", 0, mode("HS_MODE_BLOCK"), nil)
    {:ok, scratch} = alloc_scratch(db)
    assert match(db, "abc", scratch) == {:ok, true}
    assert match(db, "xyz", scratch) == {:ok, false}
  end
end
