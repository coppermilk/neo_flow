from PIL import Image
import os

# Путь к директории с изображениями GIF
input_directory = "gifs/"
# Путь к директории для сохранения кадров
output_directory = "frames/"

# Создаем директорию для сохранения кадров, если ее не существует
if not os.path.exists(output_directory):
    os.makedirs(output_directory)

def extract_frames(gif_file):
    with Image.open(gif_file) as img:
        # Получаем данные всех кадров
        frames_data = []
        for frame in range(img.n_frames):
            img.seek(frame)
            frame_data = img.copy()
            frames_data.append(frame_data)
        
        # Сохраняем каждый кадр в отдельное изображение
        for frame_number, frame_data in enumerate(frames_data):
            new_image = Image.new("RGB", (8, 8))
            pixels = new_image.load()
            for y in range(5, 45, 5):  # Выбираем каждый 5-й пиксель по вертикали
                for x in range(5, 45, 5):  # Выбираем каждый 5-й пиксель по горизонтали
                    pixel = frame_data.getpixel((x, y))
                    pixels[x // 5 - 1, y // 5 - 1] = pixel  # Записываем пиксель в новое изображение
            new_image.save(os.path.join(output_directory, f"frame_{frame_number}.bmp"), "BMP")
            print(f"Frame {frame_number} saved successfully.")

if __name__ == "__main__":
    gif_file = "gifs/112_icon_thumb.gif"  # Укажите путь к вашему GIF-файлу
    extract_frames(gif_file)

print("Extraction completed.")
