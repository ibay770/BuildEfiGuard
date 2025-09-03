
import sys
import uuid
import string
import secrets
import random

PATTERN = string.ascii_lowercase + string.ascii_uppercase + string.digits

def generate_guid():
    u = uuid.uuid4()
    time_low, time_mid, time_hi_version, cshv, csl, node = u.fields
    data4 = bytes([cshv, csl]) + node.to_bytes(6, 'big')
    data4_hex = ", ".join(f"0x{b:x}" for b in data4)
    return f"0x{time_low:08x}, 0x{time_mid:04x}, 0x{time_hi_version:04x}, {{ {data4_hex} }}"

def generate_string(len):
    return ''.join([ secrets.choice(PATTERN) for _ in range(len) ])

def main(argv):
    guid = generate_guid()
    varname = generate_string(random.randint(16, 64))
    cookie = random.randint(0x00000000, 0xFFFFFFFF)

    print(f'GUID:    {guid}')
    print(f'VARNAME: {varname}')
    print(f'COOKIE:  0x{cookie:08X}')

    src = open(argv[1], 'r').read()
    src = src.replace('{0}', guid)
    src = src.replace('{1}', varname)
    src = src.replace('{2}', f'0x{cookie:08X}')
    open(argv[2], 'w').write(src)

if __name__ == '__main__':
    main(sys.argv)
