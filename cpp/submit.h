#include <iostream>
#include <string>

/******************************************/
extern std::string ai_name;
/******************************************/

int ai_side; // 0 -> up and 1 -> down

void SubmitInit() {
	std::cin >> ai_side;
	std::cout << ai_name << "\nEND\n";
	std::cout.flush();
}

std::pair<int, int> Get() {
	int a, b;
	std::cin >> a >> b;
	return std::make_pair(a, b);
}

void Wait() {
	std::string temp;
	std::cin >> temp;
}


void Post(std::pair<int, int> loc) {
	std::cout << loc.first << ' ' << loc.second << "\nEND\n";
	std::cout.flush();
}

void Debug(std::string message) {
	std::cerr << message;
	std::cerr.flush();
}

void init();
void GetUpdate(std::pair<int, int>);
std::pair<int, int> Action();

int main() {
	SubmitInit();
	init();
	if (ai_side == 0) {
		while (true) {
			std::pair<int, int> temp;
			Wait();
			Post(Action());
			temp = Get();
			GetUpdate(temp);
		}
	}
	else {
		while (true) {
			std::pair<int, int> temp;
			temp = Get();
			GetUpdate(temp);
			Wait();
			Post(Action());
		}
	}
	return 0;
}
