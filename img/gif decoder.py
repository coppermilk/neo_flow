from PIL import Image
import os

def gif_to_pixel_arrays(gif_path):
    # Open the GIF image
    gif = Image.open(gif_path)

    # Get the number of frames in the GIF
    num_frames = gif.n_frames

    # Get the duration of each frame in milliseconds
    duration = gif.info['duration']

    # Create a list to store arrays of pixels for each frame
    pixel_arrays = []

    # Iterate through each frame and create a two-dimensional array for it
    for frame_num in range(num_frames):
        # Select the current frame
        gif.seek(frame_num)
        
        # Convert the frame to RGB format (if not already in this format)
        current_frame = gif.convert('RGB')
        
        # Get the dimensions of the image
        width, height = current_frame.size
        
        # Create a two-dimensional array for the current frame
        pixel_array = []
        for y in range(height):
            row = []
            for x in range(width):
                # Get the color of the pixel in RGB
                pixel_color = current_frame.getpixel((x, y))
                # Add the pixel color to the array
                row.append(pixel_color)
            # Add the row of pixels to the array for the current frame
            pixel_array.append(row)
        
        # Add the array of pixels for the current frame to the list of all frames
        pixel_arrays.append(pixel_array)
    
    return pixel_arrays, num_frames, duration

def extract_sub_arrays(pixel_arrays):
    sub_arrays = []
    for pixel_array in pixel_arrays:
        # Create a new array of size 8x8
        sub_array = []
        for i in range(5, 45, 5):  # 5 + 5*n, where n changes from 0 to 7
            row = []
            for j in range(5, 45, 5):
                # Extract every fifth element
                pixel = pixel_array[i][j]
                row.append(pixel)
            sub_array.append(row)
        sub_arrays.append(sub_array)
    return sub_arrays

def create_sprite(pixel_arrays, num_frames, duration, output_path, gif_name):
    # Determine the dimensions of the sprite
    sprite_width = 8 * num_frames
    sprite_height = 8
    
    # Create a new image for the sprite
    sprite = Image.new('RGB', (sprite_width, sprite_height))

    # Get the image editing object
    pixels = sprite.load()

    # Fill the sprite with colors from the pixel arrays
    for frame_index, pixel_array in enumerate(pixel_arrays):
        for y in range(8):
            for x in range(8):
                # Extract the color of the pixel from the array if the array size is valid
                if y < len(pixel_array) and x < len(pixel_array[y]):
                    pixel_color = pixel_array[y][x]
                else:
                    # If we go beyond the array bounds, use white color
                    pixel_color = (255, 255, 255)  # white color
                # Set the pixel color in the sprite
                pixels[x + frame_index * 8, y] = pixel_color

    # Add information about the number of frames and frame duration to the sprite name
    output_sprite_path = f"{output_path}{gif_name}_{gif_name}_{num_frames}f_{duration}ms.sprite.bmp"

    # Save the sprite in BMP format
    sprite.save(output_sprite_path)
    print(f"Created horizontal sprite {output_sprite_path}")


def run():
    srcDir = "gifs/"
    output_path = "frames/"

    for i in range(1, 60000):
        imgName = f"{i}_icon_thumb.gif"
        gif_path = os.path.join(srcDir, imgName)

        # Check if the file exists
        if not os.path.exists(gif_path):
            print(f"File {imgName} does not exist. Skipping.")
            continue

        # Get pixel arrays for each frame of the GIF
        arrays_for_each_frame, num_frames, duration = gif_to_pixel_arrays(gif_path)

        # Use the extract_sub_arrays function to get subarrays for each frame of the GIF
        sub_arrays_for_each_frame = extract_sub_arrays(arrays_for_each_frame)

        # Create a horizontal sprite from subarrays with information about the number of frames and frame duration
        create_sprite(sub_arrays_for_each_frame, num_frames, duration, output_path, imgName[:-4])


run()

