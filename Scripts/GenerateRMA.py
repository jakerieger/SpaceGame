'''
This script takes three grayscale texture maps for Roughness, Metallic, and Ambient Occlusion and
combines them in to one image with each map occupying one of the color channels (R, G, or B).
'''

from PIL import Image
import sys

def combine_rma(roughness_map, metallic_map, ao_map, output_path):
    roughness = Image.open(roughness_map).convert("L")
    metallic = Image.open(metallic_map).convert("L")
    ao = Image.open(ao_map).convert("L")
    
    if roughness.size != metallic.size or roughness.size != ao.size:
        raise ValueError("All input images must have the same dimensions!")
    
    combined = Image.merge("RGB", (roughness, metallic, ao))
    combined.save(output_path)
    print(f"Combined texture saved to {output_path}")

    return


args = sys.argv

if len(args) != 5:
    raise ValueError("Incorrect number of arguments specified")

combine_rma(args[1], args[2], args[3], args[4])