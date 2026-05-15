import os
import sys
import zipfile
import argparse
import hashlib
import json
from pathlib import Path


# ----------------------------
# CLI
# ----------------------------
parser = argparse.ArgumentParser(
    description="""
Packages QSPI Analyzer Release binaries into ZIP files.
Expected format: {sdk}_{platform}_{arch}_{config}
(Windows omits config)
"""
)

parser.add_argument("--output", type=Path, required=False)
parser.add_argument("--version", required=True)

args = parser.parse_args()


# ----------------------------
# Build model
# ----------------------------
class BuildFolder:
    VALID_SDKS = {"saleae", "kingst"}
    VALID_PLATFORMS = {"win", "linux", "mac"}
    VALID_ARCHES = {"x64", "arm64", "Win32"}

    def __init__(self, path: Path, sdk, platform, arch, config):
        self.path = path
        self.sdk = self._validate("SDK", sdk, self.VALID_SDKS)
        self.platform = self._validate("platform", platform, self.VALID_PLATFORMS)
        self.arch = self._validate("arch", arch, self.VALID_ARCHES)
        self.config = config

    @staticmethod
    def _validate(name, value, valid_set):
        if value not in valid_set:
            raise RuntimeError(f"Invalid {name}: {value}")
        return value

    def filename(self, version):
        return f"QSPI_Analyzer_{self.sdk}_{self.platform}_{self.arch}_{version}.zip"


# ----------------------------
# Helpers
# ----------------------------
def get_binary(folder: BuildFolder) -> Path:
    if folder.platform == "win":
        return folder.path / "Release" / "QSPI_Analyzer.dll"
    if folder.platform == "linux":
        return folder.path / "QSPI_Analyzer.so"
    if folder.platform == "mac":
        return folder.path / "QSPI_Analyzer.dylib"
    raise RuntimeError(f"Unknown platform: {folder.platform}")


def parse_folder(path: Path):
    try:
        parts = path.name.split("_")
        if len(parts) not in (3, 4):
            return None

        sdk, platform, arch = parts[:3]
        config = parts[3] if len(parts) == 4 else None

        return BuildFolder(path, sdk, platform, arch, config)
    except Exception:
        return None


def zip_binary(file: Path, output_zip: Path):
    with zipfile.ZipFile(output_zip, "w", zipfile.ZIP_DEFLATED) as z:
        z.write(file, arcname=file.name)


def hash_file(path: Path, chunk_size=1024 * 1024):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(chunk_size):
            h.update(chunk)
    return h.hexdigest()


# ----------------------------
# Paths
# ----------------------------
build_dir = Path(__file__).resolve().parents[1] / "build"
output_dir = args.output or build_dir
output_dir = output_dir if output_dir.is_absolute() else build_dir / output_dir

json_path = output_dir / "binary_hashes.json"


# ----------------------------
# Load cache
# ----------------------------
hash_db = {}
if json_path.exists():
    hash_db = json.loads(json_path.read_text())


# ----------------------------
# Validation
# ----------------------------
if not build_dir.exists():
    raise RuntimeError(f"Build directory missing: {build_dir}")

if not output_dir.exists():
    raise RuntimeError(f"Output directory missing: {output_dir}")


# ----------------------------
# Stats
# ----------------------------
success = 0
skipped = 0
failed = []
skip_reasons = []


def skip(msg):
    global skipped
    print(f"\nSkipping: {msg}")
    skipped += 1


# ----------------------------
# Main loop
# ----------------------------
for d in build_dir.iterdir():
    if not d.is_dir():
        continue

    print(f"\nParsing: {d.name}")

    build = parse_folder(d)
    if not build:
        skip("Unrecognized directory")
        continue

    if build.platform != "win" and build.config != "release":
        skip(f"Non-release config: {build.config}")
        continue

    binary = get_binary(build)

    if not binary.exists():
        failed.append(binary)
        skip(f"Missing binary: {binary.name}")
        continue

    zip_name = build.filename(args.version)
    zip_path = output_dir / zip_name

    file_hash = hash_file(binary)

    if zip_path.exists() and hash_db.get(zip_name) == file_hash:
        skip("Already packaged (hash match)")
        continue

    try:
        zip_binary(binary, zip_path)
        hash_db[zip_name] = file_hash
        success += 1
        print(f"Packaged: {zip_path}")
    except Exception as ex:
        failed.append(binary)
        print(f"ZIP failed: {ex}", file=sys.stderr)


# ----------------------------
# Save hash DB once
# ----------------------------
try:
    output_dir.mkdir(parents=True, exist_ok=True)
    json_path.write_text(json.dumps(hash_db, indent=2))
except Exception as ex:
    print(f"Failed writing hash DB: {ex}", file=sys.stderr)


# ----------------------------
# Summary
# ----------------------------
print(
    f"\nDone: {success} success, {skipped} skipped, {len(failed)} failed"
)

if failed:
    print("\nFailed or Missing Binaries:")
    for f in failed:
        print(f" - {f}")