#include <iostream>
#include <stdlib.h>
using namespace std;

int main()
{
const char strLine[] = "rps";
int max = sizeof(strLine)-1;
char userSmbl;

srand(time(0));
int rndNum = random()%max+0;
char rtnSmbl = strLine[rndNum];
//cout << "rtnSmbl= " << rtnSmbl << "\n"; //Debug line

<<<<<<< cbc22f74b804e7d4af334a306b8c9b48e568c8aa
{
while (userSmbl !='r' && userSmbl !='p' && userSmbl !='s')
=======
while (userSmbl !='r' && userSmbl !='p' && userSmbl !='s')
{
>>>>>>> 02-Git: Finish hometask
	cout << "Please choose: rock(r) - paper(p) - scissors (s) \n";
	try {
		cin >> userSmbl;
	}
	catch (int e) {
<<<<<<< cbc22f74b804e7d4af334a306b8c9b48e568c8aa
	}
		cout << "incorrect input. Error: " << e << "\n";
=======
		cout << "incorrect input. Error: " << e << "\n";
	}
>>>>>>> 02-Git: Finish hometask
}

char concatSmbl[2];
concatSmbl[0]= rtnSmbl;
concatSmbl[1] = userSmbl;

char winLine[] = "rr-pp-ss-ps-sr-rp-sp-rs-sp"; //0..7 - Friends won;9..16 - Player won; 18..25 - boot won;
int position;
int winLineSize = sizeof(winLine);
//cout << "winLineSize= " << winLineSize << "\n";  //Debug line
<<<<<<< cbc22f74b804e7d4af334a306b8c9b48e568c8aa
for (int i =0; i < winLineSize; i++) {

=======

for (int i =0; i < winLineSize; i++) {
>>>>>>> 02-Git: Finish hometask
	if (rtnSmbl == winLine[i] && userSmbl == winLine[i+1]) {
		position = i;
		break;
	}
}

//cout << "position= " << position << "\n";  //Debug line
cout << "I choose : " << rtnSmbl << ", You choose: " << userSmbl << "\n=======\n";
<<<<<<< cbc22f74b804e7d4af334a306b8c9b48e568c8aa
if (position < 8) {

	cout << "Friends win :|\n";
}
if (position > 8 && position < 17) {

=======

if (position < 8) {
	cout << "Friends win :|\n";
}

if (position > 8 && position < 17) {
>>>>>>> 02-Git: Finish hometask
	cout << "You win :) \n";
}
if (position > 17) {
	cout << "I win :( \n";
}

return 0;
}

