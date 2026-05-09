sed -i 's/case 0x10: {/case 0x10: { printf("CALL fi=%d np=%d nr=%d\\n", fi, np, nr);/' runtime/luon_vm.c
sed -i 's/case 0x0F: {/case 0x0F: { printf("RET sp=%d\\n", sp);/' runtime/luon_vm.c
