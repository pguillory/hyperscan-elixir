defmodule HyperscanTest do
  import Hyperscan
  use ExUnit.Case

  doctest Hyperscan

  test "populate_platform" do
    {:ok, platform_info} = populate_platform()
    assert is_reference(platform_info)

    assert platform_info_to_map(platform_info) == %{
             cpu_features: 0,
             reserved1: 0,
             reserved2: 0,
             tune: 0
           }
  end

  test "valid_platform" do
    {:ok, valid_platform} = valid_platform()
    assert valid_platform == true
  end

  test "version" do
    assert version() |> is_binary()
  end

  test "compile" do
    {:ok, db} = compile("asdf", 0, mode("HS_MODE_BLOCK"))
    assert db |> is_reference()
  end

  test "compile_multi" do
    {:ok, db} = compile_multi(["asdf"], [0], [0], mode("HS_MODE_BLOCK"))
    assert db |> is_reference()
  end

  test "expression_info" do
    {:ok, expression_info} = expression_info("asdf?", 0)

    assert expression_info == %{
             min_width: 3,
             max_width: 4,
             unordered_matches: false,
             matches_at_eod: false,
             matches_only_at_eod: false
           }
  end

  test "database_info" do
    {:ok, db} = compile("asdf", 0, mode("HS_MODE_BLOCK"))
    {:ok, database_info} = database_info(db)
    assert is_binary(database_info)
  end

  test "database_size" do
    {:ok, db} = compile("asdf", 0, mode("HS_MODE_BLOCK"))
    {:ok, database_size} = database_size(db)
    assert is_integer(database_size)
  end

  test "serialize_database" do
    {:ok, db} = compile("foo", 0, mode("HS_MODE_BLOCK"))
    {:ok, binary} = serialize_database(db)
    assert is_binary(binary)
    {:ok, db2} = deserialize_database(binary)
    {:ok, scratch} = alloc_scratch(db2)
    assert match(db2, "foo", scratch) == {:ok, true}
    assert match(db2, "bar", scratch) == {:ok, false}
  end

  test "various scratch functions" do
    {:ok, db} = compile("a", 0, mode("HS_MODE_BLOCK"))
    {:ok, scratch} = alloc_scratch(db)
    :ok = realloc_scratch(db, scratch)
    {:ok, _scratch2} = clone_scratch(scratch)
    {:ok, _size} = scratch_size(scratch)
  end

  test "match" do
    {:ok, db} = compile("a", 0, mode("HS_MODE_BLOCK"))
    {:ok, scratch} = alloc_scratch(db)
    assert match(db, "abc", scratch) == {:ok, true}
    assert match(db, "xyz", scratch) == {:ok, false}
  end

  test "match_multi" do
    {:ok, db} = compile_multi(["a", "b"], [0, 0], [1, 2], mode("HS_MODE_BLOCK"))
    {:ok, scratch} = alloc_scratch(db)
    assert match_multi(db, "abc", scratch) == {:ok, [2, 1]}
    assert match_multi(db, "ayz", scratch) == {:ok, [1]}
    assert match_multi(db, "xyz", scratch) == {:ok, []}
  end

  test "replace" do
    {:ok, db} = compile("a", flag("HS_FLAG_SOM_LEFTMOST"), mode("HS_MODE_BLOCK"))
    {:ok, scratch} = alloc_scratch(db)
    assert replace(db, "abab", "A", scratch) == {:ok, "AbAb"}
    assert replace(db, "baba", "A", scratch) == {:ok, "bAbA"}
  end
end
