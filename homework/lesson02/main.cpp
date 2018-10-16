#include <iostream>
#include <stdio.h>

enum eItem
{
	ROCK, PAPER, SCISSORS, MAX
};

enum eResult
{
	WIN, LOSE, PAR
};

static eItem ask()
{
	while (true) {
		std::cout
			<< "Please choose: rock (r) - paper (p) - scissors (s)"
				" - quit (q)" << std::endl;
		const char c = getchar();
		switch (c) {
		case 'r':
			return eItem::ROCK;
		case 'p':
			return eItem::PAPER;
		case 's':
			return eItem::SCISSORS;
		case 'q':
			exit(0);
		default:
			std::cout << "Wrong value!" << std::endl;
		}
	}
}

static eItem generate()
{
	return eItem(rand() % eItem::MAX);
}

static char const * const eItemToStr(eItem item)
{
	static char const * const items_str[] = { "rock", "paper", "scissors" };
	return items_str[item];
}

static eResult getResult(const eItem userAnswer, const eItem computerAnswer)
{
	static eResult matrix[eItem::MAX][eItem::MAX] = {
		/*ROCK	   PAPER	  SCISSORS*/
		{ eResult::PAR, eResult::LOSE, eResult::WIN },/*ROCK*/
		{ eResult::WIN, eResult::PAR, eResult::LOSE },/*PAPER*/
		{ eResult::LOSE, eResult::WIN, eResult::PAR } };/*SCISSORS*/
	return matrix[userAnswer][computerAnswer];
}

static void printResult(const eResult result)
{
	static char const * const matrix[] = { "Win", "Lose", "Parity" };
	std::cout << matrix[result] << std::endl;
}

int main()
{
	eItem userAnswer = ask();
	eItem computerAnswer = generate();

	std::cout << "Your choice: " << eItemToStr(userAnswer) << std::endl;
	std::cout << "Computer choice: " << eItemToStr(computerAnswer)
			<< std::endl;

	eResult result = getResult(userAnswer, computerAnswer);
	printResult(result);

	return 0;
}
