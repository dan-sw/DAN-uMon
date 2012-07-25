/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = 0x40400000;
    .text	:
    {
      *(.text.vector)
      *(.text)
    }
    . = ALIGN(32);
    .rodata	:
    {
      *(.piggy.rodata)
      *(.rodata)
    }
    . = ALIGN(4);
	.ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
    . = ALIGN(4);
    .data : { *(.data) }
    . = ALIGN(4);
    .got : { *(.got) }
	__exidx_start = .;
	.ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
	__exidx_end = .;
    . = .;
    __image_end = .;
    . = ALIGN(32);
    __bss_start = .;
    .bss : { *(.bss) }
    _end = .;
    end = .;
}
