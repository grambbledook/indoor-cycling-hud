[string]$BuildType = "Debug"

# Check for release flags
if ($args -contains "-r" -or $args -contains "r" -or $args -contains "--release" -or $args -contains "release") {
    $BuildType = "Release"
}

# Configure and build the project
cmake --preset=default -DCONFIG="$BuildType"
cmake --build build --config "$BuildType"
