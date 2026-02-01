# Playground

Motivation:

- **Modern Vulkan**: Latest graphics API features and best practices
- **Modern C++**: C++20 standards and idioms
- **Algorithm Experimentation**: Basic rendering and AI techniques
- **AI utilization**: Usage of 🤖 in the project

## Quick Start

```bash
# Configure
cmake --preset=ninja

# Build
cd build
ninja

# Run tests
cd ..\bin
PlaygroundSDK_test.exe
```

See [BUILD.md](BUILD.md) for detailed instructions.

## Project Structure

- **src/** - Reusable SDK (core, cpu, gpu, engine modules)
- **example??/** - Sample applications demonstrating SDK usage
- **test/** - Unit tests for SDK components
- **resources/** - Shaders and test images
- **docs/** - Technical documentation
- **tools/** - Development utilities

## Documentation

- [Build Instructions](BUILD.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Coordinate System](docs/coordinate_system.md)

## Contributing

Contributions are welcome! Please read the [Contributing Guide](CONTRIBUTING.md) for details on our code style, development workflow, and how to submit changes.

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) for details.

Third-party dependencies are documented in [THIRD-PARTY-LICENSES.md](THIRD-PARTY-LICENSES.md).
