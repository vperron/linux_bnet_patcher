/*
 * =====================================================================================
 *
 *       Filename:  linux_bnet_patcher.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2012 01:05:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Victor Perron (), public@iso3103.net
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <sys/ptrace.h>

#define ADDR 0x3C9C5605

int main(int argc, char** argv) {

	long val, patched;
	int pid;

	if (argc < 2) {
		printf("Usage : %s <PID>\n", argv[0]);
		return 1;
	}

	pid = atoi(argv[1]);

	val = ptrace(PTRACE_ATTACH, pid, 0, 0);
	if (val != -1) {
		printf("Successfully attached to process: %d\n",pid);
		//need to wait a bit, to be sure the process is stopped
        	wait(2);
		val = ptrace(PTRACE_PEEKDATA, pid, ADDR, 0);
		if (val != -1) {
			// The read byte should be 0x75 (JNZ / JumpNotZero)
			printf("Read byte at %x : %x\n", ADDR, (int)(val & 0xff));
			// Replace it with 0xEB (JMP, forced jump)
			patched = (val & 0xffffff00) | 0xeb;
			ptrace(PTRACE_POKEDATA, pid, ADDR, (int)patched);
			val = ptrace(PTRACE_PEEKDATA, pid, ADDR, 0);
			printf("Read patched byte at %x : %x\n", ADDR, (int)(val & 0xff));
			ptrace(PTRACE_DETACH, pid, 0, 0);
			return 0;
		}
	}
	perror(NULL);
	return 1;
}
