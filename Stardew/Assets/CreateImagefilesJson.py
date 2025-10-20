import json
import glob, os

imgFiles = {"ImageFileRegistry" : []}
for root, dirs, files in os.walk("Image"):
    for file in files:
        if file.endswith(".png"):
            fullPath = os.path.join(root, file)
            #print(fullPath)
            imgFiles["ImageFileRegistry"].append(fullPath)

strRes = json.dumps(imgFiles, indent=4)

with open("ImageFiles.json", "w") as text_file:
    text_file.write(strRes)
    
print(strRes)
