# Build the project and run the program passed as an argument
import os
import subprocess
import argparse

# Path to directories
root_dir = os.path.abspath(os.path.dirname(__file__))  # Path to root directory
build_dir = os.path.join(root_dir, 'build', 'Release')  # Path to build/Release
programs_dir = os.path.join(root_dir, 'build', 'Release', 'programs')  # Path to build/Release/programs

# Commands
cmake_command = [
    "cmake", "../..", 
    "-DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake", 
    "-DCMAKE_BUILD_TYPE=Release"
]

build_command = ["cmake", "--build", "."]


def run_command(command, cwd=None):
    """Runs a command in the terminal."""
    try:
        # Execute the command without capturing the output
        result = subprocess.run(command, cwd=cwd, check=True, text=True)

    except subprocess.CalledProcessError as e:
        print(f"Error executing the command: {e}")
        raise

def main(program_name):
    # Check if the build/Release directory exists
    if not os.path.exists(build_dir):
        print(f"The directory {build_dir} does not exist.")
        return
    
    # Run the cmake command in build/Release
    print(f"Moving to {build_dir} and running CMake...")
    run_command(cmake_command, cwd=build_dir)
    
    # Compile the project
    print("Compiling the project...")
    run_command(build_command, cwd=build_dir)
    
    # Run the specified program
    program_path = os.path.join(programs_dir, program_name)
    if os.path.exists(program_path):
        print(f"Running the program {program_name}...")
        run_command([program_path], cwd=programs_dir)
    else:
        print(f"The program {program_name} does not exist in {programs_dir}.")

if __name__ == "__main__":
    # Define the argument parser
    parser = argparse.ArgumentParser(description='Script to build and run a program.')
    parser.add_argument('program', help='The name of the program to run after building')
    
    # Get the arguments
    args = parser.parse_args()
    
    # Call the main function with the specified program
    main(args.program)


# Example usage:
# python buildLinuxExe.py compute_shader1610
