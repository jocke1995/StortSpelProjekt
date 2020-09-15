#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <utility>

class MouseMovement;

enum class SCAN_CODES
{
	ESCAPE = 0x101,
	ONE = 0x102,
	TWO = 0x103,
	THREE = 0x104,
	FOUR = 0x105,
	FIVE = 0x106,
	SIX = 0x107,
	SEVEN = 0x108,
	EIGHT = 0x109,
	NINE = 0x10a,
	ZERO = 0x10b,
	PLUS = 0x10c,
	BACKSLASH = 0x10d,
	BACKSPACE = 0x10e,
	TAB = 0x10f,
	Q = 0x110,
	W = 0x111,
	E = 0x112,
	R = 0x113,
	T = 0x114,
	Y = 0x115,
	U = 0x116,
	I = 0x117,
	O = 0x118,
	P = 0x119,
	A_RING = 0x11a,
	DIAERESIS = 0x11b,
	ENTER = 0x11c,
	LEFT_CTRL = 0x11d,
	A = 0x11e,
	S = 0x11f,
	D = 0x120,
	F = 0x121,
	G = 0x122,
	H = 0x123,
	J = 0x124,
	K = 0x125,
	L = 0x126,
	O_DIAERESIS = 0x127,
	A_DIAERESIS = 0x128,
	SECTION_SIGN = 0x129,
	LEFT_SHIFT = 0x12a,
	APOSTROPHE = 0x12b,
	Z = 0x12c,
	X = 0x12d,
	C = 0x12e,
	V = 0x12f,
	B = 0x130,
	N = 0x131,
	M = 0x132,
	COMMA = 0x133,
	PERIOD = 0x134,
	DASH = 0x135,
	RIGHT_SHIFT = 0x136,
	KEYPAD_TIMES = 0x137,
	ALT = 0x138,
	SPACE = 0x139,
	CAPS_LOCK = 0x13a,
	F1 = 0x13b,
	F2 = 0x13c,
	F3 = 0x13d,
	F4 = 0x13e,
	F5 = 0x13f,
	F6 = 0x140,
	F7 = 0x141,
	F8 = 0x142,
	F9 = 0x143,
	F10 = 0x144,
	NUM = 0x145,
	SCROLL = 0x146,
	KEYPAD_7 = 0x147,
	KEYPAD_8 = 0x148,
	KEYPAD_9 = 0x149,
	KEYPAD_MINUS = 0x14a,
	KEYPAD_4 = 0x14b,
	KEYPAD_5 = 0x14c,
	KEYPAD_6 = 0x14d,
	KEYPAD_PLUS = 0x14e,
	KEYPAD_1 = 0x14f,
	KEYPAD_2 = 0x150,
	KEYPAD_3 = 0x151,
	KEYPAD_0 = 0x152,
	KEYPAD_PERIOD = 0x153,

	CHEVRON = 0x156,
	F11 = 0x157,
	F12 = 0x158,

	KEYPAD_ENTER = 0x21c,
	RIGHT_CTRL = 0x21d,
	KEYPAD_DIVISION = 0x235,
	PRINT_SCREEN = 0x237,
	ALT_GR = 0x238,
	HOME = 0x247,
	UP_ARROW = 0x248,
	PAGE_UP = 0x249,
	LEFT_ARROW = 0x24b,
	RIGHT_ARROW = 0x24d,
	END = 0x24f,
	DOWN_ARROW = 0x250,
	PAGE_DOWN = 0x251,
	INSERT = 0x252,
	DEL = 0x253,
	WINDOWS_KEY = 0x25b,
	APPS = 0x25d,
};

enum class MOUSE_BUTTON
{
	LEFT_DOWN = 0x001,
	LEFT_UP = 0x002,
	MIDDLE_DOWN = 0x010,
	MIDDLE_UP = 0x020,
	RIGHT_DOWN = 0x004,
	RIGHT_UP = 0x008,
	WHEEL = 0x400,
};

class Input {
public:
	/// <summary>
	/// Get an instance of the class.
	/// </summary>
	/// <returns>an instance of the class</returns>
	static Input& GetInstance();

	/// <summary>
	/// Register the input devices to be used.
	/// </summary>
	/// <param name="hWnd">: The handle of the input window</param>
	void RegisterDevices(const HWND* hWnd);

	/// <summary>
	/// Sets the state of a keyboard key, either pressed or not pressed.
	/// </summary>
	/// <param name="key">: The scan code of the key</param>
	/// <param name="pressed"> : The state of the key. True means pressed, false means not pressed</param>
	void SetKeyState(SCAN_CODES key, bool pressed);
	/// <summary>
	/// Sets the state of a mouse button, either pressed or not pressed.
	/// </summary>
	/// <param name="button">: The scan code of the button</param>
	/// <param name="pressed">: The state of the button. True means pressed, false means not pressed</param>
	void SetMouseButtonState(MOUSE_BUTTON button, bool pressed);
	/// <summary>
	/// Sets the scroll state of the mouse wheel, either positive (when scrolling forward) or negative (when scrolling backwards).
	/// </summary>
	/// <param name="scroll">: The amount of scroll</param>
	void SetMouseScroll(SHORT scroll);
	/// <summary>
	/// Set the position of the mouse cursor relative to the center of the window.
	/// </summary>
	/// <param name="x">: The x-coordinate of the cursor relative to the center of the window</param>
	/// <param name="y">: The y-coordinate of the cursor relative to the center of the window</param>
	void SetMouseMovement(int x, int y);
	/// <summary>
	/// Get the state of a keyboard key.
	/// </summary>
	/// <param name="key">: The scan code of the key</param>
	/// <returns>true if key is pressed, false if key is not pressed</returns>
	bool GetKeyState(SCAN_CODES key);
	/// <summary>
	/// Get the state of a mouse button.
	/// </summary>
	/// <param name="button">: The scan code of the button</param>
	/// <returns>true if button is pressed, false if button is not pressed</returns>
	bool GetMouseButtonState(MOUSE_BUTTON button);

private:
	Input();
	void unlockMouse(MouseMovement* evnt);

	std::unordered_map<SCAN_CODES, bool> m_KeyState;
	std::unordered_map<MOUSE_BUTTON, bool> m_MouseButtonState;

	bool m_MouseUnlocked = true;
};

#endif // !INPUT_H

