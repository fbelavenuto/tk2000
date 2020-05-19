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

#pragma once

#include "Observer.h"

/*************************************************************************************************/
template <typename T>
class CSubject {
public:
	CSubject() {}
	virtual ~CSubject() {
		mObservers.clear();
	}
	void attach(CObserver<T>* observer) {
		mObservers.emplace_back(observer);
	}
	void detach(CObserver<T>* observer) {
		auto i = std::find(mObservers.begin(), mObservers.end(), observer);
		if (i != mObservers.end()) {
			mObservers.erase(i);
		}
	}
protected:
	void notify(T* val) {
		for (auto i : mObservers) {
			i->notify(val);
		}
	}
private:
	std::vector<CObserver<T>*> mObservers;
};