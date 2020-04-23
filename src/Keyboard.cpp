

#include "Keyboard.h"

/*************************************************************************************************/
CKeyboard::CKeyboard(CBus *bus) {
	bus->addDevice(0xC000, 0xC01F, this);
	bus->addDevice(0xC05E, 0xC05F, this);
}

/*************************************************************************************************/
byte CKeyboard::read(word addr) {
	if (mKbOutCtrl && mCtrl) {
		return 1;
	} else if (mKbOut == 1 && mShift) {
		return 1;
	}

	if (mKbOut == (byte)(1 << mRow)) {
		return (byte)(1 << mCol);
	}
	return 0;
}

/*************************************************************************************************/
void CKeyboard::write(word addr, byte data) {
	if (addr == 0xC05E) {
		mKbOutCtrl = false;
		return;
	} else if (addr == 0xC05F) {
		mKbOutCtrl = true;
		return;
	}
	mKbOut = data;
}

/*************************************************************************************************/
void CKeyboard::update(unsigned long cycles) {

}

/*************************************************************************************************/
void CKeyboard::reset() {
	mCol = 10;
	mRow = 10;
	mCtrl = false;
	mShift = false;
}

/*************************************************************************************************/
void CKeyboard::processEvent(SDL_KeyboardEvent e) {

	if (e.repeat) {
		return;
	}
	if (e.state == SDL_RELEASED) {
		mRow = 10;
		mCol = 10;
		mCtrl = false;
		mShift = false;
		return;
	}
	switch (e.keysym.sym){
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		mCtrl = true;

	case SDLK_a:
		mRow = 1;
		mCol = 5;
		return;

	case SDLK_b:
		mRow = 0;
		mCol = 1;
		return;

	case SDLK_c:
		mRow = 0;
		mCol = 3;
		return;

	case SDLK_d:
		mRow = 1;
		mCol = 3;
		return;

	case SDLK_e:
		mRow = 2;
		mCol = 3;
		return;

	case SDLK_f:
		mRow = 1;
		mCol = 2;
		return;

	case SDLK_g:
		mRow = 1;
		mCol = 1;
		return;

	case SDLK_h:
		mRow = 6;
		mCol = 1;
		return;

	case SDLK_i:
		mRow = 5;
		mCol = 3;
		return;

	case SDLK_j:
		mRow = 6;
		mCol = 2;
		return;

	case SDLK_k:
		mRow = 6;
		mCol = 3;
		return;

	case SDLK_l:
		mRow = 6;
		mCol = 4;
		return;

	case SDLK_m:
		mRow = 7;
		mCol = 2;
		return;

	case SDLK_n:
		mRow = 7;
		mCol = 1;
		return;

	case SDLK_o:
		mRow = 5;
		mCol = 4;
		return;

	case SDLK_p:
		mRow = 5;
		mCol = 5;
		return;

	case SDLK_q:
		mRow = 2;
		mCol = 5;
		return;

	case SDLK_r:
		mRow = 2;
		mCol = 2;
		return;

	case SDLK_s:
		mRow = 1;
		mCol = 4;
		return;

	case SDLK_t:
		mRow = 2;
		mCol = 1;
		return;

	case SDLK_u:
		mRow = 5;
		mCol = 2;
		return;

	case SDLK_v:
		mRow = 0;
		mCol = 2;
		return;

	case SDLK_w:
		mRow = 2;
		mCol = 4;
		return;

	case SDLK_x:
		mRow = 0;
		mCol = 4;
		return;

	case SDLK_y:
		mRow = 1;
		mCol = 3;
		return;

	case SDLK_z:
		mRow = 5;
		mCol = 1;
		return;

	case SDLK_1:
		mRow = 3;
		mCol = 5;
		return;

	case SDLK_2:
		mRow = 3;
		mCol = 4;
		return;

	case SDLK_3:
		mRow = 3;
		mCol = 3;
		return;

	case SDLK_4:
		mRow = 3;
		mCol = 2;
		return;

	case SDLK_5:
		mRow = 3;
		mCol = 1;
		return;

	case SDLK_6:
		mRow = 4;
		mCol = 1;
		return;

	case SDLK_7:
		mRow = 4;
		mCol = 2;
		return;

	case SDLK_8:
		mRow = 4;
		mCol = 3;
		return;

	case SDLK_9:
		mRow = 4;
		mCol = 4;
		return;

	case SDLK_0:
		mRow = 4;
		mCol = 5;
		return;

	case SDLK_COMMA:
		mRow = 7;
		mCol = 3;
		return;

	case SDLK_PERIOD:
		mRow = 7;
		mCol = 4;
		return;

	case SDLK_COLON:
		mRow = 6;
		mCol = 5;
		return;

	case SDLK_QUESTION:
		mRow = 7;
		mCol = 5;
		return;

	case SDLK_EXCLAIM:
		mRow = 3;
		mCol = 5;
		mShift = true;
		return;

	case SDLK_QUOTEDBL:
		mRow = 3;
		mCol = 4;
		mShift = true;
		return;

	case SDLK_HASH:
		mRow = 3;
		mCol = 3;
		mShift = true;
		return;

	case SDLK_DOLLAR:
		mShift = true;
		mRow = 3;
		mCol = 2;
		return;

	case SDLK_PERCENT:
		mRow = 3;
		mCol = 1;
		mShift = true;
		return;

	case SDLK_AMPERSAND:
		mShift = true;
		mRow = 4;
		mCol = 1;
		return;

	case SDLK_SLASH:
		mRow = 4;
		mCol = 2;
		mShift = true;
		return;

	case SDLK_LEFTPAREN:
		mShift = true;
		mRow = 4;
		mCol = 3;
		return;

	case SDLK_RIGHTPAREN:
		mShift = true;
		mRow = 4;
		mCol = 4;
		return;

	case SDLK_EQUALS:
		mShift = true;
		mRow = 5;
		mCol = 4;
		return;

	case SDLK_MINUS:
		mShift = true;
		mRow = 5;
		mCol = 3;
		return;

	case SDLK_PLUS:
		mShift = true;
		mRow = 5;
		mCol = 5;
		return;

	case SDLK_ASTERISK:
		mShift = true;
		mRow = 4;
		mCol = 5;
		return;

	case SDLK_CARET:
		mShift = true;
		mRow = 6;
		mCol = 3;
		return;

	case SDLK_AT:
		mShift = true;
		mRow = 6;
		mCol = 4;
		return;

	case SDLK_UP:
		mRow = 6;
		mCol = 0;
		return;

	case SDLK_DOWN:
		mRow = 5;
		mCol = 0;
		return;

	case SDLK_LEFT:
		mRow = 3;
		mCol = 0;
		return;

	case SDLK_RIGHT:
		mRow = 4;
		mCol = 0;
		return;

	case SDLK_RETURN:
		mRow = 7;
		mCol = 0;
		return;

	case SDLK_BACKSPACE:
		mRow = 3;
		mCol = 0;
		return;

	case SDLK_SPACE:
		mRow = 2;
		mCol = 0;
		return;
	}
}

/*
 *   Keyboard Matrix
 *   ===============
 *
 *                         KBIN
 *             0   1   2   3   4   5   6   7
 *         0       B   V   C   X   Z
 *     K   1       G   F   D   S   A
 *     B   2  " "  T   R   E   W   Q
 *     O   3  LFT  5   4   3   2   1
 *     U   4  RGT  6   7   8   9   0
 *     T   5  DWN  Y   U   I   O   P
 *         6  UP   H   J   K   L   :
 *         7  RTN  N   M   ,   .   ?
*/