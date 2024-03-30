import requests
import os

# Create folder if it doesn't exist
folder_name = 'data'
if not os.path.exists(folder_name):
    os.makedirs(folder_name)

for i in range(4980, 60000, 1):
    url = 'https://developer.lametric.com/content/apps/icon_thumbs/' + str(i) +'_icon_thumb_lg.png'
    filename = os.path.join(folder_name, url.split('/')[-1])

    r = requests.get(url, allow_redirects=True)
    with open(filename, 'wb') as f:
        f.write(r.content)

print(f"Icon saved as {filename}")
