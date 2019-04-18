# qt-aes-crypt
A simple qt gui application to encrypt (decrypt) files (text or binary data) using aes 128 or 256 bit.

The program uses libraries kokke/tiny-AES-C - https://github.com/kokke/tiny-AES-C.
Encryption Modes Supported AES ECB, CBC, 128 or 256 bit. If the extension of the selected file  is "aes", file will be decrypted, otherwise it will be encrypted (extension "aes" will be added). Size of file must be evenly divisible by 16bytes (str_len % 16 == 0), otherwise it will be padded with zeros.

# License

This software is provided under the  <a href="http://unlicense.org/" rel="nofollow">UNLICENSE</a>
