import sys
import base64
# pip install pycryptodome
from Crypto.Cipher import AES
from Crypto import Random

class AESCipher:
    def __init__( self, key ):
        self.key = key
        self.bs = 32

    def encrypt( self, raw ):
        raw = self._pad(raw)
        #iv = Random.new().read( AES.block_size )
        iv = bytes(b'\x00' * 16)
        cipher = AES.new( self.key, AES.MODE_CBC, iv )
        #return base64.b64encode( iv + cipher.encrypt( raw ) ) 
        return cipher.encrypt( raw )

    def decrypt( self, enc ):
        #enc = base64.b64decode(enc)
        iv = bytes(b'\x00' * 16)
        cipher = AES.new(self.key, AES.MODE_CBC, iv )
        # unpad(...)
        return self._unpad(cipher.decrypt(enc))

    def _pad(self, s):
        return s + bytes([self.bs - len(s) % self.bs]) * (self.bs - len(s) % self.bs)

    @staticmethod
    def _unpad(s):
        return s[:-ord(s[len(s)-1:])]

# --------------------------------------------------------------------------------
encoded_file = sys.argv[1]
decoded_file = encoded_file + '.decoded'

with open("..\\aes.key.bin", "rb") as f: aes_key = f.read()
with open(encoded_file, "rb") as f: encoded_data = f.read()

print("AES key (%u): %s" % (len(aes_key), aes_key)) 
print("Encoded (%u): %s" % (len(encoded_data), encoded_data)) 
aes_key_encoded = encoded_data[0:256]
aes_data_encoded = encoded_data[256:]
c = AESCipher(aes_key)
dec_data = c.decrypt(aes_data_encoded)
print("Decoded (%u): %s" % (len(dec_data), dec_data)) 

with open(decoded_file, "wb") as f: f.write(dec_data)
