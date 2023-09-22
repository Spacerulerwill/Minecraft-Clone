"""
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
"""

import yaml
import os
from PIL import Image

TEXTURE_SIZE = 16
MAX_ANIMATION_FRAMES = 32

def round_next_pow_2(num: int):
    num -= 1
    num|= num >> 1
    num |= num >> 2
    num |= num >> 4
    num |= num >> 8
    num |= num >> 16
    num += 1
    return num


def atlas_make():
    BASE_DIR = os.path.join( os.path.dirname( __file__ ), '..', '..')
    yml_location = os.path.join(BASE_DIR, "res", "blocks.yml")

    images_location = os.path.join(BASE_DIR, "res", "textures", "block")

    block_images = {}
    image_count = 0
    for filename in os.scandir(images_location):
        if filename.is_file():
            image_count += 1
            block_images[os.path.splitext(filename.name)[0]] = Image.open(os.path.join(images_location, filename.name))

    
    # Create MAX_ANIMATION_FRAMES amount of blank texture atlases
    atlases = [Image.new("RGBA", (TEXTURE_SIZE, image_count * TEXTURE_SIZE), (0,0,0,0)) for i in range(MAX_ANIMATION_FRAMES)]

    with open(yml_location, 'r') as file:
        images_added = 0
        blocks_yml:dict = yaml.safe_load(file)
        for block, block_data in blocks_yml.items():
            unique_faces:int = block_data["unique_faces"]

            match unique_faces:
                case 1:
                    try:
                        frames = block_data["frames"]
                        block_texture:Image.Image = block_images[block]

                        if isinstance(frames, int):
                            if frames > MAX_ANIMATION_FRAMES:
                                raise ValueError(f"Cannot have more than {MAX_ANIMATION_FRAMES} frames. {frames} is too many!")
                            
                            frames_padded = round_next_pow_2(frames)
                            num_repeats = int(MAX_ANIMATION_FRAMES / frames_padded)

                            index = 0
                            for i in range(num_repeats):
                                for f in range(frames):
                                    block_texture_copy = block_texture.copy()
                                    atlases[index].paste(
                                        block_texture_copy.crop((0, f * TEXTURE_SIZE, TEXTURE_SIZE, (f + 1) * TEXTURE_SIZE)),
                                        (0, TEXTURE_SIZE * images_added)
                                    )
                                    index += 1

                                for f in range(frames_padded - frames):
                                    block_texture_copy = block_texture.copy()
                                    atlases[index].paste(
                                        block_texture_copy.crop((0, (frames) * TEXTURE_SIZE, TEXTURE_SIZE, (frames + 1) * TEXTURE_SIZE)),
                                        (0, TEXTURE_SIZE * images_added)
                                    )
                                    index += 1
                            images_added += 1

                        elif isinstance(frames, list):
                            if len(frames) > MAX_ANIMATION_FRAMES:
                                raise ValueError(f"Cannot have more than {MAX_ANIMATION_FRAMES} frames. {frames} is too many!")
                            
                    except KeyError:
                        block_texture:Image.Image = block_images[block]
                        for i in range(MAX_ANIMATION_FRAMES):
                            atlases[i].paste(block_texture, (0, TEXTURE_SIZE * images_added))
                        images_added += 1
                
                case 2:
                    for i in range(MAX_ANIMATION_FRAMES):
                        for count, ext in enumerate(["_vertical", "_side"]):
                            block_texture = block_images[block + ext]
                            atlases[i].paste(block_texture, (0, TEXTURE_SIZE * (images_added + count)))
                    images_added += 2
                case 3:
                    for i in range(MAX_ANIMATION_FRAMES):
                        for count, ext in enumerate(["_top", "_bottom", "_side"]):
                            block_texture = block_images[block + ext]
                            atlases[i].paste(block_texture, (0, TEXTURE_SIZE * (images_added + count)))
                    images_added += 3
                case 6:
                    for i in range(MAX_ANIMATION_FRAMES):
                        for count, ext in enumerate(["_top", "_bottom", "_left", "_right", "_front", "_back"]):
                            block_texture = block_images[block + ext]
                            atlases[i].paste(block_texture, (0, TEXTURE_SIZE * (images_added + count)))
                    images_added += 6


        for i in range(MAX_ANIMATION_FRAMES):
            atlases[i].save(os.path.join("res", "textures", "atlases", f"atlas{i}.png"))

    """
    BASE_DIR = os.path.join( os.path.dirname( __file__ ), '..', '..')
    yml_location = os.path.join(BASE_DIR, "res", "blocks.yml")
    output_location = os.path.join(BASE_DIR, "res", "textures", "atlas.png")

    image_locations = os.path.join(BASE_DIR, "res", "textures", "block")
    _, _, files = next(os.walk(image_locations))
    image_count = len(files)

    with open(yml_location, 'r') as file:
        blocks_yml:dict = yaml.safe_load(file)

        img = Image.new('RGBA', (TEXTURE_SIZE, image_count * TEXTURE_SIZE), (0, 0, 0, 0))

        images_added = 0
        for block, block_data in blocks_yml.items():
            unique_faces:int = block_data["unique_faces"]
            match unique_faces:
                case 1:
                    texture_path = os.path.join(image_locations, f"{block}.png")
                    texture_img = Image.open(texture_path)
                    img.paste(texture_img, (0, TEXTURE_SIZE * images_added))
                    images_added += 1
                case 2:
                     for ext in ["_vertical", "_side"]:
                        texture_path = os.path.join(image_locations, f"{block}{ext}.png")
                        texture_img = Image.open(texture_path)
                        img.paste(texture_img, (0, TEXTURE_SIZE * images_added))
                        images_added += 1
                case 3:
                    for ext in ["_top", "_bottom", "_side"]:
                        texture_path = os.path.join(image_locations, f"{block}{ext}.png")
                        texture_img = Image.open(texture_path)
                        img.paste(texture_img, (0, TEXTURE_SIZE * images_added))
                        images_added += 1
                case 6:
                    for ext in ["_top", "_bottom", "_left", "_right", "_front", "_back"]:
                        texture_path = os.path.join(image_locations, f"{block}{ext}.png")
                        texture_img = Image.open(texture_path)
                        img.paste(texture_img, (0, TEXTURE_SIZE * images_added))
                        images_added += 1

        img.save(output_location)
        """

if __name__ == "__main__":
    atlas_make()

"""
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
