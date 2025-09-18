python engine/scripts/ConvertTiled.py ./Assets/out_rle --rle true -m ./Assets/Farm.json ./Assets/House.json ./Assets/RoadToTown.json -a engine/scripts/AtlasTool.exe

python engine/scripts/ConvertTiled.py ./Assets/out -m ./Assets/Farm.json ./Assets/House.json ./Assets/RoadToTown.json -a engine/scripts/AtlasTool.exe


"engine/scripts/AtlasTool.exe" ./Assets/ui_atlas.xml -o ./Assets/ui_atlas.atlas
pause