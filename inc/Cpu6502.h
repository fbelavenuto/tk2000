
#pragma once

#include "DataTypes.h"
#include "Bus.h"

/* Typedefs */
typedef union {
	struct {
		byte low;
		byte hi;
	};
	word value;
} TReg16bit;

/*************************************************************************************************/
class CCpu6502 {
public:
	CCpu6502(CBus *bus);
	~CCpu6502();
	unsigned long long getCumulativeCycles();
	void setClock(unsigned long clock);
	unsigned long getClock();
	void setFullSpeed(bool val);
	bool getFullSpeed();
	void executeOpcode();
	void reset();
private:
	CBus *mBus = nullptr;
	unsigned long mClock;
	bool mFullSpeed = false;
	unsigned long long mCumulativeCycles = 0;
	int interruptFlags = 0;
	
	inline byte readByte(word address) {
		return mBus->readByte(address);
	}
	
	inline word readWord(word address) {
		return mBus->readWord(address);
	}

	inline void writeByte(word address, byte value) {
		mBus->writeByte(address, value);
	}

	inline void writeWord(word address, word value) {
		mBus->writeWord(address, value);
	}

	// Flags Interrupts
	const byte INT_NMI = (1 << 0);
	const byte INT_IRQ = (1 << 1);

	// ALU look up tables
	word m_BCDTableAdd[512];			// addition
	word m_BCDTableSub[512];			// subtraction

	// Flags bitmask
	const byte FLAG_C = (1 << 0);
	const byte FLAG_Z = (1 << 1);
	const byte FLAG_I = (1 << 2);
	const byte FLAG_D = (1 << 3);
	const byte FLAG_B = (1 << 4);
	const byte FLAG_V = (1 << 6);
	const byte FLAG_N = (1 << 7);
	
	// Registers
	byte mRegA = 0;
	byte mRegX = 0;
	byte mRegY = 0;
	byte mRegS = 0xFF;
	byte mRegFlags = 0x20;	// On an 6502, bit 5 always must be 1
	TReg16bit mRegPC{ 0 };
	
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

	inline bool getN() {
		return ((mRegFlags & FLAG_N) != 0);
	}

	inline bool getV() {
		return ((mRegFlags & FLAG_V) != 0);
	}

	inline bool getD() {
		return ((mRegFlags & FLAG_D) != 0);
	}

	inline bool getI() {
		return ((mRegFlags & FLAG_I) != 0);
	}

	inline bool getZ() {
		return ((mRegFlags & FLAG_Z) != 0);
	}

	inline bool getC() {
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
	inline void accCycles(int cycles) {
		mCumulativeCycles += cycles;
	}
	inline void branch(char operand) {
		mRegPC.value += operand;
		accCycles(1);
	}
};
