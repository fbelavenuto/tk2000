// Copyright (c) 2020 FBLabs
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "pch.h"
#include "Cpu6502.h"

/* Constants */


/*************************************************************************************************/
CCpu6502::CCpu6502(CBus *bus) : mBus(bus) {
	assert(mBus != nullptr);

	for (int i = 0; i < 512; i++) {
		m_BCDTableAdd[i] = ((i & 0x0F) <= 0x09) ? i : (i + 0x06);
		m_BCDTableAdd[i] += ((m_BCDTableAdd[i] & 0xf0) <= 0x90) ? 0 : 0x60;
		if (m_BCDTableAdd[i] > 0x1ff) {
			m_BCDTableAdd[i] -= 0x100;
		}
		m_BCDTableSub[i] = ((i & 0x0f) <= 0x09) ? i : (i - 0x06);
		m_BCDTableSub[i] -= ((m_BCDTableSub[i] & 0xf0) <= 0x90) ? 0 : 0x60;
	}
}

/*************************************************************************************************/
CCpu6502::~CCpu6502() {

}

/*************************************************************************************************/
unsigned long long CCpu6502::getCumulativeCycles() const {
	return mCumulativeCycles;
}

/*************************************************************************************************/
void CCpu6502::setClock(const unsigned long clock) {
	assert(clock > 0);
	mClock = clock;
}

/*************************************************************************************************/
unsigned long CCpu6502::getClock() const {
	return mClock;
}

/*************************************************************************************************/
void CCpu6502::setFullSpeed(const bool val) {
	mFullSpeed = val;
}

/*************************************************************************************************/
bool CCpu6502::getFullSpeed() const {
	return mFullSpeed;
}

/*************************************************************************************************/
void CCpu6502::reset() {
	interruptFlags = 0;	// clear all interrupt flags
	mRegA = mRegX = mRegY = 0;
	mRegFlags = 0x20;
	mRegS = 0xFF;
	mRegPC.value = readWord(0xFFFC);
	mCumulativeCycles = 0;
}

/*************************************************************************************************/
void CCpu6502::executeOpcode() {
	byte operand;
	word result, operandAddr;

	//fprintf(stderr, "PC = %04X\n", mRegPC.value);
	const byte opcode = readByte(this->mRegPC.value++);
	switch (opcode) {

	case 0x69: // ADC #imm
		operand = eaimm();
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x6D: // ADC abs
		operand = readByte(eaabs());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x65: // ADC zp
		operand = readByte(eazp());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(3);
		break;

	case 0x61: // ADC (zp,X)
		operand = readByte(eazpxind());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(6);
		break;

	case 0x71: // ADC (zp),Y
		operandAddr = eazpindy();
		operand = readByte(operandAddr);
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(5);
		break;

	case 0x75: // ADC zp,X
		operandAddr = eazpx();
		operand = readByte(operandAddr);
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x7D: // ADC abs,X
		operand = readByte(eaabsx());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x79: // ADC abs,Y
		operand = readByte(eaabsy());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableAdd[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x29: // AND #imm
		mRegA &= eaimm();
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x2D: // AND abs
		mRegA &= readByte(eaabs());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x25: // AND zp
		mRegA &= readByte(eazp());
		setNZ(mRegA);
		accCycles(3);
		break;

	case 0x21: // AND (zp,X)
		mRegA &= readByte(eazpxind());
		setNZ(mRegA);
		accCycles(6);
		break;

	case 0x31: // AND (zp),Y
		mRegA &= readByte(eazpindy());
		setNZ(mRegA);
		accCycles(5);
		break;

	case 0x35: // AND zp,X
		mRegA &= readByte(eazpx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x3D: // AND abs,X
		mRegA &= readByte(eaabsx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x39: // AND abs,Y
		mRegA &= readByte(eaabsy());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x0E: // ASL abs
		operandAddr = eaabs();
		result = readByte(operandAddr) << 1;
		setC((result & 0x100) != 0);
		operand = result & 0xFF;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x06: // ASL zp
		operandAddr = eazp();
		result = readByte(operandAddr) << 1;
		setC((result & 0x100) != 0);
		operand = result & 0xFF;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(5);
		break;

	case 0x0A: // ASL acc
		result = mRegA << 1;
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x16: // ASL zp,X
		operandAddr = eazpx();
		result = readByte(operandAddr) << 1;
		setC((result & 0x100) != 0);
		operand = result & 0xFF;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x1E: // ASL abs,X
		operandAddr = eaabsx();
		result = readByte(operandAddr) << 1;
		setC((result & 0x100) != 0);
		operand = result & 0xFF;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(7);
		break;

	case 0x90: // BCC rr
		operand = earel();
		accCycles(2);
		if (!getC())
			branch(operand);
		break;

	case 0xB0: // BCS rr
		operand = earel();
		accCycles(2);
		if (getC())
			branch(operand);
		break;

	case 0xF0: // BEQ rr
		operand = earel();
		accCycles(2);
		if (getZ())
			branch(operand);
		break;

	case 0x2C: // BIT abs
		operand = readByte(eaabs());
		setN((operand & 0x80) != 0);
		setV((operand & 0x40) != 0);
		setZ((operand & mRegA) == 0);
		accCycles(4);
		break;

	case 0x24: // BIT zp
		operand = readByte(eazp());
		setN((operand & 0x80) != 0);
		setV((operand & 0x40) != 0);
		setZ((operand & mRegA) == 0);
		accCycles(3);
		break;

	case 0x30: // BMI rr
		operand = earel();
		accCycles(2);
		if (getN())
			branch(operand);
		break;

	case 0xD0: // BNE rr
		operand = earel();
		accCycles(2);
		if (!getZ())
			branch(operand);
		break;

	case 0x10: // BPL rr
		operand = earel();
		accCycles(2);
		if (!getN())
			branch(operand);
		break;

	case 0x00: // BRK
assert(0);
		stackPush(mRegPC.hi); // save PCH, PCL & P
		stackPush(mRegPC.low);
		setN(getN());
		setZ(getZ());
		setC(getC());
		setB(true);
		stackPush(mRegFlags);
		setI(true);
		mRegPC.value = readWord(0xFFFE);
		accCycles(7);
		break;

	case 0x50: // BVC rr
		operand = earel();
		accCycles(2);
		if (!getV())
			branch(operand);
		break;

	case 0x70: // BVS rr
		operand = earel();
		accCycles(2);
		if (getV())
			branch(operand);
		break;

	case 0x18: // CLC rr
		setC(false);
		accCycles(2);
		break;

	case 0xD8: // CLD
		setD(false);
		accCycles(2);
		break;

	case 0x58: // CLI
		setI(false);
		accCycles(2);
		break;

	case 0xB8: // CLV
		setV(false);
		accCycles(2);
		break;

	case 0xC9: // CMP #imm
		result = mRegA - eaimm();
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(2);
		break;

	case 0xCD: // CMP abs
		result = mRegA - readByte(eaabs());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(4);
		break;

	case 0xC5: // CMP zp
		result = mRegA - readByte(eazp());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(3);
		break;

	case 0xC1: // CMP (zp,X)
		result = mRegA - readByte(eazpxind());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(6);
		break;

	case 0xD1: // CMP (zp),Y
		result = mRegA - readByte(eazpindy());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(5);
		break;

	case 0xD5: // CMP zp,X
		result = mRegA - readByte(eazpx());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(4);
		break;

	case 0xDD: // CMP abs,X
		result = mRegA - readByte(eaabsx());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(4);
		break;

	case 0xD9: // CMP abs,Y
		result = mRegA - readByte(eaabsy());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(4);
		break;

	case 0xE0: // CPX #imm
		result = mRegX - eaimm();
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(2);
		break;

	case 0xEC: // CPX abs
		result = mRegX - readByte(eaabs());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(4);
		break;

	case 0xE4: // CPX zp
		result = mRegX - readByte(eazp());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(3);
		break;

	case 0xC0: // CPY #imm
		result = mRegY - eaimm();
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(2);
		break;

	case 0xCC: // CPY abs
		result = mRegY - readByte(eaabs());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(4);
		break;

	case 0xC4: // CPY zp
		result = mRegY - readByte(eazp());
		setC((result & 0x100) == 0);
		operand = (byte)result;
		setNZ(operand);
		accCycles(3);
		break;

	case 0xCE: // DEC abs
		operandAddr = eaabs();
		operand = readByte(operandAddr) - 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0xC6: // DEC zp
		operandAddr = eazp();
		operand = readByte(operandAddr) - 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(5);
		break;

	case 0xD6: // DEC zp,X
		operandAddr = eazpx();
		operand = readByte(operandAddr) - 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0xDE: // DEC abs,X
		operandAddr = eaabsx();
		operand = readByte(operandAddr) - 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(7);
		break;

	case 0xCA: // DEX
		--mRegX;
		setNZ(mRegX);
		accCycles(2);
		break;

	case 0x88: // DEY
		--mRegY;
		setNZ(mRegY);
		accCycles(2);
		break;

	case 0x49: // EOR #imm
		mRegA ^= eaimm();
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x4D: // EOR abs
		mRegA ^= readByte(eaabs());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x45: // EOR zp
		mRegA ^= readByte(eazp());
		setNZ(mRegA);
		accCycles(3);
		break;

	case 0x41: // EOR (zp,X)
		mRegA ^= readByte(eazpxind());
		setNZ(mRegA);
		accCycles(6);
		break;

	case 0x51: // EOR (zp),Y
		mRegA ^= readByte(eazpindy());
		setNZ(mRegA);
		accCycles(5);
		break;

	case 0x55: // EOR zp,X
		mRegA ^= readByte(eazpx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x5D: // EOR abs,X
		mRegA ^= readByte(eaabsx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x59: // EOR abs,Y
		mRegA ^= readByte(eaabsy());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xEE: // INC abs
		operandAddr = eaabs();
		operand = readByte(operandAddr) + 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0xE6: // INC zp
		operandAddr = eazp();
		operand = readByte(operandAddr) + 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(5);
		break;

	case 0xF6: // INC zp,X
		operandAddr = eazpx();
		operand = readByte(operandAddr) + 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0xFE: // INC abs,X
		operandAddr = eaabsx();
		operand = readByte(operandAddr) + 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(7);
		break;

	case 0xE8: // INX
		++mRegX;
		setNZ(mRegX);
		accCycles(2);
		break;

	case 0xC8: // INY
		++mRegY;
		setNZ(mRegY);
		accCycles(2);
		break;

	case 0x4C: // JMP abs
		mRegPC.value = eaabs();
		accCycles(3);
		break;

	case 0x6C: // JMP (abs)
		mRegPC.value = eaabsind();
		accCycles(5);
		break;

	case 0x20: // JSR abs
		operandAddr = eaabs();
		--mRegPC.value;
		stackPush(mRegPC.hi);
		stackPush(mRegPC.low);
		mRegPC.value = operandAddr;
		accCycles(6);
		break;

	case 0xA9: // LDA #imm
		mRegA = eaimm();
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0xAD: // LDA abs
		mRegA = readByte(eaabs());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xA5: // LDA zp
		mRegA = readByte(eazp());
		setNZ(mRegA);
		accCycles(3);
		break;

	case 0xA1: // LDA (zp,X)
		mRegA = readByte(eazpxind());
		setNZ(mRegA);
		accCycles(6);
		break;

	case 0xB1: // LDA (zp),Y
		mRegA = readByte(eazpindy());
		setNZ(mRegA);
		accCycles(5);
		break;

	case 0xB5: // LDA zp,X
		mRegA = readByte(eazpx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xBD: // LDA abs,X
		mRegA = readByte(eaabsx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xB9: // LDA abs,Y
		mRegA = readByte(eaabsy());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xA2: // LDX #imm
		mRegX = eaimm();
		setNZ(mRegX);
		accCycles(2);
		break;

	case 0xAE: // LDX abs
		mRegX = readByte(eaabs());
		setNZ(mRegX);
		accCycles(4);
		break;

	case 0xA6: // LDX zp
		mRegX = readByte(eazp());
		setNZ(mRegX);
		accCycles(3);
		break;

	case 0xBE: // LDX abs,Y
		mRegX = readByte(eaabsy());
		setNZ(mRegX);
		accCycles(4);
		break;

	case 0xB6: // LDX zp,Y
		mRegX = readByte(eazpy());
		setNZ(mRegX);
		accCycles(4);
		break;

	case 0xA0: // LDY #imm
		mRegY = eaimm();
		setNZ(mRegY);
		accCycles(2);
		break;

	case 0xAC: // LDY abs
		mRegY = readByte(eaabs());
		setNZ(mRegY);
		accCycles(4);
		break;

	case 0xA4: // LDY zp
		mRegY = readByte(eazp());
		setNZ(mRegY);
		accCycles(3);
		break;

	case 0xB4: // LDY zp,X
		mRegY = readByte(eazpx());
		setNZ(mRegY);
		accCycles(4);
		break;

	case 0xBC: // LDY abs,X
		mRegY = readByte(eaabsx());
		setNZ(mRegY);
		accCycles(4);
		break;

	case 0x4E: // LSR abs
		operandAddr = eaabs();
		operand = readByte(operandAddr);
		setC((operand & 0x01) != 0);
		operand >>= 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x46: // LSR zp
		operandAddr = eazp();
		operand = readByte(operandAddr);
		setC((operand & 0x01) != 0);
		operand >>= 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(5);
		break;

	case 0x4A: // LSR acc
		setC((mRegA & 0x01) != 0);
		mRegA >>= 1;
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x56: // LSR zp,X
		operandAddr = eazpx();
		operand = readByte(operandAddr);
		setC((operand & 0x01) != 0);
		operand >>= 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x5E: // LSR abs,X
		operandAddr = eaabsx();
		operand = readByte(operandAddr);
		setC((operand & 0x01) != 0);
		operand >>= 1;
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(7);
		break;

	case 0xEA: // NOP
		accCycles(2);
		break;

	case 0x09: // ORA #imm
		mRegA |= eaimm();
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x0D: // ORA abs
		mRegA |= readByte(eaabs());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x05: // ORA zp
		mRegA |= readByte(eazp());
		setNZ(mRegA);
		accCycles(3);
		break;

	case 0x01: // ORA (zp,X)
		mRegA |= readByte(eazpxind());
		setNZ(mRegA);
		accCycles(6);
		break;

	case 0x11: // ORA (zp),Y
		mRegA |= readByte(eazpindy());
		setNZ(mRegA);
		accCycles(5);
		break;

	case 0x15: // ORA zp,X
		mRegA |= readByte(eazpx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x1D: // ORA abs,X
		mRegA |= readByte(eaabsx());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x19: // ORA abs,Y
		mRegA |= readByte(eaabsy());
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x48: // PHA
		stackPush(mRegA);
		accCycles(3);
		break;

	case 0x08: // PHP
		setN(getN());
		setZ(getZ());
		setC(getC());
		stackPush(mRegFlags);
		accCycles(3);
		break;

	case 0x68: // PLA
		mRegA = stackPop();
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x28: // PLP
		mRegFlags = stackPop() | 0x20; // fix bug in bit5 of P
		accCycles(4);
		break;

	case 0x2E: // ROL abs
		operandAddr = eaabs();
		result = readByte(operandAddr) << 1;
		operand = (result & 0xFF) | (getC() ? 1 : 0);
		setC((operand & 0x100) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x26: // ROL zp
		operandAddr = eazp();
		result = readByte(operandAddr) << 1;
		operand = (result & 0xFF) | (getC() ? 1 : 0);
		setC((result & 0x100) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(5);
		break;

	case 0x2A: // ROL acc
		result = mRegA << 1;
		mRegA = (result & 0xFF) | (getC() ? 1 : 0);
		setC((result & 0x100) != 0);
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x36: // ROL zp,X
		operandAddr = eazpx();
		result = readByte(operandAddr) << 1;
		operand = (result & 0xFF) | (getC() ? 1 : 0);
		setC((result & 0x100) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x3E: // ROL abs,X
		operandAddr = eaabsx();
		result = readByte(operandAddr) << 1;
		operand = (result & 0xFF) | (getC() ? 1 : 0);
		setC((result & 0x100) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(7);
		break;

	case 0x6E: // ROR abs
		operandAddr = eaabs();
		result = readByte(operandAddr);
		operand = (result >> 1) | (getC() ? 0x80 : 0);
		setC((result & 0x01) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x66: // ROR zp
		operandAddr = eazp();
		result = readByte(operandAddr);
		operand = (result >> 1) | (getC() ? 0x80 : 0);
		setC((result & 0x01) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(5);
		break;

	case 0x6A: // ROR acc
		result = mRegA;
		mRegA = (result >> 1) | (getC() ? 0x80 : 0);
		setC((result & 0x01) != 0);
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x76: // ROR zp,X
		operandAddr = eazpx();
		result = readByte(operandAddr);
		operand = (result >> 1) | (getC() ? 0x80 : 0);
		setC((result & 0x01) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(6);
		break;

	case 0x7E: // ROR abs,X
		operandAddr = eaabsx();
		result = readByte(operandAddr);
		operand = (result >> 1) | (getC() ? 0x80 : 0);
		setC((result & 0x01) != 0);
		setNZ(operand);
		writeByte(operandAddr, operand);
		accCycles(7);
		break;

	case 0x40: // RTI
		mRegFlags = stackPop() | 0x20; // bit 5 bug of 6502
		mRegPC.low = stackPop();
		mRegPC.hi = stackPop();
		accCycles(6);
		break;

	case 0x60: // RTS
		mRegPC.low = stackPop();
		mRegPC.hi = stackPop();
		++mRegPC.value;
		accCycles(6);
		break;

	case 0xE9: // SBC #imm
		operand = 255 - eaimm();
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0xED: // SBC abs
		operand = 255 - readByte(eaabs());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xE5: // SBC zp
		operand = 255 - readByte(eazp());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(3);
		break;

	case 0xE1: // SBC (zp,X)
		operand = 255 - readByte(eazpxind());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(6);
		break;

	case 0xF1: // SBC (zp),Y
		operand = 255 - readByte(eazpindy());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(5);
		break;

	case 0xF5: // SBC zp,X
		operand = 255 - readByte(eazpx());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xFD: // SBC abs,X
		operand = 255 - readByte(eaabsx());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0xF9: // SBC abs,Y
		operand = 255 - readByte(eaabsy());
		result = operand + mRegA + (getC() ? 1 : 0);
		setV(!(((operand ^ mRegA) & 0x80) != 0)
			&& (((mRegA ^ result) & 0x80) != 0));
		if (getD()) {
			result = m_BCDTableSub[result];
		}
		setC((result & 0x100) != 0);
		mRegA = result & 0xFF;
		setNZ(mRegA);
		accCycles(4);
		break;

	case 0x38: // SEC
		setC(true);
		accCycles(2);
		break;

	case 0xF8: // SED
		setD(true);
		accCycles(2);
		break;

	case 0x78: // SEI
		setI(true);
		accCycles(2);
		break;

	case 0x8D: // STA abs
		writeByte(eaabs(), mRegA);
		accCycles(4);
		break;

	case 0x85: // STA zp
		writeByte(eazp(), mRegA);
		accCycles(3);
		break;

	case 0x81: // STA (zp,X)
		writeByte(eazpxind(), mRegA);
		accCycles(6);
		break;

	case 0x91: // STA (zp),Y
		writeByte(eazpindy(), mRegA);
		accCycles(6);
		break;

	case 0x95: // STA zp,X
		writeByte(eazpx(), mRegA);
		accCycles(4);
		break;

	case 0x9D: // STA abs,X
		writeByte(eaabsx(), mRegA);
		accCycles(5);
		break;

	case 0x99: // STA abs,Y
		writeByte(eaabsy(), mRegA);
		accCycles(5);
		break;

	case 0x8E: // STX abs
		writeByte(eaabs(), mRegX);
		accCycles(4);
		break;

	case 0x86: // STX zp
		writeByte(eazp(), mRegX);
		accCycles(3);
		break;

	case 0x96: // STX zp,Y
		writeByte(eazpy(), mRegX);
		accCycles(4);
		break;

	case 0x8C: // STY abs
		writeByte(eaabs(), mRegY);
		accCycles(4);
		break;

	case 0x84: // STY zp
		writeByte(eazp(), mRegY);
		accCycles(3);
		break;

	case 0x94: // STY zp,X
		writeByte(eazpx(), mRegY);
		accCycles(4);
		break;

	case 0xAA: // TAX
		mRegX = mRegA;
		setNZ(mRegX);
		accCycles(2);
		break;

	case 0xA8: // TAY
		mRegY = mRegA;
		setNZ(mRegY);
		accCycles(2);
		break;

	case 0xBA: // TSX
		mRegX = mRegS;
		setNZ(mRegX);
		accCycles(2);
		break;

	case 0x8A: // TXA
		mRegA = mRegX;
		setNZ(mRegA);
		accCycles(2);
		break;

	case 0x9A: // TXS
		mRegS = mRegX;
		accCycles(2);
		break;

	case 0x98: // TYA
		mRegA = mRegY;
		setNZ(mRegA);
		accCycles(2);
		break;

	default: // unknown instructions
		accCycles(2);
	}
	// Check interrupts
	if ((interruptFlags & INT_NMI) != 0) {
		interruptFlags &= ~INT_NMI;
		stackPush(mRegPC.hi);
		stackPush(mRegPC.low);
		stackPush(mRegFlags);
		mRegPC.value = readWord(0xFFFA);
		accCycles(6);
	}
	if ((interruptFlags & INT_IRQ) != 0 && getI()) {
		interruptFlags &= ~INT_IRQ;
		stackPush(mRegPC.hi);
		stackPush(mRegPC.low);
		stackPush(mRegFlags);
		setI(false);
		mRegPC.value = readWord(0xFFFE);
		accCycles(6);
	}
}

