import os
import sys
from PIL import Image

def main():
    folder = os.path.dirname(os.path.abspath(sys.argv[0]))
    if len(sys.argv) > 1:
        folder = sys.argv[1]

    pngs = sorted([f for f in os.listdir(folder) if f.lower().endswith('.png') and not f.startswith('.')])
    if not pngs:
        print("No PNG files found.")
        input("Press Enter to exit...")
        return

    images = [Image.open(os.path.join(folder, f)) for f in pngs]
    total_width = sum(img.width for img in images)
    max_height = max(img.height for img in images)

    result = Image.new('RGBA', (total_width, max_height))
    x = 0
    for img in images:
        result.paste(img, (x, 0))
        x += img.width

    folder_name = os.path.basename(folder)
    output_path = os.path.join(folder, f"{folder_name}.png")
    result.save(output_path)
    print(f"Done: {output_path} ({len(pngs)} images, {total_width}x{max_height})")
    input("Press Enter to exit...")

if __name__ == '__main__':
    main()
