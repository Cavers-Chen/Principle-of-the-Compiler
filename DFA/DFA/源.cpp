#include <iostream>
bool isDigit(char);
bool isDot(char);
bool isE_e(char);
bool isSign(char);
bool NFA(char*, int);
int main() {
	char buffer[999];
	while (std::cin >> buffer) {
		int len = strnlen_s(buffer, 999);
		if (NFA(buffer, len)) std::cout << "True\n";
		else std::cout << "False\n";
	}
	return 0;
}
bool isDigit(char ch) {
	if (ch <= '9'&&ch >= '0') return true;
	return false;
}
bool isDot(char ch) {
	if (ch == '.') return true;
	return false;
}
bool isE_e(char ch) {
	if (ch == 'e' || ch == 'E') return true;
	return false;
}
bool isSign(char ch) {
	if (ch == '+' || ch == '-') return true;
	return false;
}
bool NFA(char* str,int len) {
	int state = 0, i = 0;
	while (true&&len>i) {
		switch (state)
		{
		case 0:
			if (isDigit(str[i])) { state = 1; break; }
			return false;
		case 1:
			if (isDigit(str[i])) break;
			if (isDot(str[i])) { state = 2; break; }
			if (isE_e(str[i])) { state = 4; break; }
			return false;
		case 2:
			if (isDigit(str[i])) { state = 3; break; }
			return false;
		case 3:
			if (isDigit(str[i])) { state = 3; break; }
			if (isE_e(str[i])) { state = 4; break; }
			return false;
		case 4:
			if (isDigit(str[i])) { state = 6; break; }
			if (isSign(str[i])) { state = 5; break; }
			return false;
		case 5:
			if (isDigit(str[i])) { state = 6; break; }
			return false;
		case 6:
			if (isDigit(str[i])) break;
			return false;
		default:
			break;
		}
		i++;
	}
	return true;
}