/* Custom Uzebox linker script for normal executables
 * Creates 2 new sections:
 *   uze_progmem_ogigin: located in flash right after the vectors
 *   uze_ram_origin: located at origin 0x100
 *
 * Based on: \WinAVR\avr\lib\ldscripts\avr5.x
 */
OUTPUT_FORMAT("elf32-avr","elf32-avr","elf32-avr")
OUTPUT_ARCH(avr:5)
MEMORY
{
  text      (rx)   : ORIGIN = 0, LENGTH = 128K
  data      (rw!x) : ORIGIN = 0x800060, LENGTH = 0xffa0
  eeprom    (rw!x) : ORIGIN = 0x810000, LENGTH = 64K
  fuse      (rw!x) : ORIGIN = 0x820000, LENGTH = 1K
  lock      (rw!x) : ORIGIN = 0x830000, LENGTH = 1K
  signature (rw!x) : ORIGIN = 0x840000, LENGTH = 1K
}
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .hash          : { *(.hash)		}
  .dynsym        : { *(.dynsym)		}
  .dynstr        : { *(.dynstr)		}
  .gnu.version   : { *(.gnu.version)	}
  .gnu.version_d   : { *(.gnu.version_d)	}
  .gnu.version_r   : { *(.gnu.version_r)	}
  .rel.init      : { *(.rel.init)		}
  .rela.init     : { *(.rela.init)	}
  .rel.text      :
    {
      *(.rel.text)
      *(.rel.text.*)
      *(.rel.gnu.linkonce.t*)
    }
  .rela.text     :
    {
      *(.rela.text)
      *(.rela.text.*)
      *(.rela.gnu.linkonce.t*)
    }
  .rel.fini      : { *(.rel.fini)		}
  .rela.fini     : { *(.rela.fini)	}
  .rel.rodata    :
    {
      *(.rel.rodata)
      *(.rel.rodata.*)
      *(.rel.gnu.linkonce.r*)
    }
  .rela.rodata   :
    {
      *(.rela.rodata)
      *(.rela.rodata.*)
      *(.rela.gnu.linkonce.r*)
    }
  .rel.data      :
    {
      *(.rel.data)
      *(.rel.data.*)
      *(.rel.gnu.linkonce.d*)
    }
  .rela.data     :
    {
      *(.rela.data)
      *(.rela.data.*)
      *(.rela.gnu.linkonce.d*)
    }
  .rel.ctors     : { *(.rel.ctors)	}
  .rela.ctors    : { *(.rela.ctors)	}
  .rel.dtors     : { *(.rel.dtors)	}
  .rela.dtors    : { *(.rela.dtors)	}
  .rel.got       : { *(.rel.got)		}
  .rela.got      : { *(.rela.got)		}
  .rel.bss       : { *(.rel.bss)		}
  .rela.bss      : { *(.rela.bss)		}
  .rel.plt       : { *(.rel.plt)		}
  .rela.plt      : { *(.rela.plt)		}
  
  
  /* Internal text space or external memory.  */
  .text   :
  {
    *(.vectors)
    KEEP(*(.vectors))

    /* UZEBOX Extension: Game data that needs to be located at the beginning of progmem right after vectors  */
	*(.uzepgmorigin)
	KEEP (*(.uze_progmem_origin))

    /* For data that needs to reside in the lower 64k of progmem.  */
    *(.progmem.gcc*)
    *(.progmem*)
    . = ALIGN(2);
     __trampolines_start = . ;
    /* The jump trampolines for the 16-bit limited relocs will reside here.  */
    *(.trampolines)
    *(.trampolines*)
     __trampolines_end = . ;
    /* For future tablejump instruction arrays for 3 byte pc devices.
       We don't relax jump/call instructions within these sections.  */
    *(.jumptables)
    *(.jumptables*)
    /* For code that needs to reside in the lower 128k progmem.  */
    *(.lowtext)
    *(.lowtext*)
     __ctors_start = . ;
     *(.ctors)
     __ctors_end = . ;
     __dtors_start = . ;
     *(.dtors)
     __dtors_end = . ;
    KEEP(SORT(*)(.ctors))
    KEEP(SORT(*)(.dtors))
    /* From this point on, we don't bother about wether the insns are
       below or above the 16 bits boundary.  */
    *(.init0)  /* Start here after reset.  */
    KEEP (*(.init0))
    *(.init1)
    KEEP (*(.init1))
    *(.init2)  /* Clear __zero_reg__, set up stack pointer.  */
    KEEP (*(.init2))
    *(.init3)
    KEEP (*(.init3))
    *(.init4)  /* Initialize data and BSS.  */
    KEEP (*(.init4))
    *(.init5)
    KEEP (*(.init5))
    *(.init6)  /* C++ constructors.  */
    KEEP (*(.init6))
    *(.init7)
    KEEP (*(.init7))
    *(.init8)
    KEEP (*(.init8))
    *(.init9)  /* Call main().  */
    KEEP (*(.init9))
    *(.text)
    . = ALIGN(2);
    *(.text.*)
    . = ALIGN(2);
    *(.fini9)  /* _exit() starts here.  */
    KEEP (*(.fini9))
    *(.fini8)
    KEEP (*(.fini8))
    *(.fini7)
    KEEP (*(.fini7))
    *(.fini6)  /* C++ destructors.  */
    KEEP (*(.fini6))
    *(.fini5)
    KEEP (*(.fini5))
    *(.fini4)
    KEEP (*(.fini4))
    *(.fini3)
    KEEP (*(.fini3))
    *(.fini2)
    KEEP (*(.fini2))
    *(.fini1)
    KEEP (*(.fini1))
    *(.fini0)  /* Infinite loop after program termination.  */
    KEEP (*(.fini0))
     _etext = . ;
  }  > text
  

  
  .data	  : AT (ADDR (.text) + SIZEOF (.text))
  {
     	
     PROVIDE (__data_start = .) ;

    *(.uze_data_origin)
    *(.data)
    *(.data*)
    *(.rodata)  /* We need to include .rodata here if gcc is used */
    *(.rodata*) /* with -fdata-sections.  */
    *(.gnu.linkonce.d*)
    . = ALIGN(2);
     _edata = . ;
     PROVIDE (__data_end = .) ;
  }  > data
  .bss   : AT (ADDR (.bss))
  {
     PROVIDE (__bss_start = .) ;
    *(.bss)
    *(.bss*)
    *(COMMON)
     PROVIDE (__bss_end = .) ;
  }  > data
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
  /* Global data not cleared after reset.  */
  .noinit  :
  {
     PROVIDE (__noinit_start = .) ;
    *(.noinit*)
     PROVIDE (__noinit_end = .) ;
     _end = . ;
     PROVIDE (__heap_start = .) ;
  }  > data
  .eeprom  :
  {
    *(.eeprom*)
     __eeprom_end = . ;
  }  > eeprom
  .fuse  :
  {
    KEEP(*(.fuse))
    KEEP(*(.lfuse))
    KEEP(*(.hfuse))
    KEEP(*(.efuse))
  }  > fuse
  .lock  :
  {
    KEEP(*(.lock*))
  }  > lock
  .signature  :
  {
    KEEP(*(.signature*))
  }  > signature
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
     
       /* Memory mapped IO symbols defined to support GDB debugging*/
	PINA	= 0x800020;
	DDRA	= 0x800021;
	PORTA	= 0x800022;  
	PINB	= 0x800023;
	DDRB	= 0x800024;
	PORTB	= 0x800025;  
	PINC	= 0x800026;
	DDRC	= 0x800027;
	PORTC	= 0x800028;  
	PIND	= 0x800029;
	DDRD	= 0x80002a;
	PORTD	= 0x80002b;  
	TIFR0	= 0x800035;
	TIFR1	= 0x800036;
	TIFR2	= 0x800037;
	PCIFR	= 0x80003b;
	EIFR	= 0x80003c;
	EIMSK	= 0x80003d;      
	GPIOR0	= 0x80003e;
	EECR	= 0x80003f;
	EEDR	= 0x800040;
	EEARL	= 0x800041;
	EEARH	= 0x800042;
	GTCCR	= 0x800043;
	TCCR0A	= 0x800044;
	TCCR0B	= 0x800045;
	TCNT0	= 0x800046;
	OCR0A	= 0x800047;
	OCR0B	= 0x800048;
	GPIOR1	= 0x80004a;
	GPIOR2	= 0x80004b;
	SPCR	= 0x80004c;
	SPSR	= 0x80004d;
	SPDR	= 0x80004e;
	ACSR	= 0x800050;
	OCDR 	= 0x800051;
	SMCR	= 0x800053;
	MCUSR	= 0x800054;
	MCUCR	= 0x800055;
	SPMCSR	= 0x800057;
	SPL		= 0x80005d;
	SPH		= 0x80005e;
	SREG	= 0x80005f;	
	WDTCSR	= 0x800060;
	CLKPR	= 0x800061;
	PRR		= 0x800064;
	OSCCAL	= 0x800066;
	PCICR	= 0x800068;
	EICRA	= 0x800069;
	PCMSK0	= 0x80006b;
	PCMSK1	= 0x80006c;
	PCMSK2	= 0x80006d;
	TIMSK0	= 0x80006e;
	TIMSK1	= 0x80006f;
	TIMSK2	= 0x800070;
	PCMSK3	= 0x800073;
	ADCL	= 0x800078;
	ADCH	= 0x800079;
	ADCSRA	= 0x80007a;
	ADCSRB	= 0x80007b;
	ADMUX	= 0x80007c;
	DIDR0	= 0x80007e;
	DIDR1	= 0x80007f;
	TCCR1A	= 0x800080;
	TCCR1B	= 0x800081;
	TCCR1C	= 0x800082;	
	TCNT1L 	= 0x800084;
	TCNT1H	= 0x800085;
	ICR1L	= 0x800086;
	ICR1H	= 0x800087;
	OCR1AL	= 0x800088;
	OCR1AH	= 0x800089;	
	OCR1BL	= 0x80008a;
	OCR1BH	= 0x80008b;
	TCCR2A	= 0x8000b0;
	TCCR2B	= 0x8000b1;		
	TCNT2	= 0x8000b2;
	OCR2A	= 0x8000b3;
	OCR2B	= 0x8000b4;
	ASSR	= 0x8000b6;	
	TWBR	= 0x8000b8;
	TWSR	= 0x8000b9;		
	TWAR	= 0x8000ba;
	TWDR	= 0x8000bb;
	TWCR	= 0x8000bc;
	TWAMR	= 0x8000bd;				  
	UCSR0A	= 0x8000c0;
	UCSR0B	= 0x8000c1;		
	UCSR0C	= 0x8000c2;
	UBRR0L	= 0x8000c4;
	UBRR0H	= 0x8000c5;
	UDR0	= 0x8000c6;	
	
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
}
