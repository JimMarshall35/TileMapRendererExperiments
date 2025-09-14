import argparse
import json
import xml.etree.ElementTree as ET
import typing
import os


TILEMAP_FILE_VERSION = 1

OBJECTS_FILE_VERSION = 1

ATLAS_FILE_VERSION = 1

def do_cmd_args():
    parser = argparse.ArgumentParser(
                    prog='ConvertTiled',
                    description='convert data files from the level editor program "tiled" into our game engine format',
                    epilog='Jim Marshall - 2025')
    parser.add_argument('-m', '--tile_maps', nargs='+', default=[])
    parser.add_argument("outputDir", type=str, help="the output directory")
    args = parser.parse_args()
    return args

def find_tileset(index: int, tilesets: object) -> object:
    for t in tilesets:
        if index >= t["firstgid"]:
            return t

def get_normalized_index(index: int, tileset: object) -> int:
    return (index - tileset["firstgid"]) + 1

def get_tiles_used_per_tileset(parsed_tile_maps) -> dict[str, set[int]]:
    used_tiles = dict()
    for p in parsed_tile_maps:
        tilesets = p["tilesets"]
        tilesets = sorted(tilesets, key=lambda tileset: tileset["firstgid"])   # sort by age
        tilesets.reverse()
        for t in tilesets:
            t["tiles"] = set()
        for layer in p["layers"]:
            if "data" in layer:
                for index in layer["data"]:
                    tileset = find_tileset(index, tilesets)
                    if tileset:
                        tileset["tiles"].add(get_normalized_index(index, tileset))
        for t in tilesets:
            if t["source"] in used_tiles:
                used_tiles[t["source"]] = used_tiles[t["source"]].union(t["tiles"])
            else:
                used_tiles[t["source"]] = t["tiles"]
    return used_tiles

class AtlasSprite:
    def __init__(self, path, top, left, width, height, name):
        self.path = path
        self.top = top
        self.left = left
        self.width = width
        self.height = height
        self.name = name
    def get_attributes(self, counter):
        return {
            "source" : self.path,
            "top" : str(int(self.top)),
            "left" : str(int(self.left)),
            "width" : str(int(self.width)),
            "height" : str(int(self.height)),
            "name" : f"{self.name}_{counter}"
        }

class Atlas:
    sprites: list[AtlasSprite]
    def get_atlas_index(self, normalized_index, path):
        pass
    def output_atlas_xml_file(self, outPath):
        counter = 0
        top = ET.Element("atlas")
        tree = ET.ElementTree(top)
        for s in self.sprites:
            ET.SubElement(top, "sprite", s.get_attributes(counter))
            counter += 1

        ET.indent(tree, space=" ", level=3)
        with open(outPath, "wb") as f:
            f.write(ET.tostring(top))
            #tree.write(f)
    def make_sprites(self):
        for key in self.originalNormalizedIndexes:
            with open(key, "r") as f:
                j = json.load(f)
                imgWidth = j["imagewidth"]
                imgHeight = j["imageheight"]
                tileHeight = j["tileheight"]
                tileWidth = j["tilewidth"]
                margin = j["margin"]
                spacing = j["spacing"]
                columns = j["columns"]
                name = j["name"]
                image = j["image"]
                for index in self.originalNormalizedIndexes[key]:
                    l = margin + (((index - 1) % columns) * tileWidth  + spacing)
                    t = margin + (((index - 1) / columns) * tileHeight + spacing)
                    self.sprites.append(AtlasSprite(image, t, l, tileWidth, tileHeight, name))
    def __init__(self, originalNormalizedIndexes : dict[str, set[int]]):
        self.originalNormalizedIndexes = originalNormalizedIndexes
        self.sprites = []
        self.make_sprites()
        pass

def convert_tile_maps(args):
    "convert tile map files to a set of three files, an atlas xml file, an game objects xml file, and a tilemap binary file"
    parsed_tile_maps = []
    for p in args.tile_maps:
        print(p)
        with open(p, "r") as f:
            parsed_tile_maps.append(json.load(f))
    used : dict[str, set[int]] = get_tiles_used_per_tileset(parsed_tile_maps)
    print(used)
    atlas = Atlas(used)
    atlas.output_atlas_xml_file(os.path.join(os.path.abspath(args.outputDir), "atlas.xml"))

def main():
    args = do_cmd_args()
    convert_tile_maps(args)
main()