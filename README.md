# HPET-UEFI
A simple baremetal app, that demonstrates the work of the HPET timer in the UEFI enviroment.

## Building and running
```make``` to build.  
```make qemu``` to run and already built app in the qemu.  
In order to run in on the real hardware, create a EFI bootable USB and replace it`s BOOTX64.EFI and startup.nsh with the ones, found in the bootdrv folder after you build the app.  

## Project contents
### main.asm
### HPET_driver_generic.inc
### HPET.inc
### IDT.inc
### MADT.inc
### apic.inc
### cvt2hex.inc
### ioapic.inc
### jd9999_hdr_macro.inc
