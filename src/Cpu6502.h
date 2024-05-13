// Copyright (c) 2024 FBLabs
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

#pragma once

#include "Common.h"
#include "Bus.h"

/* Typedefs */

union UReg16bit {
	struct {
		byte low;
		byte hi;
	};
	word value;
};

/*************************************************************************************************/
class CCpu6502 final : public CDevice {
public:
	CCpu6502(CBus& bus);
	byte read(const word addr, const uint64_t cycles) override {
		return 0xFF;
	};
	void write(const word addr, const byte data, const uint64_t cycles) override {};
	void update(const uint64_t cycles) override {};
	void reset() override;
public:
	uint64_t getCumulativeCycles() const;
	void setClock(const unsigned long clock);
	unsigned long getClock() const;
	void setFullSpeed(const bool val);
	bool getFullSpeed() const;
	double getClockRate() const;
	void executeOpcode();
private:
	CBus& mBus;
	unsigned long mClock{ CPU_CLOCK };
	bool mFullSpeed{ false };
	uint64_t mCumulativeCycles{ 0 };
	int interruptFlags{ 0 };
private:
	inline byte readByte(word address) const {
		return mBus.readByte(address, mCumulativeCycles);
	}

	inline word readWord(word address) const {
		return mBus.readWord(address, mCumulativeCycles);
	}

	inline void writeByte(word address, byte value) const {
		mBus.writeByte(address, value, mCumulativeCycles);
	}

	inline void writeWord(word address, word value) const {
		mBus.writeWord(address, value, mCumulativeCycles);
	}
private:
	// Flags Interrupts
	const byte INT_NMI{ (1 << 0) };
	const byte INT_IRQ{ (1 << 1) };

	// ALU look up tables
	word m_BCDTableAdd[512];			// addition
	word m_BCDTableSub[512];			// subtraction

	// Flags bitmask
	const byte FLAG_C{ (1 << 0) };
	const byte FLAG_Z{ (1 << 1) };
	const byte FLAG_I{ (1 << 2) };
	const byte FLAG_D{ (1 << 3) };
	const byte FLAG_B{ (1 << 4) };
	const byte FLAG_V{ (1 << 6) };
	const byte FLAG_N{ (1 << 7) };

	// Registers
	byte mRegA{ 0 };
	byte mRegX{ 0 };
	byte mRegY{ 0 };
	byte mRegS{ 0xFF };
	byte mRegFlags{ 0x20 };	// On an 6502, bit 5 always must be 1
	UReg16bit mRegPC{ 0 };

	// Stack macros

	inline byte stackPop() {
		return readByte(0x100 | ++mRegS);
	}

	inline void stackPush(byte v) {
		writeByte(0x100 | mRegS--, v);
	}

	// Flags utils

	inline void setN(bool b) {
		if (b) {
			mRegFlags |= FLAG_N;
		} else {
			mRegFlags &= ~FLAG_N;
		}
	}

	inline void setV(bool b) {
		if (b) {
			mRegFlags |= FLAG_V;
		} else {
			mRegFlags &= ~FLAG_V;
		}
	}

	inline void setB(bool b) {
		if (b) {
			mRegFlags |= FLAG_B;
		} else {
			mRegFlags &= ~FLAG_B;
		}
	}

	inline void setD(bool b) {
		if (b) {
			mRegFlags |= FLAG_D;
		} else {
			mRegFlags &= ~FLAG_D;
		}
	}

	inline void setI(bool b) {
		if (b) {
			mRegFlags |= FLAG_I;
		} else {
			mRegFlags &= ~FLAG_I;
		}
	}

	inline void setZ(bool b) {
		if (b) {
			mRegFlags |= FLAG_Z;
		} else {
			mRegFlags &= ~FLAG_Z;
		}
	}

	inline void setC(bool b) {
		if (b) {
			mRegFlags |= FLAG_C;
		} else {
			mRegFlags &= ~FLAG_C;
		}
	}

	inline void setNZ(byte val) {
		setN((val & 0x80) != 0);
		setZ(val == 0);
	}

	inline bool getN() const noexcept {
		return ((mRegFlags & FLAG_N) != 0);
	}

	inline bool getV() const noexcept {
		return ((mRegFlags & FLAG_V) != 0);
	}

	inline bool getD() const noexcept {
		return ((mRegFlags & FLAG_D) != 0);
	}

	inline bool getI() const noexcept {
		return ((mRegFlags & FLAG_I) != 0);
	}

	inline bool getZ() const noexcept {
		return ((mRegFlags & FLAG_Z) != 0);
	}

	inline bool getC() const noexcept {
		return ((mRegFlags & FLAG_C) != 0);
	}

	// Inline for effective address calculation

	inline byte eaimm() {
		return readByte(mRegPC.value++);
	}

	inline byte eazp() {
		return readByte(mRegPC.value++);
	}

	inline byte eazpx() {
		return readByte(mRegPC.value++) + mRegX;
	}

	inline byte eazpy() {
		return readByte(mRegPC.value++) + mRegY;
	}

	inline word eaabs() {
		word result = readWord(mRegPC.value);
		mRegPC.value += 2;
		return result;
	}

	inline char earel() {
		return (char)readByte(mRegPC.value++);
	}

	inline word eaabsx() {
		return eaabs() + mRegX;
	}

	inline word eaabsy() {
		return eaabs() + mRegY;
	}

	// Indirect addressing

	inline word eaabsind() {
		return readWord(eaabs());
	}

	inline word eazpxind() {
		return readWord(eazpx());
	}

	inline word eazpindy() {
		return readWord(eaimm()) + mRegY;
	}

	// Misc. macros
	inline void accCycles(int cycles) noexcept {
		mCumulativeCycles += cycles;
	}
	inline void branch(signed char operand) noexcept {
		mRegPC.value += (operand);
		accCycles(1);
	}
};

