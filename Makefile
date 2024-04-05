.PHONY: nifs clean

ERL_PATH = $(shell elixir -e 'IO.puts [:code.root_dir, "/erts-", :erlang.system_info :version]')
CFLAGS := -fPIC -I $(ERL_PATH)/include

UNAME_SYS := $(shell uname -s)
ifeq ($(UNAME_SYS), Darwin)
	CFLAGS += -bundle -bundle_loader $(ERL_PATH)/bin/beam.smp
else ifeq ($(UNAME_SYS), FreeBSD)
	CFLAGS += -shared
else ifeq ($(UNAME_SYS), Linux)
	CFLAGS += -shared
endif

nifs: priv/hyperscan.so

priv:
	mkdir -p priv

priv/hyperscan.so: priv src/hyperscan.c
	gcc -o priv/hyperscan.so src/hyperscan.c $(CFLAGS) -I /opt/homebrew/Cellar/vectorscan/5.4.11/include/hs -L/opt/homebrew/Cellar/vectorscan/5.4.11/lib -lhs

clean:
	rm -f priv/hyperscan.so
