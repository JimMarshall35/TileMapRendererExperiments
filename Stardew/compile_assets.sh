python3 game/game_convert_tiled.py ./Assets/out_rle --rle true -m ./Assets/Farm.json ./Assets/House.json ./Assets/RoadToTown.json -a engine/scripts/AtlasTool.exe

python3 game/game_convert_tiled.py ./Assets/out -m ./Assets/Farm.json ./Assets/House.json ./Assets/RoadToTown.json -a ./build/atlastool/AtlasTool -bmp Atlas.bmp -iw 128 -ih 128


./build/atlastool/AtlasTool ./Assets/ui_atlas.xml -o ./Assets/ui_atlas.atlas -bmp UIAtlas.bmp

# really the python tool should do this but it doesn't work - cba to fix it right now - this is just as good - the same thing
./build/atlastool/AtlasTool ./Assets/out/atlas.xml -o ./Assets/out/main.atlas -bmp Atlas.bmp
./build/atlastool/AtlasTool ./Assets/out/atlas.xml -o ./Assets/out_rle/main.atlas
