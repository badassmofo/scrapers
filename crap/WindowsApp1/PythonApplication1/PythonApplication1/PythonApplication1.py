import sys, requests, io, zlib, json

test_id = 511343

def read_int(stream, size=4):
  return int.from_bytes(stream.read(size), byteorder='little', signed=True)

def read_str(stream, size):
  return stream.read(size).decode('utf8')

req = requests.get('http://www.dustkid.com/backend8/get_replay.php', params={'replay': test_id}, allow_redirects=True)
req.raise_for_status()

replay = io.BytesIO(req.content)
if not read_str(replay, 6) == "DF_RPL":
  raise Exception("Invalid replay!")

unk1   = replay.read(1)
user   = read_str(replay, read_int(replay, 2))
if not read_str(replay, 6) == "DF_RPL":
  raise Exception("Invalid replay!")
unk2   = replay.read(1)
unk3   = replay.read(2)
d_len  = read_int(replay)
frames = read_int(replay)
char   = ["Dustboy", "Dustgirl", "Dustkid", "Dustworth"][read_int(replay, 1)]
level  = read_str(replay, read_int(replay, 1))
data   = io.BytesIO(zlib.decompress(replay.read(d_len + 2)))

class InputsReader(object):
  def __init__(self, data):
    self.data = data
    self.len  = len(data)
    self.pos  = 0
    
  def read(self, bits):
    if bits <= 0:
      bitPosition += bits
      return 0xff

    buffer = [0xFF]
    firstBitOffsetInByte = self.pos & 7
    bytePos = self.pos >> 3
    outputPos = 0
    bitsRemaining = bits
    overflowBits = 8 - bits
    while bitsRemaining > 0:
      part1 = self.data[bytePos] >> firstBitOffsetInByte
      part2 = self.data[bytePos + 1] << (8 - firstBitOffsetInByte) if bytePos + 1 < self.len else 0
      mask = 0xFF >> overflowBits if bitsRemaining < 8 else 0xFF
      buffer[outputPos] = ((part2 | part1) & mask)
      bitsRemaining -= 8
      overflowBits += 8
      bytePos += 1
      outputPos += 1
    self.pos += bits
    return buffer[0]

inputs_len  = read_int(data)
inputs = []
for i in range(0, 7):
  raw = InputsReader(data.read(read_int(data)))
  ret = ""
  last_val = 0
  while True:
    fs = raw.read(8)
    if fs == 0xFF:
      break
    val = raw.read(4 if i >= 5 else 2)

    for j in range(0, fs + 1):
      ret += chr(last_val + (48 if last_val < 10 else 87))
    last_val = val
  if (len(ret) > 1):
    inputs.append(ret[1:])

sync = []
for i in range(0, read_int(data)):
  euid = read_int(data)
  unk4 = read_int(data)
  sync.append({ "entity_uid": euid, "corrections": [[read_int(data) for k in range(0, 5)] for j in range(0, read_int(data))]})

with open("%d.json" % test_id, "w") as fh:
  fh.write(json.dumps({"user":    user,
                       "level":   level,
                       "frames":  frames,
                       "char":    char,
                       "inputs":  inputs,
                       "sync":    sync}, indent=4))

for i in range(0, frames):
  finputs = [0, 0, 0, 0, 0]
  for j in range(0, 5):
    pass

input("Press Enter to continue...")