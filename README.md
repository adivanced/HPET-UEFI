# HPET-UEFI
A simple baremetal app, that demonstrates the work of the HPET timer in the UEFI enviroment.

## Building and running
```make``` to build.  
```make qemu``` to run and already built app in the qemu.  
In order to run in on the real hardware, create a EFI bootable USB and replace it`s BOOTX64.EFI and startup.nsh with the ones, found in the bootdrv folder after you build the app.  

## Project contents
### main.asm
  Save the EFI variables into the .data section.  
  Print "start!" to indicate that the app is running okay.  
  Get the MADT ACPI table base address.  
  Configure Local APIC.  
  Disable legacy PIC.  
  Print "start!" again to indicate that the app is running okay. 
  Initialize HPET: bind it to the 0x22 interrupt of the IDT, use the ```intfunc``` as the interrupt vector, set HPET to tick periodically every millisecond.  
  Print the return value of the ```init_hpet``` function.  
  Print the current time counter value (in milliseconds).  
  Sleep for 3000 milliseconds.  
  Print the current time counter value (in milliseconds).  
  Stall indefinitely.  
### HPET_driver_generic.inc
### HPET.inc
### IDT.inc
### MADT.inc
### apic.inc
### cvt2hex.inc
### ioapic.inc
### jd9999_hdr_macro.inc
