#include <iostream>
#include <cstring>

using namespace std;
int main()
{
	const char* human_choices[] = {"paper", "quit", "rock", "scissors"};
	const char* ai_choices[] = {"scissors", "quit", "paper", "rock"};
	const int dist_to_p = 112;
	do {
		char human_choice = '\0';
		int8_t index= 0;
		do {
			cout << "Please choose: rock (r) - paper (p) - scissors (s) or (q)uit for exit" << endl;
			cin >> human_choice;
			human_choice = tolower(human_choice);
			if (human_choice == 'q') {
				cout << "By, see you later." << endl;
				return 0;
			}
		} while(human_choice !='r' && human_choice !='p' && human_choice !='s' && human_choice!='q');
		index = human_choice - dist_to_p;
		cout << "You choose " << human_choices[index] << ", I choose " << ai_choices[index] << endl;
		cout << "I win: " << ai_choices[index] << " beats " << human_choices[index] << endl;
	} while(true);
	return 0;
}
