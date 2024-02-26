from PIL import Image

def iterate_pixels(image_path):
    # Open the image
    image = Image.open(image_path)
    
    # Get the dimensions of the image
    width, height = image.size
    
    # Iterate through each pixel in the image
    for y in range(height):
        for x in range(width):
            # Get the RGB values of the pixel at coordinates (x, y)
            pixel = image.getpixel((x, y))
            # Do something with the pixel values, for example, print them
            print(f"Pixel at ({x}, {y}): {pixel}")

# Provide the path to your .jpg image
image_path = "your_image.jpg"

# Call the function to iterate through pixels
iterate_pixels(image_path)
