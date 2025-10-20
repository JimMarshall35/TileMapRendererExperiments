#
#   Serialize entity types for the game from the tiled json file
#   plugs into and extends engine/scripts/ConvertTiled.py
#

import sys
import os
import struct

print(os.path.abspath("../Stardew/engine/scripts"))
sys.path.insert(1, os.path.abspath("../Stardew/engine/scripts"))  # add Folder_2 path to search list
from ConvertTiled import main, register_entity_serializer, get_tiled_object_custom_prop

def serialize_WoodedArea(file, obj):
    file.write(struct.pack("I", 1)) # version
    file.write(struct.pack("f", get_tiled_object_custom_prop(obj, "ConiferousPercentage")["value"]))
    file.write(struct.pack("f", get_tiled_object_custom_prop(obj, "DeciduousPercentage")["value"]))
    file.write(struct.pack("f", get_tiled_object_custom_prop(obj, "PerMeterDensity")["value"]))
    file.write(struct.pack("f", obj["width"]))
    file.write(struct.pack("f", obj["height"]))

def get_type_WoodedArea(obj):
    return 6

register_entity_serializer("WoodedArea", serialize_WoodedArea, get_type_WoodedArea, False)

main()