# HPET-UEFI
A simple baremetal app, that demonstrates the work of the HPET timer in the UEFI enviroment.

## Building and running
```make``` to build.  
```make qemu``` to run and already built app in the qemu.  
In order to run in on the real hardware, create a EFI bootable USB and replace it's BOOTX64.EFI and startup.nsh with the ones, found in the bootdrv folder after you build the app.  

## Project contents
### main.asm
  Save the EFI variables into the .data section.  
  Print "start!" to indicate that the app is running okay.  
  Get the MADT ACPI table base address.  
  Configure Local APIC.  
  Disable legacy PIC.  
  Print "start!" again to indicate that the app is running okay.  
  Initialize HPET: bind it to the 0x22 interrupt of the IDT, use the ```intfunc``` as the interrupt vector, set HPET to tick periodically every millisecond, use timer №0.  
  Print the return value of the ```init_hpet``` function.  
  Print the current time counter value (in milliseconds).  
  Sleep for 3000 milliseconds.  
  Print the current time counter value (in milliseconds).  
  Stall indefinitely.  
### HPET.inc
  Contains the ```get_ACPI_HPET_BAR``` function. returns the HPET ACPI table base address.
### HPET_driver_generic.inc
  Contains the ```init_hpet``` function. Configures the N'th HPET timer into the periodic mode, with the given frequency. Binds the HPET ticks to a given interrupt vector, that is bound to the given interrupt number.  
  If possible, function routes the interrupt through the FSB routing mechanism. Otherwise, it uses the IOAPIC routing.
  #### Possible return values:
    0 - HPET successfully configured, used the FSB routing mechanism.  
    1 - HPET successfully configured, used the IOAPIC routing mechanism.  
    -1 - Failed to locate the RSDP (Root System Description Pointer).  
    -2 - Failed to locate the XSDT ACPI table.  
    -3 - Failed to locate the HPET ACPI table.  
    -4 - Desired frequency is not implemented in hardware.  
    -5 - Comparator width is 32-bit, but 64-bits needed to implement the desired frequency.  
    -6 - Selected timer has no periodic mode.  

### IDT.inc
  Contains the functions ```write_IDT_gate``` and ```read_IDT_gate```, needed for binding your function to the N'th interrupt vector, and for getting the current function, bound to the N'th interrupt vector respectively.  
  The HPET driver needs only the ```write_IDT_gate``` function.  
  ### Possible return values:
    0 - Successfully bound the ISR to the selected gate.
    -1 - Gate N is out of range.
    
### MADT.inc
  Contains the ```get_MADT_BAR``` function. Finds and stores the base address of ACPI MADT table in the .data section, and returns it in rax. 
  #### Possible return values:
    MADT BAR - successfully found and returned the MADT base address.
    1 - Failed to locate the RSDP (Root System Description Pointer).  
    2 - Failed to locate the XSDT ACPI table.  
    3 - Failed to locate the MADT ACPI table.  
    
### apic.inc
  Contains the ```conf_LAPIC``` function. Reads and stores the LAPIC base address, Enables external interrupts, Non-maskable interrupts, sets task priority to 0.  

### cvt2hex.inc
  Contains the ```cvt2hex``` function. A simple unsigned integer to hexadecimal string coversion function.  

### ioapic.inc
### jd9999_hdr_macro.inc
