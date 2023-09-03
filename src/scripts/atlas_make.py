"""
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
"""

import yaml
import os
from PIL import Image

def atlas_make():
    BASE_DIR = os.path.join( os.path.dirname( __file__ ), '..', '..')
    yml_location = os.path.join(BASE_DIR, "res", "blocks.yml")
    output_location = os.path.join(BASE_DIR, "res", "textures", "atlas.png")

    image_locations = os.path.join(BASE_DIR, "res", "textures", "block")
    _, _, files = next(os.walk(image_locations))
    image_count = len(files)

    with open(yml_location, 'r') as file:
        blocks_yml:dict = yaml.safe_load(file)
        img = Image.new('RGBA', (16, image_count * 16), (0, 0, 0, 0))

        images_added = 0
        for block, block_data in blocks_yml.items():
            unique_faces:int = block_data["unique_faces"]
            match unique_faces:
                case 1:
                    texture_path = os.path.join(image_locations, f"{block}.png")
                    texture_img = Image.open(texture_path)
                    img.paste(texture_img, (0, 16 * images_added))
                    images_added += 1
                case 2:
                     for ext in ["_vertical", "_side"]:
                        texture_path = os.path.join(image_locations, f"{block}{ext}.png")
                        texture_img = Image.open(texture_path)
                        img.paste(texture_img, (0, 16 * images_added))
                        images_added += 1
                case 3:
                    for ext in ["_top", "_bottom", "_side"]:
                        texture_path = os.path.join(image_locations, f"{block}{ext}.png")
                        texture_img = Image.open(texture_path)
                        img.paste(texture_img, (0, 16 * images_added))
                        images_added += 1
                case 6:
                    for ext in ["_top", "_bottom", "_left", "_right", "_front", "_back"]:
                        texture_path = os.path.join(image_locations, f"{block}{ext}.png")
                        texture_img = Image.open(texture_path)
                        img.paste(texture_img, (0, 16 * images_added))
                        images_added += 1

        img.save(output_location)

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