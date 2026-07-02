# 🖼️ LSB Image Steganography in C

This project is a simple implementation of **Least Significant Bit (LSB) Image Steganography** in C. It allows users to hide a secret text message inside a **24-bit BMP image** and later retrieve it without noticeably changing the image.

The main objective of this project is to understand how steganography works by using bit manipulation and file handling in C. The application follows a modular design with separate encoding and decoding modules, making the code easy to understand and maintain.

## Features

- Hide a secret message inside a BMP image.
- Extract the hidden message from the stego image.
- Uses the Least Significant Bit (LSB) technique.
- Preserves the visual quality of the original image.
- Modular and well-structured C code.
- Includes basic input validation and error handling.

## Technologies Used

- C Programming
- File Handling
- Bit Manipulation
- BMP Image Processing

## How It Works

1. Read the original BMP image.
2. Read the secret text file.
3. Embed the secret message into the least significant bits of the image pixels.
4. Generate a new stego image.
5. Decode the hidden message whenever required.

## Project Screenshots

### Encoding
![Encoding](encode.png)

### Decoding
![Decoding](decode.png)

### Original Image
![Original](beautiful.bmp.png)

### Secret File
![Secret](secret_file_data.png)

### Stego Image
![Stego](stego_img.png)

### Decoded Output
![Output](outputtxt.png)

## Author

**Vijay**
