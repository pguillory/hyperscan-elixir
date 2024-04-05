# Hyperscan

Elixir bindings for Hyperscan, a high performance multiple-regex matching
library.

Hyperscan was published by Intel and allows simultaneous matching of up to
tens of thousands of regular expressions simulataneously and efficiently. It
is used for things like [deep packet inspection].

[deep packet inspection]: https://en.wikipedia.org/wiki/Deep_packet_inspection

These bindings try to stay as close as possible to the C API provided by
Hyperscan so that its [excellent documentation] will be useful to you, as a
user of the library.

[excellent documentation]: https://intel.github.io/hyperscan/dev-reference/api_files.html

## Example

```elixir
# Compile a regular expression.
{:ok, db} = Hyperscan.compile("quick", 0)

# Allocate scratch space for matching it.
{:ok, scratch} = Hyperscan.alloc_scratch(db)

# Test the regex against a string.
{:ok, true} = Hyperscan.match(db, "the quick brown fox", scratch)
```

## Installation

Building the NIF requires the Hyperscan library to be installed. Support for
ARM-based systems is available through a fork called Vectorscan.

```bash
# On an x86-64 Linux host try:
apt install libhyperscan-dev
```

```bash
# On an M1/M2 Macbook, this should work:
brew install vectorscan
```

Then add it to your project dependencies in Mix.exs:
```elixir
def deps do
  [
    {:hyperscan, "~> 0.1.0"}
  ]
end
```
