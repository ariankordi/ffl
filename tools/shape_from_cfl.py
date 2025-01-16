# NOTE if you are reading: if it's not obvious, this is AI slop
# it is designed to take an output folder from THIS: https://github.com/B3n30/citra_system_archives/blob/master/mii/mii.py
# and create an accompanying _shape folder for FFLResource.py
# unfortunately this cannot do textures yet, the dimensions are not the exact same

import os
import sys
import trimesh
import json
from collections import defaultdict

def load_redirects(cfl_dir):
    """
    Loads redirect mappings from all redirect.txt files.
    Returns a dictionary {folder: {source_id: target_id}}.
    """
    redirects = defaultdict(dict)
    for folder in range(9):
        redirect_file = os.path.join(cfl_dir, str(folder), "redirect.txt")
        if os.path.exists(redirect_file):
            with open(redirect_file, 'r') as file:
                for line in file:
                    try:
                        source, target = map(str.strip, line.split('->'))
                        redirects[folder][int(source)] = int(target)
                    except ValueError:
                        print(f"Malformed redirect line in {redirect_file}: {line.strip()}")
    return redirects

def get_ffl_prefix_and_index(folder, obj_index):
    """
    Determines the FFL prefix and adjusted index based on the folder and index.
    Alternates between Cap and Normal when needed.
    """
    FFL_TO_CFL = {
        0: "Beard",
        1: "HatNormal",
        2: "Faceline",
        3: "ForeheadNormal",
        4: "Glass",
        5: "HairNormal",
        6: "Mask",
        7: "Noseline",
        8: "Nose"
    }

    # Alternating Cap/Normal logic
    if folder in {1, 3, 5}:
        if obj_index % 2 == 0:
            return FFL_TO_CFL[folder], obj_index // 2
        else:
            return FFL_TO_CFL[folder].replace("Normal", "Cap"), obj_index // 2
    else:
        return FFL_TO_CFL[folder], obj_index

def parse_extra_txt(extra_txt_path, folder):
    """
    Parses the CFL _extra.txt file and extracts transformation data.
    Ensures 6 vectors in the correct order with default fallback if missing.
    """
    if os.path.getsize(extra_txt_path) == 0:
        print(f"WARNING: {extra_txt_path} is empty. Using zeroed transform.")
        return [[0.0, 0.0, 0.0] for _ in range(6)]  # Default to 6 zero vectors
    """
    if (index == 0x40)
    {
        (model->partsTransform).headFrontRotate[0] = 0.34906584;
        (model->partsTransform).headFrontRotate[1] = 0.0;
        (model->partsTransform).headFrontRotate[2] = 0.0;
        (model->partsTransform).headTopRotate[0] = -0.43633232;
        (model->partsTransform).headTopRotate[1] = 0.0;
        (model->partsTransform).headTopRotate[2] = 0.0;
    }
    """
    transform_data = {
        "hair": [0.0, 0.0, 0.0],
        "nose": [0.0, 0.0, 0.0],
        "beard": [0.0, 0.0, 0.0],
        "hat_a_position": [0.0, 0.0, 0.0],
        "hat_a_angle": [0.0, 0.0, 0.0],
        "hat_b_position": [0.0, 0.0, 0.0],
        "hat_b_angle": [0.0, 0.0, 0.0],
        "hat_c_position": [0.0, 0.0, 0.0],
        "hat_c_angle": [0.0, 0.0, 0.0]
    }

    try:
        with open(extra_txt_path, 'r') as file:
            for line in file:
                if '=' in line:
                    key, values = line.split('=')
                    values = [float(v) for v in values.split(',')]
                    if key.strip() in transform_data:
                        transform_data[key.strip()] = values
    except Exception as e:
        print(f"Error reading transform data from {extra_txt_path}: {e}")

    # Return correctly ordered transform vectors for each folder type
    if folder == 2:  # Faceline, Hair, Nose, Beard
        return [
            transform_data["hair"],
            transform_data["nose"],
            transform_data["beard"],
            [0.0, 0.0, 0.0],  # Placeholder to ensure 6 vectors
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0]
        ]
    elif folder == 5:  # Hat folder (A, B, C ordering)
        return [
            transform_data["hat_a_position"],
            transform_data["hat_a_angle"],
            transform_data["hat_b_position"],
            transform_data["hat_b_angle"],
            transform_data["hat_c_position"],
            transform_data["hat_c_angle"]
        ]
    else:
        # Fallback if no structured order is defined
        return [[0.0, 0.0, 0.0] for _ in range(6)]


def generate_ffl_json(ffl_path, transform_data):
    """
    Generates the JSON structure for the FFL file.
    """
    json_data = {
        "partsInfo": {
            "compressLevel": 3,
            "memoryLevel": 5,
            "strategy": 0,
            "windowBits": 4
        },
        "shape": {
            "filename": os.path.basename(ffl_path),
            "transform": transform_data
        }
    }
    json_path = f"{ffl_path.replace('.glb', '.json')}"
    try:
        with open(json_path, 'w') as json_file:
            json.dump(json_data, json_file, indent=4)
        print(f"Generated JSON: {json_path}")
    except Exception as e:
        print(f"Error writing JSON for {ffl_path}: {e}")

def convert_obj_to_glb(obj_file, output_file):
    """
    Converts an .obj file to .glb using trimesh.
    """
    try:
        mesh = trimesh.load(obj_file, force='mesh')
        mesh.export(output_file)
        print(f"Converted: {obj_file} -> {output_file}")
    except Exception as e:
        print(f"Error converting {obj_file}: {e}")

def main(cfl_dir, ffl_dir):
    if not os.path.exists(ffl_dir):
        os.makedirs(ffl_dir)

    # Load redirect mappings once
    redirects = load_redirects(cfl_dir)

    # Iterate through all CFL folders (0-8)
    for folder in range(9):
        folder_path = os.path.join(cfl_dir, str(folder))
        if not os.path.exists(folder_path):
            continue

        print(f"\nProcessing CFL folder {folder}...")

        # Collect all files AND redirected entries
        obj_indices = set()

        # Add actual .obj files
        for obj_file in os.listdir(folder_path):
            if obj_file.endswith('.obj'):
                obj_indices.add(int(obj_file.split('.')[0]))

        # Add redirect "virtual" files to the set
        for source, target in redirects[folder].items():
            obj_indices.add(source)

        # Loop through all collected indices
        for obj_index in sorted(obj_indices):
            # Resolve redirects recursively
            final_index = obj_index
            seen_indices = set()
            while final_index in redirects[folder]:
                if final_index in seen_indices:
                    print(f"ERROR: Circular redirect detected in folder {folder}!")
                    break
                seen_indices.add(final_index)
                final_index = redirects[folder][final_index]

            # Determine the source file (real or redirected)
            final_file = os.path.join(folder_path, f"{final_index}.obj")
            extra_file = os.path.join(folder_path, f"{final_index}_extra.txt")

            if not os.path.isfile(final_file):
                print(f"WARNING: {final_file} does not exist. Skipping...")
                continue
            if os.path.getsize(final_file) == 0:  # Skip if the file is empty
                print(f"WARNING: {final_file} is empty. Skipping...")
                continue

            # Determine the FFL name and index
            ffl_prefix, ffl_index = get_ffl_prefix_and_index(folder, obj_index)
            ffl_output_file = os.path.join(ffl_dir, f"{ffl_prefix}_{ffl_index}.glb")

            # Convert OBJ to GLB
            convert_obj_to_glb(final_file, ffl_output_file)

            # Generate JSON with transform data or fallback to empty
            if os.path.isfile(extra_file):
                transform_data = parse_extra_txt(extra_file, folder)
            else:
                print(f"WARNING: No transform data found for {final_file}. Using empty transform.")
                transform_data = [[0.0, 0.0, 0.0] for _ in range(6)]

            # Generate the FFL JSON file (empty if no transform data available)
            generate_ffl_json(ffl_output_file, transform_data)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(
            f"Usage: python {sys.argv[0]} <cfl_input_dir> <ffl_output_dir>\n\n"
            "Description:\n"
            "    - The <cfl_input_dir> should be the extracted output folder generated by:\n"
            "      https://github.com/B3n30/citra_system_archives/blob/master/mii/mii.py\n"
            "    - The <ffl_output_dir> will be a _shape directory compatible with:\n"
            "      FFLResource.py from the FFL decomp by aboood400091 (ariankordi fork: https://github.com/ariankordi/ffl/blob/nsmbu-win-port-linux64/tools/FFLResource.py).\n\n"
            "This script converts .obj models from CFL archives into .glb models and generates\n"
            "accompanying .json files to use with FFLResource.py.\n"
        )
        sys.exit(1)

    cfl_input_dir = sys.argv[1]
    ffl_output_dir = sys.argv[2]

    main(cfl_input_dir, ffl_output_dir)
