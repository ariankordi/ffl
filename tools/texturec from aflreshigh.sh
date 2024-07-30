#!/bin/bash

# NOTE NOTE: YOU HAVE TO CHANGE ALL THESE!!!!

# https://github.com/bkaradzic/bimg/tree/master/tools/texturec
texturec_bin="/home/arian/Downloads/build/bimg/.build/linux64_gcc/bin/texturecRelease"

# Directory containing the input textures

# https://github.com/jaames/mii-assets/tree/master
# USING AFLRESHIGH, NOT AFLRESHIGH_2_3!!!!!!!!!!!!
input_dir="/home/arian/Downloads/big/mii-assets/aflreshigh_tex"

output_dir="/home/arian/Downloads/ffl/tools/FFLResHigh_texture"

function call_texturec {
    for i in $(seq 0 $3); do
        $texturec_bin -f "$input_dir"/tex_$(($1 + $i)).png -o $output_dir/$2_$i.dds -t $4 -m
    done
}

# offsets from AFLResHigh, extracted by mii-assets with texture count of 364!!!!!

call_texturec 0 Beard 2 R8

# Loop over all files in the input directory
for file in "$input_dir"/tex_*.png; do
    # Extract the numeric part of the filename
    file_name=$(basename "$file")
    tex_num=$(echo "$file_name" | grep -oP '\d+')
    
    # Check if the tex_num is within the range 12 to 133
    if [ "$tex_num" -ge 12 ] && [ "$tex_num" -le 133 ]; then
        # Calculate the output texture number
        output_tex_num=$((tex_num - 3))
        
        # Construct the output filename
        output_file="$output_dir/Cap_$output_tex_num.dds"
        
        # Run the texturecRelease command
        $texturec_bin -f "$file" -o "$output_file" -t R8 -m
    fi
done


call_texturec 135 Eye 61 RGBA8
call_texturec 215 Eyebrow 23 R8
call_texturec 243 Faceline 11 R8
call_texturec 255 FaceMake 11 RGBA8
# - NO GLASS!
# dds for all glass types were done by hand
# think the range for AFLResHigh_2_3's glass types are 267 and there's 20
# all converted by gimp L8A8 then flipped endian of all bytes 0x80 and onwards
call_texturec 276 Mole 1 R8
call_texturec 278 Mouth 36 RGBA8
call_texturec 330 Mustache 5 R8
call_texturec 336 Noseline 17 R8

