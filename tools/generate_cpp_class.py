#!/usr/bin/env python3
"""
C++ Class Generator
Generates C++ header and implementation files following the project's coding style.
All copy/move operators and constructors are marked as deleted.
"""

import sys
import os
import re
from pathlib import Path


def to_snake_case(name):
    """Convert CamelCase to snake_case."""
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def generate_header_guard(class_name, namespace_path=""):
    """Generate header guard in project style: NAMESPACE_PATH_CLASSNAME_H_"""
    if namespace_path:
        guard = namespace_path.upper().replace('/', '_').replace('\\', '_') + '_'
    else:
        guard = ""
    guard += class_name.upper() + "_H_"
    return guard


def generate_header(class_name, namespace_path=""):
    """Generate C++ header file content."""
    header_guard = generate_header_guard(class_name, namespace_path)
    
    header = f"""#ifndef {header_guard}
#define {header_guard}

class {class_name}
{{

public:

	{class_name}() = delete;

	{class_name}(const {class_name}&) = delete;
	{class_name}({class_name}&&) = delete;

	{class_name} operator=(const {class_name}&) = delete;
	{class_name} operator=({class_name}&&) = delete;

}};

#endif /* {header_guard} */
"""
    return header


def generate_implementation(class_name):
    """Generate C++ implementation file content."""
    impl = f"""#include "{class_name}.h"
"""
    return impl


def write_file(filepath, content):
    """Write content to file."""
    with open(filepath, 'w', newline='\n') as f:
        f.write(content)
    print(f"Generated: {filepath}")


def main():
    if len(sys.argv) < 2:
        print("Usage: python generate_cpp_class.py <ClassName> [namespace_path]")
        print()
        print("Examples:")
        print("  python generate_cpp_class.py MyClass")
        print("  python generate_cpp_class.py MyClass core/utility")
        print("  python generate_cpp_class.py MyClass gpu/vulkan/utility")
        print()
        print("This will generate <ClassName>.h and <ClassName>.cpp in the current directory.")
        print("The namespace_path is used for the header guard only.")
        sys.exit(1)
    
    class_name = sys.argv[1]
    namespace_path = sys.argv[2] if len(sys.argv) > 2 else ""
    
    # Validate class name
    if not re.match(r'^[A-Z][A-Za-z0-9]*$', class_name):
        print(f"Error: Invalid class name '{class_name}'")
        print("Class name should start with an uppercase letter and contain only alphanumeric characters.")
        sys.exit(1)
    
    # Generate files
    header_file = f"{class_name}.h"
    impl_file = f"{class_name}.cpp"
    
    header_content = generate_header(class_name, namespace_path)
    impl_content = generate_implementation(class_name)
    
    write_file(header_file, header_content)
    write_file(impl_file, impl_content)
    
    print()
    print("Files generated successfully!")


if __name__ == "__main__":
    main()
