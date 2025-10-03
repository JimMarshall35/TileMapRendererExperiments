python3 engine/scripts/ConvertTiled.py ./Assets/out_rle --rle true -m ./Assets/Farm.json ./Assets/House.json ./Assets/RoadToTown.json -a engine/scripts/AtlasTool.exe

python3 engine/scripts/ConvertTiled.py ./Assets/out -m ./Assets/Farm.json ./Assets/House.json ./Assets/RoadToTown.json -a ./build/atlastool/AtlasTool -bmp Atlas.bmp -iw 128 -ih 128


./build/atlastool/AtlasTool ./Assets/ui_atlas.xml -o ./Assets/ui_atlas.atlas -bmp UIAtlas.bmp -iw 128 -ih 128
