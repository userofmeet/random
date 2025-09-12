import os
from PIL import Image

input_folder = r"C:\Users\djain\Downloads\maz_rendered"
output_folder = r"C:\Users\djain\Downloads\maz_rendered"

os.makedirs(output_folder, exist_ok=True)
tif_files = [f for f in os.listdir(input_folder) if f.lower().endswith(".tif")]

tif_files.sort()

for i, file_name in enumerate(tif_files, start=1):
    file_path = os.path.join(input_folder, file_name)
    
    img = Image.open(file_path)
    output_path = os.path.join(output_folder, f"{i}.png")
    img.save(output_path, "PNG")

    print(f"Saved {output_path}")

print("Conversion complete!")
