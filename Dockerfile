# The location of the Chuffed source in the container.
ARG CHUFFED_DIR=/usr/local/src/chuffed

#=======================================================================================================================

# Specify the build environment as a separate stage
# so it can be used to compile the source code during development.

# Derive from the GCC Docker image.
FROM gcc:14.1 AS build-env

# Install CMake, Flex and Bison, which are used to compile the source files of Chuffed.
RUN apt-get -y update && \
    apt-get install -y cmake flex bison && \
    apt-get autoclean && \
    apt-get autoremove && \
    apt-get clean

#=======================================================================================================================

# Inherent the build environment.
FROM build-env AS fzn-chuffed

# Import the global variable.
ARG CHUFFED_DIR

# Specify the working directory
WORKDIR ${CHUFFED_DIR}

# Copy all the source code and build files into the container.
#COPY ./chuffed ./chuffed/
#COPY ./tests ./tests/
#COPY ./CMakeLists.txt .
#COPY ./chuffed.msc.in .
#COPY ./chuffed-config.cmake.in .
#COPY ./.clang-format .
#COPY ./.clang-tidy .
# Copy all files. The .dockerignore file ensures only relevant files are included in the context.
COPY . .

# Create the build directory to which the compilation artifacts will be written.
# IntelliJ copies empty directories into the container, so the build directory may already exist.
RUN mkdir -p build

# Enter build directory.
WORKDIR ${CHUFFED_DIR}/build

# Compile the project using CMake.
RUN cmake ..
RUN cmake --build .

#=======================================================================================================================

# Inherent the build environment.
FROM build-env AS fzn-chuffed-cache-events

# Import the global variable.
ARG CHUFFED_DIR

# Specify the working directory
WORKDIR ${CHUFFED_DIR}

# Copy everything from the Docker context, which contains all the C++ source code, to the Docker image.
COPY . .

# Create the build directory to which the compilation artifacts will be written.
# IntelliJ copies empty directories into the container, so the build directory may already exist.
RUN mkdir -p build

# Enter build directory.
WORKDIR ${CHUFFED_DIR}/build

# Compile the project using CMake with the logging of cache events enabled.
RUN cmake -DLOG_CACHE_EVENTS=1 ..
RUN cmake --build .

#=======================================================================================================================

# Inherent the build environment.
FROM build-env

# Import the global variable.
ARG CHUFFED_DIR

# Store the executables in the local binaries directory.
WORKDIR /usr/local/bin

# Copy the executables from the intermediate containers.
COPY --from=fzn-chuffed ${CHUFFED_DIR}/build/fzn-chuffed .
COPY --from=fzn-chuffed-cache-events ${CHUFFED_DIR}/build/fzn-chuffed ./fzn-chuffed-cache-events

# Make the executable that does not log cache events the entrypoint.
CMD ["fzn-chuffed"]