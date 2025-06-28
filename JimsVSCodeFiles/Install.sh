

echo installing .vscode folder...

if [ ! -d "../.vscode" ];
then
  echo folder not found, creating it
  mkdir ../.vscode
else
  echo folder already exists
fi

for filename in ./* 
do
    if [ "$filename" != "./Install.sh" ]
    then
        echo copying file $filename
        cp "$filename" ../.vscode/${filename}
    fi
done