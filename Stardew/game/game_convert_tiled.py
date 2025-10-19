import sys
import os
print(os.path.abspath("../Stardew/engine/scripts"))
sys.path.insert(1, os.path.abspath("../Stardew/engine/scripts"))  # add Folder_2 path to search list
import ConvertTiled

ConvertTiled.main()