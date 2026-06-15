#!/usr/bin/env python3
"""Client test cục bộ: gửi 1 request, kiểm tra server echo lại correlation_id."""
import socket
import struct
import sys

HOST, PORT = "localhost", 9092
EXPECTED_CORRELATION_ID = 311908132  # số bất kỳ để kiểm tra

# Request body: api_key(int16) + api_version(int16) + correlation_id(int32)
#   ">"  = big-endian (network byte order)
#   "h"  = int16, "i" = int32
body = struct.pack(">hhi", 18, 4, EXPECTED_CORRELATION_ID)
request = struct.pack(">i", len(body)) + body  # 4 byte message_size ở đầu

with socket.create_connection((HOST, PORT), timeout=5) as s:
    s.sendall(request)

    # Đọc 4 byte message_size của response
    size_bytes = s.recv(4)
    (resp_size,) = struct.unpack(">i", size_bytes)

    # Đọc resp_size byte thân
    body_bytes = s.recv(resp_size)
    (correlation_id,) = struct.unpack(">i", body_bytes[:4])

print(f"response message_size = {resp_size}")
print(f"correlation_id nhận về = {correlation_id}")

if correlation_id == EXPECTED_CORRELATION_ID:
    print("✅ PASS")
    sys.exit(0)
else:
    print(f"❌ FAIL (mong đợi {EXPECTED_CORRELATION_ID})")
    sys.exit(1)
