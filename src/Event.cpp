#include "Event.hpp"

struct Event_Struct Event_Struct;

void Event_Struct::AddKey(const SDL_KeyboardEvent & event) {
	if (event.repeat)
		return;

	for (auto K = this->key_events.begin(); K < this->key_events.end(); ++K) {
		if (K->key == event.keysym.sym) {
			if (event.type == SDL_KEYUP) {
				K->type = KEY_UP;
				return;
			}
			return;
		}
	}

	this->key_events.push_back(Keyboard_Event{ event.keysym.sym , KEY_DOWN});
}

KEY_TYPE Event_Struct::GetKeyState(SDL_Keycode key) {
	for (auto K : this->key_events) {
		if (K.key == key)
			return K.type;
	}

	return KEY_NULL;
}
