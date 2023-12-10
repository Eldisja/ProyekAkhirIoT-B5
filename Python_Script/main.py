# main.py
import time

from WiFiCommunicator import WiFiCommunicator

# Buat objek WiFiCommunicator
wifi_communicator = WiFiCommunicator(max_buffer_sz=1024)

# Jalankan program
try:
    while True:
        # Dapatkan pesan dari antrian pesan masuk
        message = wifi_communicator.get_message()
        print(message.data)
        wifi_communicator.send_message("test")
        if message is not None:
            # Periksa apakah plat nomor sesuai dengan yang diinginkan
            plat_nomor_terdeteksi = message.data
            plat_nomor_diinginkan = "ABC123"

            is_plat_nomor_sesuai = plat_nomor_terdeteksi == plat_nomor_diinginkan

            # Cetak apakah palang bedas naik atau turun berdasarkan hasil komparasi
            #if is_plat_nomor_sesuai:
               # print("Palang Bedas Naik")
            #else:
               # print("Palang Bedas Turun")
        time.sleep(0.5)
except KeyboardInterrupt:
    # Tangkap sinyal keyboard interrupt dan hentikan program
    wifi_communicator.destroy()
