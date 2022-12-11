#include <iostream>
#include "FA.h"
using namespace std;

int global_state = 0;

int main() {
	NFA_TRAN translator;
	char str[] = "ab*c(a|b)*";
	size_t pos = 0;
	translator.LoadExpress(str, pos, strnlen_s(str, 999));
	translator.PrintStates();
	return 0;
}