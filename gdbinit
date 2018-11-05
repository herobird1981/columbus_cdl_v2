
target remote localhost:2331

# monitor clrbp xxx
monitor halt
monitor reset
monitor WriteU32  0x20002008  0xDE
# shell sleep 1

load

delete 
info breakpoints

# add-symbol-file D:/src/brite_only/arm/boot_gcc/objs/boot_rom.axf 0x0

#break _start
#break main

set $pc = _start
#c
