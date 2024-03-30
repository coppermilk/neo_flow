import requests
import os
from PIL import Image
from io import BytesIO
from concurrent.futures import ThreadPoolExecutor

# Function to download and save image
def download_image(i):
    url = 'https://developer.lametric.com/content/apps/icon_thumbs/' + str(i) + '_icon_thumb.gif'
    filename = os.path.join(folder_name, f"{i}_icon_thumb_lg.png")

    r = requests.get(url, allow_redirects=True)
    if r.status_code == 200:
        try:
            img = Image.open(BytesIO(r.content))
            img.verify()  # Попытка открыть и проверить изображение
            with open(filename, 'wb') as f:
                f.write(r.content)
            print(f"Icon saved as {filename}")
        except Exception as e:
            print(f"Image at {url} is not valid:", e)
    else:
        print(f"Image at {url} not found")

# Create folder if it doesn't exist
folder_name = 'big'
if not os.path.exists(folder_name):
    os.makedirs(folder_name)

# Define number of threads
num_threads = 50  # Adjust as needed

# Create a thread pool
with ThreadPoolExecutor(max_workers=num_threads) as executor:
    # Submit tasks for each range
    for i in range(39111, 100000):
        executor.submit(download_image, i)
