import os
import subprocess
import sys
import argparse
from regex import Regex, RegexFlag

NOTAPPLICABLE = "(Not Applicable)"
VS_PATTERN = r"((?:Visual Studio (?P<Version>\d{2}) (?P<Year>\d{4}))(?!\s\[arch\]))" # Excludes VS 2015 and lower
BASE_DIR = os.path.dirname(os.path.abspath(__file__)).replace('\\', '/')
distro = NOTAPPLICABLE
use_wsl = False

def get_abs_build_path(relative):        
    if use_wsl:
        drive, rest = BASE_DIR.split(":", 1)
        drive = drive.lower()
        base_dir = f"/mnt/{drive}{rest}"
    else:
        base_dir = BASE_DIR
    
    return os.path.join(base_dir, relative).replace('\\', '/')
    
def ask(prompt, options):
    while True:
        try:
            print(f"\n{prompt}")
            for i, opt in enumerate(options):
                print(f"  {i + 1}) {opt}")
                
            if len(options) == 1:
                print(f"\nAuto selected: {options[0]}")
                return options[0]

            choice = int(input("> ")) - 1

            if 0 <= choice < len(options):
                return options[choice]

            print("Invalid selection.")

        except KeyboardInterrupt:
            print("\nCancelled, exiting...")
            raise SystemExit(1)

        except:
            print("Invalid selection.")
            
def pick(name, value, prompt, options):
    if isinstance(options[0], str):
        if value:
            if value in options:
                print(f"{prompt}: {value} (CLI)")
                return value
            raise Exception(f"Invalid {name}: {value}")

        return ask(prompt, options)
    else:
        values = [v for _, v in options]

        if value:
            if value in values:
                print(f"{prompt}: {value} (CLI)")
                return value
            raise Exception(f"Invalid {name}: {value}")

        choice = ask(prompt, [label for label, _ in options])
        return dict(options)[choice]

def run(cmd, use_wsl=False, distro=None, cwd=None):
    if use_wsl:
        if not distro:
            raise Exception("WSL distro not set")

        if cwd:
            cwd = subprocess.check_output(
                ["wsl", "wslpath", cwd],
                text=True
            ).strip()

            cmd = ["wsl", "-d", distro, "--cd", cwd] + cmd
        else:
            cmd = ["wsl", "-d", distro] + cmd
    else:
        if cwd:
            subprocess.check_call(cmd, cwd=cwd)
            return
        
    print("\n>>", " ".join(cmd))
    subprocess.check_call(cmd)

# Check for Windows Subsystem for Linux
def check_wsl_installed():
    try:
        return subprocess.run(
            ["wsl", "-l", "-q"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL
        ).returncode == 0
    except FileNotFoundError:
        return False

def get_wsl_distros():
    try:
        result = subprocess.run(
            ["wsl", "-l", "-q"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=False
        )

        text = result.stdout.decode("utf-16-le", errors="ignore")
        
        return [
            line.strip()
            for line in text.splitlines()
            if line and not line.isspace() and not line == '\0'
        ]
    except FileNotFoundError:
        return []
    except subprocess.CalledProcessError:
        return []
    
def get_vs_generators():
    generators = []
    
    result = subprocess.run(
            ["cmake", "--help"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
    
    lines = result.stdout.splitlines()
    regex = Regex(VS_PATTERN)

    for line in lines:
        match = regex.search(line)
        if match:
            version = int(match.group(2))
            year = int(match.group(3))

            full = match.group(1)

            generators.append({
                "full": full,
                "version": version,
                "year": year,
            })

    return generators

# -----------------------------
# OS detection
# -----------------------------
if sys.platform.startswith("win"):
    os_name = "win"
elif sys.platform.startswith("darwin"):
    os_name = "mac"
else:
    os_name = "linux"
    
print(f"\nDetected OS: {os_name}")

platform = os_name
has_wsl = os_name == "win" and check_wsl_installed()
distros = None if has_wsl is False else get_wsl_distros()
vs_generators = get_vs_generators()
    
# -----------------------------
# Arguments
# -----------------------------
parser = argparse.ArgumentParser(
    description=
"""QSPI CMake Build Wizard

This tool configures (and optionally builds) the QSPI project using CMake.

All arguments are optional. If a required option is not provided, the wizard
will interactively prompt for the missing values.

Some options depend on others (for example SDK and architecture combinations).
Invalid combinations will be rejected during configuration.
"""
)

build_group = parser.add_argument_group(
    title="Build Options")

build_group.add_argument(
    '-s', "--sdk",
    choices=['saleae', 'kingst'],
    required=False,
    help="Select the SDK to build against (Saleae or KingstVis)."
)

build_group.add_argument(
    '-a', "--arch",
    choices=['x64', 'arm64', 'Win32'],
    metavar="Saleae: {x64,arm64} | Kingst: {x64,Win32}",
    required=False,
    help="Target CPU architecture. "
         "Saleae supports: x64, arm64. "
         "KingstVis supports: x64, Win32."
)

build_group.add_argument(
    '-c', "--config",
    choices=["Debug", "Release"] if os_name != "win" else ["Debug", "Release", "MinSizeRel", "RelWithDebInfo"],
    required=False,
    metavar="{Debug,Release} | Additional Windows: {MinSizeRel,RelWithDebInfo}",
    help="Build configuration." + 
    ("" if os_name != "win" else " 'MinSizeRel' and 'RelWithDebInfo' are Windows only.")
)

parser.add_argument(
    '-S', "--skip",
    action="store_true",
    required=False,
    help="Skip the final confirmation prompt before running configuration."  + 
    ("" if os_name != "win" else " Skips Build prompt and uses value of '--build'.")
)

win_group = parser.add_argument_group(
    title="Windows Only")

if os_name == "win":
    win_group.add_argument(
        '-B', "--build",
        action="store_true",
        required=False,
        help="Automatically build the project after CMake configuration."
    )

    win_group.add_argument(
        '-v', "--vs",
        choices=[g['version'] for g in vs_generators],
        required=False,
        help="Visual Studio generator version to use for CMake."
    )
    
    # Used to set vs when options are not known.
    # Selects newest installed VS version    
    win_group.add_argument(
        "-vsi", "--vs-index",
        type=int,
        metavar=f"0-{len(vs_generators) - 1}",
        required=False,
        default=None,
        help="Selects a Visual Studio generator by index. Warning: Will raise if used with --vs."
    )
else:    
    parser.add_argument('-B', '--build', required=False, help=argparse.SUPPRESS)
    parser.add_argument('-v', '--vs', required=False, help=argparse.SUPPRESS)
    parser.add_argument('-p', '--platform', required=False, help=argparse.SUPPRESS)
    parser.add_argument('-u', '--distro', required=False, help=argparse.SUPPRESS)
    parser.add_argument('-p', '--vs-index', required=False, help=argparse.SUPPRESS)
    parser.add_argument('-u', '--distro-index', required=False, help=argparse.SUPPRESS)

if has_wsl:
    win_group.add_argument(
        '-p', "--platform",
        choices=['win', 'linux'],
        required=False,
        help="Target platform. Selecting 'linux' requires WSL with at least one installed distribution."
    )
    win_group.add_argument(
        '-u', "--distro",
        choices=distros,
        required=False,
        help="WSL distribution to use when targeting Linux."
    )
    
    # Used to set distro when options are not known
    # Selects first distro in the list
    win_group.add_argument(
        "-di", "--distro-index",
        type=int,
        metavar=f"0-{len(distros)-1}",
        required=False,
        default=None,
        help="Selects a WSL linux distro by index. Warning: Will raise if used with --distro."
    )
    


args = parser.parse_args()


# -----------------------------
# Argument conversion
# -----------------------------
# Convert argument option from version (18) to full string (Visual Studio 18 2026)
if args.vs is not None:
    if args.vs_index is not None:
        raise argparse.ArgumentError("Both --vs and --vs-index were set")
    args.vs = next(
        (g for g in vs_generators if g["version"] == args.vs),
        None
    )["full"]

# Set vs value from index
if args.vs_index is not None:
    args.vs = vs_generators[args.vs_index]["full"]    

# Set distro from index
if args.distro_index is not None:
    if args.distro is not None:
        raise argparse.ArgumentError("Both --distro and --distro-index were set")
    if len(distros) != 0:
        args.distro = distros[args.distro_index]

# -----------------------------
# Target Platform
# -----------------------------
if has_wsl:
    platforms = [
        ("Windows", "win"),
        ("Linux (WSL)", "linux"),
    ]
    if pick("--platform", args.platform, "Select platform:", platforms) == "linux":
        use_wsl = True
        platform = "linux"
        if len(distros) == 0:
            print(f"Error: No available linux distros. Exiting...", file=sys.stderr)
        distro = pick("--distro", args.distro, "Select a linux distrobution:", distros)

# -----------------------------
# Select SDK
# -----------------------------
sdk = pick("--sdk", args.sdk, "Select analyzer SDK:", ["saleae", "kingst"])

# -----------------------------
# Select architecture
# -----------------------------
arches = ["x64"]

if sdk == "saleae":
    arches.append("arm64")

if sdk == "kingst" and platform == "win":
    print(platform)
    arches.append("Win32")

arch = pick("--arch", args.arch, "Select architecture:", arches)

# -----------------------------
# Select build configuration
# -----------------------------
build_config = pick("--config", args.config, "Select build configuration:", ["Debug", "Release"] if platform != "win" else ["Debug", "Release", "MinSizeRel", "RelWithDebInfo"] )

# -----------------------------
# Build directory
# -----------------------------
build_dir = get_abs_build_path(f"build/{sdk}_{platform}_{arch}")

if platform != "win":
    build_dir += '_' + build_config.lower()
    
# -----------------------------
# CMake command
# -----------------------------
cmake_cmd = [
    "cmake",
    "-S", ".",
    "-B", build_dir,
    f"-DTARGET_SDK={sdk}",
]

# -----------------------------
# Windows generator
# -----------------------------
if platform == "win":

    vs = pick("--vs", args.vs, "Select Visual Studio generator:", [g['full'] for g in vs_generators])

    cmake_cmd.extend([
        "-G", vs,
        "-A", arch
    ])

else:
    vs = NOTAPPLICABLE
    cmake_cmd.append(f"-DCMAKE_BUILD_TYPE={build_config}")

# -----------------------------
# macOS arch
# -----------------------------
if platform == "mac":
    cmake_cmd.append(f"-DCMAKE_OSX_ARCHITECTURES={arch}")

# -----------------------------
# Confirmation
# -----------------------------
print("\nConfirm Selected Options:")
print(f"""
  SDK: {sdk}
  Arch: {arch}
  Platform: {platform}
  Configuration: {build_config}
  WSL Distro: {distro}
  Visual Studio: {vs}
  Build Directory: {build_dir}
""")

if args.skip is False:
    if ask("Continue?", ["yes", "no"]) == "no":
        print("Cancelled, exiting...")
        exit(0)

# -----------------------------
# Configure
# -----------------------------
print("\nConfiguring...")
run(cmake_cmd, use_wsl=use_wsl, distro=distro)

print("\nConfiguration complete!")

# -----------------------------
# Build?
# -----------------------------
do_build = (platform != "win" or args.build is True if args.skip is True else
    pick("--build", args.build, "Build project?", ["yes", "no"]) == "yes")

if do_build:
    build_cmd = ["cmake", "--build", build_dir]

    if platform == "win":
        build_cmd += ["--config", build_config]

    print("\nBuilding...")
    run(build_cmd, use_wsl=use_wsl, distro=distro)

    print("\nBuild complete!")