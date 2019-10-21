# local_backend

## Generating and using build configuration from CMake

The first time you build, you must run `./devops/build_init.sh` from the project root.
CMake will configure a `build/` directory containing a `make` configuration.

After this, run `./devops/build.sh` to build from the project root.

To run the built binary, use `./devops/run.sh`.
