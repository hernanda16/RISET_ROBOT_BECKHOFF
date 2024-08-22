# Global documentation

## Legend

Berisi beberapa istilah yang sering digunakan

```
ROOT_DIR -> direktori utama dari workspace ROS
MSG_IRIS -> direktori msg pada package iris_msgs
MASTER_DIR_SRC -> direktori src pada package master
BS -> BaseStation
2D -> x,y,theta
pose -> 2d -> x,y,theta
```

## Package robot

```
communication
hardware
iris_msgs
master
vision
web_ui
world_model
```

================================================================================================================================

### communication

Package ini isinya adalah program komunikasi menggunakan library icast.
Untuk mengedit config nya berada pada ${ROOT_DIR}/icast.cfg.
Untuk mengedit dictionary.json berada pada direktori icast.

Setelah mengedit biasakan melakukan re-compile icast dan re-install ke system, lalu compile lagi WS ini.

================================================================================================================================

### hardware

Berisi program interface untuk STM32 dan Motor

#### STM32

STM32 menggunakan protokol UDP, menggunakan time division, jadi intinya STM akan mengirim ketika dia menerima data dari PC.
Untuk urutan data dan data apa saja yang dikirim dan diterima oleh STM, bisa dilihat di ${MSG_IRIS}

#### Motor

Disini menggunakan serial USB 8N1. Untuk melihat apakah motor terdeteksi bisa menggunakan `lsusb`.  
Untuk melihat informasi usb bisa menggunakan `udevadm info [usb]` contoh `udevadm info /dev/ttyUSB0`
Untuk mengatur port motor bisa di config static_conf.yaml **Ingat untuk membedakan config berdasarkan nomor robot**

================================================================================================================================

### iris_msgs

Berisi custom msgs dan srv ROS.

================================================================================================================================

### master

Kontrol utama robot ada disini, berisi FSM untuk menentukan kontrol aktuator robot. Master dibagi jadi 2 bagian yaitu Master control dan role player.
Ada juga program web pada folder www

#### master control

Ini adalah low level dari master, berada pada ${MASTER_DIR_SRC}/\*.cpp .
Di master_definitions.cpp disini adalah hal-hal yang berurusan dengan pub/sub dan juga kontrol dasar robot seperti input keyboard, tombol hardware, dan BS
Di motion.cpp disini adalah program untuk membuat motion pada robot, biasanya outputnya berupa velocity 2D.
Di utils.cpp disini adalah hal yang berkaitan dengan utils

#### role player

Ini adalah hal penting bagi program robot
Disini akan membedakan setiap role setiap file.
Isi dari program per-role adalah pekerjaan apa saja yang akan dilakukan oleh robot.
Contoh ketika game memasuki state preparation dropball maka robot attacker akan berada positioning didepan robot defender.

#### Web lama

Disini berisi program web lama agar ribet wkkwwkw

================================================================================================================================

### vision

Disini ada 3 hal yaitu capture, detection, dan web adapter.

#### capture

Capture bertugas untuk akses kamera lalu di-publish ke detection

#### detection

Mendeteksi semua hal yang diperlukan.

##### deteksi bola

Dideteksi dengan threshold inrange biasa dan menggunakan colorspace YUV lalu nanti diambil 5 blob atau contour terbesar. Pixel yang diambil adalah pixel bola terdekat dengan robot.

##### deteksi garis

asdasdasdasdsad

##### deteksi gawang kosong

coming soon.. lupa aku wkwkwk

##### deteksi lapangan

Digunakan untuk mendeteksi obstacle sebenarnya kwkwwk

#### web adapter

Digunakan untuk komunikasi dengan web yang ada pada package master

================================================================================================================================

### web_ui

Ini beda dengan yang ada di master, disini server nya menggunakan python http.server. Ini digunakan untuk menampilkan world model

================================================================================================================================

### world model

Ini adalah middleware yang penting,

#### ball

Mengubah pixel jadi koordinat dunia dan fuse data dengan bola teman, menggunakan sistem voting sederhana, setiap bola yang sama akan menambah value vote sebesar 1 dst lalu dicari vote terbesar

#### line_icp

Disini perubahan koordinat dunia pixel garis, lalu setelah itu dilakukan ICP untuk mendapatkan transformasi koreksi pose robot yang nantinya koreksi transformasi tersebut digunakan pada pose_estimator untuk membenarkan posisi robot.

#### obs

Perubahan obs pixel jadi obs dunia, semua obs dunia dari saya sendiri dan teman-teman lalu nanti dilakukan hal seperti KNN untuk mengelompokkan point-point itu tadi menjadi satu gumpalan obstacle

#### pose_estimator

Mengubah data encoder dari stm dan gyro dari stm menjadi pose_robot dan juga melakukan pembenaran pose robot yang datanya didapat dari line_icp.

================================================================================================================================
