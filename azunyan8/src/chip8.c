/*
	chip8.c - Chip-8 interpreter core & supplemental function
*/

#include "globals.h"

void cpuReset()
{
	interpreter.addri = 0;
	interpreter.pc = 0x200;
	interpreter.sp = 0;
	memset(interpreter.regs, 0x00, sizeof(interpreter.regs));
	memset(interpreter.stack, 0x00, sizeof(interpreter.stack));
	interpreter.tDelay = 0;
	interpreter.tSound = 0;
}

int soundBeep()
{
	int ret = 0;
	if(program.enableSound) ret = playSound(parseFormat("%s%cbeep.wav", program.apppath, FILESEP));
	return ret;
}

int decreaseTimers()
{
	int ret = 0;
	if(interpreter.tDelay > 0) interpreter.tDelay--;
	if(interpreter.tSound > 0) interpreter.tSound--;
	if(interpreter.tSound > 0) ret = soundBeep();

	return ret;
}

void keyPressed(int key)
{
	interpreter.keys[key] = 1;
}

void keyReleased(int key)
{
	interpreter.keys[key] = 0;
}

int cpuIsKeyPressed()
{
	int i = 0;
	for(i = 0; i < 16; i++) {
		if(interpreter.keys[i] > 0) return i;
	}

	return -1;
}

void cpuOp00E0()
{
	memset(interpreter.screen, 0x00, sizeof(interpreter.screen));
}

void cpuOp00EE()
{
	if(interpreter.sp > 0) interpreter.pc = interpreter.stack[--interpreter.sp];
}

void cpuOp1NNN(unsigned short op)
{
	interpreter.pc = getNNN(op);
}

void cpuOp2NNN(unsigned short op)
{
	if(interpreter.sp < 16) {
		interpreter.stack[interpreter.sp++] = interpreter.pc;
		interpreter.pc = getNNN(op);
	}
}

void cpuOp3XKK(unsigned short op)
{
	if(interpreter.regs[getX(op)] == getKK(op)) interpreter.pc += 2;
}

void cpuOp4XKK(unsigned short op)
{
	if(interpreter.regs[getX(op)] != getKK(op)) interpreter.pc += 2;
}

void cpuOp5XY0(unsigned short op)
{
	if(interpreter.regs[getX(op)] == interpreter.regs[getY(op)]) interpreter.pc += 2;
}

void cpuOp6XKK(unsigned short op)
{
	interpreter.regs[getX(op)] = getKK(op);
}

void cpuOp7XKK(unsigned short op)
{
	interpreter.regs[getX(op)] += getKK(op);
}

void cpuOp8XY0(unsigned short op)
{
	interpreter.regs[getX(op)] = interpreter.regs[getY(op)];
}

void cpuOp8XY1(unsigned short op)
{
	interpreter.regs[getX(op)] = (interpreter.regs[getX(op)] | interpreter.regs[getY(op)]);
}

void cpuOp8XY2(unsigned short op)
{
	interpreter.regs[getX(op)] = (interpreter.regs[getX(op)] & interpreter.regs[getY(op)]);
}

void cpuOp8XY3(unsigned short op)
{
	interpreter.regs[getX(op)] = (interpreter.regs[getX(op)] ^ interpreter.regs[getY(op)]);
}

void cpuOp8XY4(unsigned short op)
{
	unsigned char vx = interpreter.regs[getX(op)];
	unsigned char vy = interpreter.regs[getY(op)];
	int result = vx + vy;

	interpreter.regs[0xF] = 0;
	if(result > 255) interpreter.regs[0xF] = 1;

	interpreter.regs[getX(op)] = result & 0xFF;
}

void cpuOp8XY5(unsigned short op)
{
	unsigned char vx = interpreter.regs[getX(op)];
	unsigned char vy = interpreter.regs[getY(op)];
	int result = vx - vy;

	interpreter.regs[0xF] = 0;
	if(vx >= vy) interpreter.regs[0xF] = 1;

	interpreter.regs[getX(op)] = result & 0xFF;
}

void cpuOp8XY6(unsigned short op)
{
	interpreter.regs[0xF] = interpreter.regs[getX(op)] & 0x1;
	interpreter.regs[getX(op)] >>= 1;
}

void cpuOp8XY7(unsigned short op)
{
	unsigned char vx = interpreter.regs[getX(op)];
	unsigned char vy = interpreter.regs[getY(op)];
	int result = vy - vx;

	interpreter.regs[0xF] = 0;
	if(vy >= vx) interpreter.regs[0xF] = 1;

	interpreter.regs[getX(op)] = result & 0xFF;
}

void cpuOp8XYE(unsigned short op)
{
	interpreter.regs[0xF] = interpreter.regs[getX(op)] >> 7;
	interpreter.regs[getX(op)] <<= 1;
}

void cpuOp9XY0(unsigned short op)
{
	if(interpreter.regs[getX(op)] != interpreter.regs[getY(op)]) interpreter.pc += 2;
}

void cpuOpANNN(unsigned short op)
{
	interpreter.addri = getNNN(op);
}

void cpuOpBNNN(unsigned short op)
{
	interpreter.pc = interpreter.regs[0x0] + getNNN(op);
}

void cpuOpCXKK(unsigned short op)
{
	unsigned char rnd = (rand() % 255);
	interpreter.regs[getX(op)] = rnd & getKK(op);
}

void cpuOpDXYN(unsigned short op)
{
	int cx = interpreter.regs[(getX(op))], cy = interpreter.regs[(getY(op))], h = getN(op);
	interpreter.regs[0xF] = 0;

	int yline = 0;
	for(yline = 0; yline < h; yline++) {
		unsigned char data = interpreter.memory[interpreter.addri + yline];
		int xpixel = 0;
		for(xpixel = 0; xpixel < 8; xpixel++, data<<=1) {
			if(data & 0x80) {
				int x = ((cx + xpixel) % 64);
				int y = ((cy + yline) % 32);
				if(interpreter.screen[x][y] == 1) interpreter.regs[0xF] = 1;
				interpreter.screen[x][y] ^= 1;
			}
		}
	}
}

void cpuOpEX9E(unsigned short op)
{
	unsigned char key = interpreter.regs[getX(op)];
	if(interpreter.keys[key]) interpreter.pc += 2;
}

void cpuOpEXA1(unsigned short op)
{
	unsigned char key = interpreter.regs[getX(op)];
	if(!interpreter.keys[key]) interpreter.pc += 2;
}

void cpuOpFX07(unsigned short op)
{
	interpreter.regs[getX(op)] = interpreter.tDelay;
}

void cpuOpFX0A(unsigned short op)
{
	int keydown = cpuIsKeyPressed();

	if(keydown == -1) {
		interpreter.pc -= 2;
	} else {
		interpreter.regs[getX(op)] = keydown;
		interpreter.keys[keydown] = 0;
	}
}

void cpuOpFX15(unsigned short op)
{
	interpreter.tDelay = interpreter.regs[getX(op)];
}

void cpuOpFX18(unsigned short op)
{
	interpreter.tSound = interpreter.regs[getX(op)];
}

void cpuOpFX1E(unsigned short op)
{
	interpreter.addri += interpreter.regs[getX(op)];
	if(interpreter.addri > 0xFFF) interpreter.regs[0xF] = 1;
}

void cpuOpFX29(unsigned short op)
{
	interpreter.addri = interpreter.regs[getX(op)] * 5;
}

void cpuOpFX33(unsigned short op)
{
	int val = interpreter.regs[getX(op)];

	int v1 = val / 100;
	int v2 = (val / 10) % 10;
	int v3 = val % 10;

	interpreter.memory[interpreter.addri] = v1;
	interpreter.memory[interpreter.addri + 1] = v2;
	interpreter.memory[interpreter.addri + 2] = v3;
}

void cpuOpFX55(unsigned short op)
{
	int i = 0;
	for(i = 0; i <= getX(op); i++) {
		if(interpreter.addri + i < 0xFFF) interpreter.memory[interpreter.addri + i] = interpreter.regs[i];
	}

	interpreter.addri += getX(op);
}

void cpuOpFX65(unsigned short op)
{
	int i = 0;
	for(i = 0; i <= getX(op); i++) {
		if(interpreter.addri + i < 0xFFF) interpreter.regs[i] = interpreter.memory[interpreter.addri + i];
	}

	interpreter.addri += getX(op);
}

void cpuOpFX75(unsigned short op)
{
	int i = 0;
	for(i = 0; i <= getX(op); i++) {
		interpreter.hpf[i] = interpreter.regs[i];
	}
}

void cpuOpFX85(unsigned short op)
{
	int i = 0;
	for(i = 0; i <= getX(op); i++) {
		interpreter.regs[i] = interpreter.hpf[i];
	}
}

void cpuRun()
{
	unsigned short op = ((interpreter.memory[interpreter.pc] << 8) | interpreter.memory[interpreter.pc + 1]);
/*
	printf("%04X: [%04X] - I:%04X - ",
		interpreter.pc, op, interpreter.addri);
	printf("V0:%02X V1:%02X V2:%02X V3:%02X V4:%02X V5:%02X V6:%02X V7:%02X V8:%02X V9:%02X VA:%02X VB:%02X VC:%02X VD:%02X VE:%02X VF:%02X\n",
		interpreter.regs[0], interpreter.regs[1], interpreter.regs[2], interpreter.regs[3],
		interpreter.regs[4], interpreter.regs[5], interpreter.regs[6], interpreter.regs[7],
		interpreter.regs[8], interpreter.regs[9], interpreter.regs[10], interpreter.regs[11],
		interpreter.regs[12], interpreter.regs[13], interpreter.regs[14], interpreter.regs[15]);
*/
	interpreter.pc += 2;

	switch(op & 0xF000) {
		case 0x0000: {
			switch(getN(op)) {
				case 0x0000: cpuOp00E0(); break;
				case 0x000E: cpuOp00EE(); break;
			}
			break;
		}
		case 0x1000: cpuOp1NNN(op); break;
		case 0x2000: cpuOp2NNN(op); break;
		case 0x3000: cpuOp3XKK(op); break;
		case 0x4000: cpuOp4XKK(op); break;
		case 0x5000: cpuOp5XY0(op); break;
		case 0x6000: cpuOp6XKK(op); break;
		case 0x7000: cpuOp7XKK(op); break;
		case 0x8000: {
			switch(getN(op)) {
				case 0x0000: cpuOp8XY0(op); break;
				case 0x0001: cpuOp8XY1(op); break;
				case 0x0002: cpuOp8XY2(op); break;
				case 0x0003: cpuOp8XY3(op); break;
				case 0x0004: cpuOp8XY4(op); break;
				case 0x0005: cpuOp8XY5(op); break;
				case 0x0006: cpuOp8XY6(op); break;
				case 0x0007: cpuOp8XY7(op); break;
				case 0x000E: cpuOp8XYE(op); break;
			}
			break;
		}
		case 0x9000: cpuOp9XY0(op); break;
		case 0xA000: cpuOpANNN(op); break;
		case 0xB000: cpuOpBNNN(op); break;
		case 0xC000: cpuOpCXKK(op); break;
		case 0xD000: cpuOpDXYN(op); break;
		case 0xE000: {
			switch(getKK(op)) {
				case 0x009E: cpuOpEX9E(op); break;
				case 0x00A1: cpuOpEXA1(op); break;
			}
			break;
		}
		case 0xF000: {
			switch(getKK(op)) {
				case 0x0007: cpuOpFX07(op); break;
				case 0x000A: cpuOpFX0A(op); break;
				case 0x0015: cpuOpFX15(op); break;
				case 0x0018: cpuOpFX18(op); break;
				case 0x001E: cpuOpFX1E(op); break;
				case 0x0029: cpuOpFX29(op); break;
				case 0x0033: cpuOpFX33(op); break;
				case 0x0055: cpuOpFX55(op); break;
				case 0x0065: cpuOpFX65(op); break;
				case 0x0075: cpuOpFX75(op); break;
				case 0x0085: cpuOpFX85(op); break;
			}
			break;
		}
		default: break;
	}
}

void saveState()
{
	//
}

void loadState()
{
	//
}
