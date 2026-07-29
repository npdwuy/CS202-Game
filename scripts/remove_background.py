import os
import argparse
from PIL import Image

def get_color_distance(c1, c2):
    # Calculate Euclidean distance between two RGB colors
    return ((c1[0] - c2[0])**2 + (c1[1] - c2[1])**2 + (c1[2] - c2[2])**2)**0.5

def remove_background(input_path, output_path, tolerance=10):
    if not os.path.exists(input_path):
        print(f"Error: Input file {input_path} does not exist.")
        return False

    print(f"Loading image: {input_path}")
    img = Image.open(input_path).convert("RGBA")
    width, height = img.size
    pixels = list(img.getdata())

    # Define background and box fill colors to remove (RGB)
    bg_colors = [
        (105, 79, 22),   # Brown background (Super Mario side)
        (45, 111, 131),  # Teal background (Fire Mario side)
        (175, 131, 37),  # Gold/brown title/box fill
        (61, 152, 138),  # Medium green/teal box fill
        (122, 190, 187), # Light teal box fill
        (88, 199, 207),  # Peach blue background (bottom left)
        (62, 147, 202),  # Medium-light blue background (bottom middle/right)
        (55, 48, 23),    # Bottom dark brown credits background
        (245, 137, 216), # Pink highlight box fill
        (186, 232, 155), # Light green highlight box fill
    ]

    print("Removing background and box fill colors...")
    new_pixels = []
    removed_count = 0
    for p in pixels:
        r, g, b, a = p
        
        # Check if the color matches any background color within tolerance
        matched = False
        for bg in bg_colors:
            if get_color_distance((r, g, b), bg) <= tolerance:
                matched = True
                break
        
        if matched:
            new_pixels.append((0, 0, 0, 0))
            removed_count += 1
        else:
            new_pixels.append(p)

    print(f"Removed {removed_count} background pixels.")

    # Save output image directly
    out_img = Image.new("RGBA", (width, height))
    out_img.putdata(new_pixels)
    out_img.save(output_path)
    print(f"Saved processed image to: {output_path}")
    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Remove background and box fill colors from sprite sheets.")
    parser.add_argument("-i", "--input", default="assets/sprites/player/mario_2.png", help="Input image path")
    parser.add_argument("-o", "--output", default="assets/sprites/player/mario_2_transparent.png", help="Output image path")
    parser.add_argument("-t", "--tolerance", type=int, default=10, help="Color distance tolerance")
    
    args = parser.parse_args()
    remove_background(args.input, args.output, args.tolerance)
