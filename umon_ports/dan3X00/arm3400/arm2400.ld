/*
 All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
 */

OUTPUT_FORMAT("elf32-bigarm", "elf32-bigarm", "elf32-bigarm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = 0x43000000;
    . = ALIGN(4);
    .text	:
    {
      *(.text.vector)
      *(.text)
    }
    . = ALIGN(4);
    .rodata : { *(.rodata) }
    . = ALIGN(4);
    .data : { *(.data) }
    . = ALIGN(4);
    .got : { *(.got) }
    . = .;
    __image_end = .;
    . = ALIGN(32);
	. = . + 0x8000;
    __bss_start = .;
    .bss : { *(.bss) }
    _end = .;
    end = .;
}
