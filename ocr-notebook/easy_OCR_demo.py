# easy_OCR_demo.py

!pip install easyocr
import cv2
import easyocr
from IPython.display import Image
from pylab import rcParams
import matplotlib.pyplot as plt

# Tampilkan gambar
Image("../plates/scaned_img_0.jpg")

# Inisialisasi EasyOCR
reader = easyocr.Reader(['en'])

# Baca teks dari gambar
output = reader.readtext('../plates/scaned_img_0.jpg')

# Tampilkan output dari EasyOCR
print("Detected License Plate:", output[0][1])

# Ambil koordinat kotak batas
cord = output[-1][0]
x_min, y_min = [int(min(idx)) for idx in zip(*cord)]
x_max, y_max = [int(max(idx)) for idx in zip(*cord)]

# Tampilkan kotak batas pada gambar
image = cv2.imread('../plates/scaned_img_0.jpg')
cv2.rectangle(image, (x_min, y_min), (x_max, y_max), (0, 0, 255), 2)

# Tampilkan gambar dengan kotak batas
rcParams['figure.figsize'] = 10, 15
plt.imshow(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
plt.show()

# Plat nomor yang diinginkan (ganti dengan plat nomor yang benar)
plat_nomor_diinginkan = "ABC123"
