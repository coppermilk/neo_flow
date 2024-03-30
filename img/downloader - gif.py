import requests
import os
from PIL import Image
from io import BytesIO

# Create folder if it doesn't exist
folder_name = 'gifs'
if not os.path.exists(folder_name):
    os.makedirs(folder_name)

for i in range(29999, 100000):
    url = 'https://developer.lametric.com/content/apps/icon_thumbs/' + str(i) + '_icon_thumb.gif'
    filename = os.path.join(folder_name, f"{i}_icon_thumb.gif")

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
