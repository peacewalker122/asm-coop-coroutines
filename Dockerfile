# syntax=docker/dockerfile:1

# Build stage installs toolchain, compiles the coroutine POC
FROM debian:bookworm-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential \
       nasm \
       make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

# Produce the default release binary in build/main
RUN make

# Runtime stage keeps the image slim but still has libc for the binary
FROM debian:bookworm-slim AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       libgcc-s1 \
       libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy artifacts and optional docs for reference inside the image
COPY --from=build /app/build/main /usr/local/bin/coroutine-demo
COPY README.md ./

# Default command runs the demo coroutine program
CMD ["coroutine-demo"]
