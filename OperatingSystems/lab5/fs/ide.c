/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurred during read the IDE disk, panic. 
// 	
// Hint: use syscalls to access device registers and buffers
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	int cur_offset = offset_begin + offset;
	u_int dev = 0x13000000;
	u_int status = 0;
	u_int read_value = 0;

	while (offset_begin + offset < offset_end) {
            // Your code here
            // error occurred, then panic.
		cur_offset = offset_begin + offset;
		if (syscall_write_dev((u_int) &diskno, dev + 0x10, sizeof(diskno)) < 0) {
			user_panic("ide read fail 1");
		}
		if (syscall_write_dev((u_int) &cur_offset, dev, sizeof(cur_offset)) < 0) {
			user_panic("ide read fail 2");
		}
		if (syscall_write_dev((u_int) & read_value, dev + 0x20, sizeof(read_value)) < 0) {
			user_panic("ide read fail 3");
		}
		status = 0;
		if (syscall_read_dev((u_int) &status, dev + 0x30, sizeof(status)) < 0) {
			user_panic("ide read fail 4");
		}
		if (status == 0) {
			user_panic("ide read fail 5");
		}
		if (syscall_read_dev((u_int) (dst + offset), dev + 0x4000, 0x200) < 0) {
			user_panic("ide read fail 6");
		}
		offset += 0x200;
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurred during read the IDE disk, panic.
//	
// Hint: use syscalls to access device registers and buffers
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
        // Your code here
	// int offset_begin = ;
	// int offset_end = ;
	// int offset = ;
	u_int offset_begin = secno * 0x200;
	u_int offset_end = offset_begin + nsecs * 0x200;
	u_int offset = 0;
	u_int dev = 0x13000000;
	u_int cur_offset = offset_begin + offset;
	u_int status = 0;
	u_int write_value = 1;

	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);

	// while ( < ) {
	    // copy data from source array to disk buffer.

            // if error occur, then panic.
	// }
	
	while (offset_begin + offset < offset_end) {
		cur_offset = offset_begin + offset;
		if (syscall_write_dev((u_int) &diskno, dev + 0x10, sizeof(diskno)) < 0) {
			user_panic("ide write fail 1");
		}
		if (syscall_write_dev((u_int) &cur_offset, dev, sizeof(cur_offset)) < 0) {
			user_panic("ide write fail 2");
		}
		if (syscall_write_dev((u_int) (src + offset), dev + 0x4000, 0x200) < 0) {
			user_panic("ide write fail 3");
		}
		if (syscall_write_dev((u_int) &write_value, dev + 0x20, sizeof(write_value)) < 0) {
			user_panic("ide write fail 4");
		}
		status = 0;
		if (syscall_read_dev((u_int) &status, dev + 0x30, sizeof(status)) < 0) {
			user_panic("ide write fail 5");
		}
		if (status == 0) {
			user_panic("ide write fail 6");
		}
		offset += 0x200;
	}
}

