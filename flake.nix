# very sorry
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {

        devShells.default = pkgs.mkShell {
          packages =
            with pkgs;
            [
              cmake
              clang-tools
              python3
              poetry
              mongodb-ce
            ]
            ++ (with python3Packages; [
              pybind11-stubgen
              numpy
              pandas
              matplotlib
              plotly
              scipy
              jupyter
              parquet
              pyarrow
              python-dotenv
              tqdm
              pymongo
            ]);

            # veeeryyy sorryyy
            shellHook = ''
              echo "Python: $(python --version)"
              echo "Virtualenv: $VIRTUAL_ENV"
              echo "Tip: use 'poetry run ...' or 'python -m <module>'"

              set -eu
              set +u
              if [ -z "$MONGO_PORT" ]; then MONGO_PORT=27017; fi
              set -u
              export MONGO_PORT="$MONGO_PORT"

              export MONGO_ROOT="$PWD/.dev/mongo"
              export MONGO_DATA_DIR="$MONGO_ROOT/data"
              export MONGO_LOG_DIR="$MONGO_ROOT"
              export MONGODB_URI="mongodb://127.0.0.1:$MONGO_PORT"

              mkdir -p "$MONGO_DATA_DIR" "$MONGO_LOG_DIR" .dev/bin

              cat > .dev/bin/mongo-up <<'EOS'
              #!/usr/bin/env bash
              set -euo pipefail

              if [ -z "$MONGO_PORT" ]; then MONGO_PORT=27017; fi
              if [ -z "$MONGO_ROOT" ]; then MONGO_ROOT="$PWD/.dev/mongo"; fi
              if [ -z "$MONGO_DATA_DIR" ]; then MONGO_DATA_DIR="$MONGO_ROOT/data"; fi
              if [ -z "$MONGO_LOG_DIR" ]; then MONGO_LOG_DIR="$MONGO_ROOT"; fi

              mkdir -p "$MONGO_DATA_DIR" "$MONGO_LOG_DIR"
              if pgrep -f "mongod.*--port $MONGO_PORT" >/dev/null; then
                echo "mongod already running on port $MONGO_PORT"
                exit 0
              fi

              mongod \
                --dbpath "$MONGO_DATA_DIR" \
                --logpath "$MONGO_LOG_DIR/mongod.log" \
                --bind_ip 127.0.0.1 \
                --port "$MONGO_PORT" \
                --fork --quiet

              echo "MONGODB_URI=mongodb://127.0.0.1:$MONGO_PORT" > "$MONGO_LOG_DIR/.env"
              echo "mongod started (log: $MONGO_LOG_DIR/mongod.log)"
              EOS

              cat > .dev/bin/mongo-down <<'EOS'
              #!/usr/bin/env bash
              set -euo pipefail
              # Try to match this project's DB path
              pids=$(pgrep -f "mongod.*--dbpath $PWD/.dev/mongo/data" || true)
              if [ -n "$pids" ]; then
                kill $pids
                echo "mongod stopped"
              else
                echo "mongod not running"
              fi
              EOS

              cat > .dev/bin/mongo-wipe <<'EOS'
              #!/usr/bin/env bash
              set -euo pipefail
              rm -rf "$PWD/.dev/mongo/data"
              mkdir -p "$PWD/.dev/mongo/data"
              echo "wiped data dir"
              EOS

              chmod +x .dev/bin/mongo-*
              export PATH="$PWD/.dev/bin:$PATH"

              echo
              echo "Mongo helpers ready:  mongo-up  |  mongo-down  |  mongo-wipe"
              echo "MONGODB_URI=$MONGODB_URI"
              echo            
            '';
        };
      }
    );
}
