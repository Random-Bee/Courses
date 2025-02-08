from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from tqdm import tqdm

def expandKey(shortKey):    
    shortKeyval1=shortKey[0]
    shortKeyval2=shortKey[1]    
    
    ByteA=shortKeyval1.to_bytes(1,"big")
    ByteB=shortKeyval2.to_bytes(1,"big")
    ByteC=(shortKeyval1^shortKeyval2).to_bytes(1,"big")
    hexByte1=0x94
    Byte1=hexByte1.to_bytes(1,"big")
    hexByte2=0x5a
    Byte2=hexByte2.to_bytes(1,"big")
    hexByte3=0xe7
    Byte3=hexByte3.to_bytes(1,"big")
    
    longKey=bytearray(ByteA)    
    longKey.extend(Byte1)
    longKey.extend(ByteB)    
    longKey.extend(Byte2)
    
    for i in range(4,9):        
        hexByte=(longKey[i-1]+longKey[i-4])%257
        if (hexByte==256):
            hexByte=0
        Byte=hexByte.to_bytes(1,"big")              
        longKey.extend(Byte)
    longKey.extend(ByteC)
    longKey.extend(Byte3)
    for i in range(11,16):
        hexByte=(longKey[i-1]+longKey[i-4])%257
        if (hexByte==256):
            hexByte=0
        Byte=hexByte.to_bytes(1,"big")              
        longKey.extend(Byte)    
    
    return longKey

message = 'Hydrodynamometer'
cipherTxt = 'ea7f6a9b8ca5641e5c574000342a6322'

dict = {}

for i in tqdm(range(0, 1<<16)):
    key = expandKey(i.to_bytes(2, "big"))

    iv = b'\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0'
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv))
    encryptor = cipher.encryptor()

    ct = encryptor.update(message.encode('utf-8')) + encryptor.finalize()

    dict[ct.hex()] = [key.hex()]

for i in tqdm(range(0, 1<<16)):
    key = expandKey(i.to_bytes(2, "big"))

    iv = b'\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0'
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv))
    decryptor = cipher.decryptor()

    pt = decryptor.update(bytes.fromhex(cipherTxt)) + decryptor.finalize()

    if pt.hex() in dict:
        dict[pt.hex()].append(key.hex())
    else:
        dict[pt.hex()] = [key.hex()]

for i in dict:
    if len(dict[i]) == 2:
        key1 = dict[i][0]
        key2 = dict[i][1]
        break

secretCipher = 'c85afb6a2947ee3497ddf2b10e3ac81b'
iv = b'\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0'
cipher2 = Cipher(algorithms.AES(bytes.fromhex(key2)), modes.CBC(iv))
decryptor2 = cipher2.decryptor()
pt1 = decryptor2.update(bytes.fromhex(secretCipher)) + decryptor2.finalize()
cipher1 = Cipher(algorithms.AES(bytes.fromhex(key1)), modes.CBC(iv))
decryptor1 = cipher1.decryptor()
pt = decryptor1.update(bytes.fromhex(pt1.hex())) + decryptor1.finalize()
print("Secret =", pt)
print("Key1 =", key1)
print("Key2 =", key2)