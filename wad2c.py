# wad2c.py
import sys
#WADファイルをCソースに書き直します。
#入手先：
#https://www.doomworld.com/idgames/idstuff/doom/win95/doom95?utm_source=chatgpt.com

data = open("./DOOM1.WAD", "rb").read()
with open("src/doomgeneric/wad_data.c", "w") as f:
    f.write("#include <stdint.h>\n#include <stddef.h>\n")
    f.write("const unsigned char doom_wad_data[] __attribute__((aligned(4))) = {\n")
    for i in range(0, len(data), 24):
        f.write(",".join(str(b) for b in data[i:i+24]) + ",\n")
    f.write("};\n")
    f.write(f"const size_t doom_wad_len = {len(data)};\n")