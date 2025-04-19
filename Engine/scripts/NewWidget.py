import re

header_template_file_path = 'WidgetHeaderTemplate.txt'
srcFilePath = 'WidgetSrcTemplate.txt'

header_template_str = ""

src_template_str = ""

single_line_template_var_names_regex = r'<<[a-zA-Z]*>>'

multi_line_template_var_names_regex = r'<\[[a-zA-Z]*\]>'

with open(header_template_file_path, 'r') as file:
    header_template_str = file.read()

with open(srcFilePath, 'r') as file:
    src_template_str = file.read()


headerSingleLineNames = re.findall(single_line_template_var_names_regex, header_template_str)
headerMultiLineNames = re.findall(multi_line_template_var_names_regex, header_template_str)

srcSingleLineNames = re.findall(single_line_template_var_names_regex, src_template_str)
srcMultiLineNames = re.findall(multi_line_template_var_names_regex, src_template_str)

single_line_names_set = set()
multi_line_names_set = set()

header_file_name = "unknown.h"
src_file_name = "unknown.c"


for val in headerSingleLineNames:
    single_line_names_set.add(val)
    
for val in srcSingleLineNames:
    single_line_names_set.add(val)


for val in headerMultiLineNames:
    multi_line_names_set.add(val)
    
for val in srcMultiLineNames:
    multi_line_names_set.add(val)

if not "<<WidgetName>>" in single_line_names_set:
    print("<<WidgetName>> is required")
    quit()


header_template_str_cpy = header_template_str
src_template_str_cpy = src_template_str

for val in single_line_names_set:       
    if val == "<<IncludeGuardDef>>":
        continue
    userVal = input(f"Enter value for {val}")
    if val == "<<WidgetName>>":
        header_file_name = f"{userVal}.h"
        src_file_name = f"{userVal}.c"
        include_guard_def_name = userVal.upper() + "_H"
        header_template_str_cpy = header_template_str_cpy.replace("<<IncludeGuardDef>>", include_guard_def_name)
        src_template_str_cpy = src_template_str_cpy.replace("<<IncludeGuardDef>>", include_guard_def_name)
        
    header_template_str_cpy = header_template_str_cpy.replace(val, userVal)
    src_template_str_cpy = src_template_str_cpy.replace(val, userVal)

for val in multi_line_names_set:
    bContinue = True
    replacementVal = ""
    while(bContinue):
        userInput = input(f"mutli line entry {val}. Enter a line and then a blank line to finish:")
        if userInput == "":
            break
        userInput += "\r\n"
        replacementVal += userInput
    header_template_str_cpy = header_template_str_cpy.replace(val, replacementVal)
    src_template_str_cpy = src_template_str_cpy.replace(val, replacementVal)
    
with open(header_file_name, "w") as text_file:
    text_file.write(header_template_str_cpy)
    
with open(src_file_name, "w") as text_file:
    text_file.write(src_template_str_cpy)

















    
