#include <stdio.h>
#include <time.h>

#if (__WIN32__ || _WIN32 || _WIN64 || __WINDOWS__)
#   define USING_WINDOWS
#endif

#if defined USING_WINDOWS
#	include <windows.h>
#else // Assume POSIX
#	include <unistd.h>
#endif
#include <string>
#include <sstream>
#include <stdlib.h>

class Hand {
  int dice[3], yaku;
  std::string name, d_str;
  void Set_Yaku();
  void Set_Name(int val);
  
  bool no_hand();
  bool hifumi();  // 1-2-3
  bool shigoro(); // 4-5-6
  int deme();     // Single
  int arashi();   // Triple
  
public:
  Hand();
  Hand(unsigned int d1, unsigned int d2, unsigned int d3);
  void operator=(const Hand& h);
  
  void Roll();
  int Yaku_Value();
  const std::string &Yaku_Name(), &Dice();
};

Hand::Hand() {
  Roll();
}

Hand::Hand(unsigned int d1, unsigned int d2, unsigned int d3) {
  dice[0] = d1;
  dice[1] = d2;
  dice[2] = d3;
  Set_Yaku();
}

void Hand::operator=(const Hand& h) {
  dice[0] = h.dice[0];
  dice[1] = h.dice[1];
  dice[2] = h.dice[2];
}

void Hand::Roll() {
  for (int i = 0; i < 3; i++)
    dice[i] = (rand() % 6) + 1;
  Set_Yaku();
}

void Hand::Set_Yaku() {
  int trip = arashi();
  int single = deme();
  
  if (no_hand())
    yaku = 0;
  else if (hifumi())
    yaku = 13;
  else if (shigoro())
    yaku = 14;
  else if (trip != 0)
    yaku = trip;
  else if (single != 0)
    yaku = single;
  else
    yaku = 0;
  Set_Name(yaku);
}

bool Hand::no_hand() {
  if (dice[0] != dice[1] && dice[0] != dice[2] && dice[1] != dice[2])
    return true;
  return false;
}

bool Hand::hifumi() {
  if (dice[0] == 1) {
    if (dice[1] == 2) {
      if (dice[2] == 3) return true;
    } else if (dice[2] == 2) {
      if (dice[1] == 3) return true;
    }
  } else if (dice[1] == 1) {
    if (dice[0] == 2) {
      if (dice[2] == 3) return true;
    } else if (dice[2] == 2) {
      if (dice[0] == 3) return true;
    }
  } else if (dice[2] == 1) {
    if (dice[1] == 2){
      if (dice[0] == 3) return true;
    } else if (dice[0] == 2) {
      if (dice[1] == 3) return true;
    }
  } return false;
}

bool Hand::shigoro() {
  if (dice[0] == 4) {
    if (dice[1] == 5) {
      if (dice[2] == 6) return true;
    } else if (dice[2] == 5) {
      if (dice[1] == 6) return true;
    }
  } else if (dice[1] == 4) {
    if (dice[0] == 5){
      if (dice[2] == 6) return true;
    } else if (dice[2] == 5) {
      if (dice[0] == 6) return true;
    }
  } else if (dice[2] == 4) {
    if (dice[1] == 5) {
      if (dice[0] == 6) return true;
    } else if (dice[0] == 5) {
      if (dice[1] == 6) return true;
    }
  } return false;
}

int Hand::deme() {
  if (dice[0] == dice[1])
    return dice[2];
  else if (dice[0] == dice[2])
    return dice[1];
  else if (dice[1] == dice[2])
    return dice[0];
  return 0;
}

int Hand::arashi() {
  if (dice[0] == dice[1] && dice[0] == dice[2])
    return dice[0] + 6;
  return 0;
}

void Hand::Set_Name(int val) {
  std::stringstream ss;
  switch (val) {
    default:
    case 0:
      name = "目なし"; //No-hand
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      ss << val;
      name = ss.str() + "の目"; //Single
      break;
    case 7:
      name = "ピンゾロ"; //1-1-1
      break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      val -= 6; ss << val;
      name = ss.str() + "のアラシ"; //Triple
      break;
    case 13:
      name = "ヒフミ"; //1-2-3
      break;
    case 14:
      name = "シゴロ"; //4-5-6
      break;
  }
}

int Hand::Yaku_Value() {
  return yaku;
}

const std::string& Hand::Yaku_Name() {
  return name;
}

const std::string& Hand::Dice() {
  std::stringstream s1, s2, s3;
  s1 << dice[0]; s2 << dice[1]; s3 << dice[2];
  d_str = s1.str() + "-" + s2.str() + "-" + s3.str();
  return d_str;
}

#define MIN_BET     100
#define MAX_PLAYERS 10

void write_title();
void setup_game();
void pray_gaim(unsigned int player_c, unsigned int money, const std::string& name);
int  player_beat_dealer(int dealer, int player);
int* place_bets(int players[], int total_players, int dealer, int start_total);
int  random_bet(int max);
int  round_up(int num, int multiple = 100);
int  check_game_over(int players[], int total_players);
void Pause(unsigned int seconds = 1);

int main(int argc, const char* argv[]) {
	srand(static_cast<int>(time(0)));
	setup_game();
	return 0;
}

void write_title() {
	printf("\n*************************************************************\n");
	printf(         "* 地下チンチ(ン)ロリ(コ)ン: Underground Chinchi(n)rori(ko)n *\n");
	printf("*************************************************************\n\n");
}

void setup_game() {
	write_title();
	unsigned int player_count, total_money;
	char input[256], name_arr[12];
	std::string name;

	printf("How many players would you like?\n");
	while (true) {
		printf("Player count: ");
		scanf("%s", input);

		std::stringstream ss(input);
		if (ss >> player_count) {
			if (player_count >= 2 && player_count <= 10)
				break;
		}
		printf("Invalid player count, please try again!\nTotal players must be between 2 and 10\n");
	}

	printf("\nHow much money for each players?\n");
	while (true) {
		printf("Total money: ");
		scanf("%s", input);

		std::stringstream ss(input);
		if (ss >> total_money) {
			if (total_money >= 10000 && total_money <= 100000)
				break;
		}
		printf("Invalid ammount, please try again!\nTotal must be between ¥10,000 and ¥100,000\n");
	}
	if (name == "") {
		while (true) {
			printf("\nYour player name: ");
			scanf("%s", name_arr);
			name = name_arr;

			if (name.length() <= 12) {
				if (name == "" || name.length() <= 0)
					name = "Unnamed";
				break;
			}
			printf("Invalid name, please try again!\nName must be shorter than 12 characters\n");
		}
	}

	pray_gaim(player_count, total_money, name);
	char pray_agen;
	while (true) {
		printf("\nYou wan pray agen? [Y/N]: ");
		pray_agen = getchar();

		if (pray_agen == 'N' || pray_agen == 'n')
			break;
		else if (pray_agen == 'Y' || pray_agen=='y')
			setup_game();
	}
}

void pray_gaim(unsigned int player_c, unsigned int money, const std::string& name) {
	int dealer = (rand() % player_c);
	printf("\nWelcome, %s\n", name.c_str());
	if (dealer == 0)
		printf("You are the dealer! ");
	else
		printf("Player %i is the dealer! ", dealer);

	int players[MAX_PLAYERS], round_num = 1;
	for (int i = 0; i < (signed)player_c; i++)
		players[i] = money;
	bool end_game = false, dealer_payout = false, dealer_hifumi = false;
	printf("ゲームスタート！！！\n");

	while (!end_game) {
		printf("\nCurrent round: ラウンド%i！\n\n", round_num);
		int *player_bets = place_bets(players, player_c, dealer, money);
		int tries = 0;
		Hand dealer_hand;

		if (dealer == 0) {
			printf("You are the dealer!\n");
			Pause(2);
		}
		else
			printf("Player %i is the dealer!\n", dealer);

		while (true) {
			dealer_hand.Roll();
			printf("Dealer");
			if (dealer == 0)
				printf(" (you)");
			printf(" rolled %s (%s)", dealer_hand.Yaku_Name().c_str(), dealer_hand.Dice().c_str());

			if (dealer_hand.Yaku_Value() == 0) {
				printf(" No-hand, %i roll(s) left.\n", (3 - (tries + 1)));
				Pause(1);
				tries++;
				if (tries == 3) {
					if (dealer == 0)
						printf("You");
					else
						printf("The dealer");

					printf(" rolled 3 no-hands! Dealer must pay out!\n");
					dealer_payout = true;
					break;
				}
			} else {
				printf("\n");
				Pause(1);
				break;
			}
		}

		if (dealer_hand.Yaku_Value() == 13)  {
			printf("\nHifumi! double payout! Roll a successful hand to win x2!\n");
			dealer_hifumi = true;
		}

		int c = dealer + 1;
		if (c >= (signed)player_c) c = 0;
		if (!dealer_payout) {
			for (int j = 0; j < ((signed)player_c - 1); j++) {
				if (players[c] > 0) {
					tries = 0;
					Hand cur_player_roll;
					bool player_hifumi = false, player_payout = false;

					while (true) {
						cur_player_roll.Roll();
						if (c == 0)
							printf("You");
						else
							printf("Player %i", c);

						printf(" rolled %s (%s)", cur_player_roll.Yaku_Name().c_str(), cur_player_roll.Dice().c_str());

						if (cur_player_roll.Yaku_Value() == 0) {
							printf(" No-hand %i roll(s) left.\n", (3 - (tries + 1)));
							Pause(1);

							tries++;
							if (tries == 3) {
								if (c == 0)
									printf("You");
								else
									printf("Player %i", c);

								printf(" rolled 3 no-hands! Dealer wins ¥%i\n", player_bets[c]);
								player_payout = true;
								break;
							}
						}
						else {
							printf("\n");
							Pause(1);
							break;
						}
					}
					if (cur_player_roll.Yaku_Value() == 13)
						player_hifumi = true;

					int k = player_beat_dealer(dealer_hand.Yaku_Value(), cur_player_roll.Yaku_Value());
					int add_score = 0;

					if (player_payout) {
						if (dealer_hand.Yaku_Value() == 7)
							add_score += (player_bets[c] * 5);
						else if (dealer_hand.Yaku_Value() >= 8 && dealer_hand.Yaku_Value() <= 12)
							add_score += (player_bets[c] * 5);
						else if (dealer_hand.Yaku_Value() == 14)
							add_score += (player_bets[c] * 2);
						else
							add_score += player_bets[c];

						players[dealer] += add_score;
						players[c] -= add_score;
						player_payout = false;
					}
					else {
PAYOUT:
						switch (k) {
							default:
							case 0: // Dealer
								if (dealer_hand.Yaku_Value() == 7 )
									add_score += (player_bets[c] * 5);
								else if (dealer_hand.Yaku_Value() >= 8 && dealer_hand.Yaku_Value() <= 12)
									add_score += (player_bets[c] * 3);
								else if (dealer_hand.Yaku_Value() == 14)
									add_score += (player_bets[c] * 2);
								else
									add_score += player_bets[c];

								if (player_hifumi == true)
									add_score *= 2;
								players[dealer] += add_score;
								players[c] -= add_score;

								if (dealer == 0)
									printf("You have won ¥%i!\n", add_score);
								else
									printf("The dealer has won ¥%i!\n", add_score);
								break;
							case 1: // Player
								if (cur_player_roll.Yaku_Value() == 7)
									add_score += (player_bets[c] * 5);
								else if (cur_player_roll.Yaku_Value() >= 8 && cur_player_roll.Yaku_Value() <= 12)
									add_score += (player_bets[c] * 3);
								else if (cur_player_roll.Yaku_Value() == 14)
									add_score += (player_bets[c] * 2);
								else
									add_score += player_bets[c];

								if (dealer_hifumi == true)
									add_score *= 2;
								players[c] += add_score;
								players[dealer] -= add_score;

								if (c == 0)
									printf("You have won ¥%i!\n", add_score);
								else
									printf("Player %i has won ¥%i!\n", c, add_score);
								break;
							case 2: // Draw
								printf("It's a push! ");
								if (c == 0 && dealer != 0)
									printf(" You must roll again!\n");
								else
									printf("Player %i must roll again!\n", c);
								Pause(1);

								Hand player_push_roll;
								if (player_push_roll.Yaku_Value() == 13)
									player_hifumi = true;
								int k2 = player_beat_dealer(dealer_hand.Yaku_Value(), player_push_roll.Yaku_Value());
								Pause(1);

								if (c == 0)
									printf("You");
								else
									printf("Player %i\n", c);
								printf(" rolled %s (%s)\n", player_push_roll.Yaku_Name().c_str(), player_push_roll.Dice().c_str());

								if (k2 == 1) {
									if (c == 0 && dealer != 0)
										printf("You have won the push! ");
									else
										printf("Player %i has won the push!\n", c);
									k = 1;

									cur_player_roll = player_push_roll;
									goto PAYOUT;
								}
								else {
									if (dealer == 0)
										printf("You have won the push! Player %i may pay out!\n", c);
									else
										printf("Dealer has won the push! Player %i must pay out!\n", c);

									k = 0;
									cur_player_roll = player_push_roll;
									goto PAYOUT;
								}
								break;
						}
					}
				}
				c++;
				if (c >= (signed)player_c) c = 0;
			}
		} else {
			for (int a = 0; a < (signed)player_c; a++) {
				if (a != dealer) {
					int dealer_total = players[dealer];
					if (dealer_total > player_bets[a]) {
						players[dealer] -= player_bets[a];
						players[a] += player_bets[a];
					} else {
						printf("Dealer doesn't have enough money to continue...\n");
						players[dealer] = 0;
						break;
					}
				}
			}
			dealer_payout = false;
		}

		printf("\nPlayer's totals (ラウンド%i)\n", round_num);
		for (int a = 0; a < (signed)player_c; a++) {
			if (a == 0)
				printf("You have ¥");
			else
				printf("Player %i has ¥", a);
			printf("%i", players[a]);
			if (a == dealer)
				printf(" (Dealer)");
			printf("\n");
		}

		int game_over = check_game_over(players, player_c);
		switch (game_over) {
			default:
			case 0: // Game isn't over
				if (dealer == ((signed)player_c - 1))
					dealer = 0;
				else
					dealer++;

				round_num += 1;
				break;
			case 1: // Player lost
				printf("\nYou are out of money! You lose!");
				end_game = true;
				break;
			case 2: // Player won
				printf("\nContratulations, %s ! You won ¥%i!", name.c_str(), players[0]);
				end_game = true;
				break;
		}
	}
}

int player_beat_dealer(int dealer, int player) {
	if (player == 7) {
		if (dealer == 7)
			return 2;
		else return 1;
		return 1;
	} else if (player == 13) {
		return 0;
	} else if (dealer == 13) {
		return 1;
	} else if (player == 14) {
		if (dealer == 7)
			return 0;
		else if (dealer == 14)
			return 2;
		else
			return 1;
	} else {
		if (dealer == player)
			return 2;
		else if (player > dealer)
			return 1;
		else
			return 0;
	}
	return 0;
}

int* place_bets(int players[], int total_players, int dealer, int start_total) {
	int *bets = new int[total_players];
	int dealer_max = players[dealer];
	int max_bet_range = (25 * start_total) / 100;

	if (dealer_max > (start_total + (start_total / 2)))
		max_bet_range = (35 * dealer_max) / 100;
	else if (dealer_max < (start_total / 2))
		max_bet_range = (50 * dealer_max) / 100;
	else if (dealer_max < (start_total / 10))
		max_bet_range = dealer_max;

	if (dealer != 0) {
		int player_max = players[0];
		if (player_max <= MIN_BET) {
			printf("%s%i%s\n", "Minimum bet is ", MIN_BET, ", you don't have enough money!");
			printf("%s%i\n", "You must go all in! Placing bet of ", player_max);
			bets[0] = players[0];
		}
		else {
			char input[80];
			int player_bet;
			while (true) {
				printf("%s", "Place your bet: ");
				scanf("%s", input);

				std::stringstream ss(input);
				if (ss >> player_bet) {
					if (player_max <= max_bet_range) {
						if (player_bet >= MIN_BET && player_bet <= player_max) {
							bets[0] = player_bet;
							break;
						} else
							printf("%s\n%s%i%s%i\n", "Invalid ammount, please try again!", "Bet must be between ¥", MIN_BET, " and ¥", player_max);
					} else {
						if (player_bet >= MIN_BET && player_bet <= max_bet_range) {
							bets[0] = player_bet;
							break;
						} else
							printf("%s\n%s%i%s%i\n", "Invalid ammount, please try again!", "Bet must be between ¥", MIN_BET, " and ¥", max_bet_range);
					}
				}
			}
		}
	}

	for (int i = 1; i < total_players; i++) {
		if (i != dealer) {
			int temp_pm = players[i];
			if (!temp_pm <= 0) {
				if (temp_pm > max_bet_range) temp_pm = max_bet_range;
				int temp_bet = random_bet(temp_pm);

				bets[i] = temp_bet;
				if (temp_bet >= players[i])
					printf("%s%i%s\n", "Player ", i, " goes all in!");
				else
					printf("%s%i%s%i\n", "Player ", i, " bets ", temp_bet);
			}
		}
	}
	printf("\n");
	return bets;
}

int random_bet(int max) {
	int i = (rand() % 200) + 1;
	int max_two, ret_bet, ret_val;

	if (i <= 100) {
		max_two = (10 * max) / 100;
		ret_bet = (rand() % ((max_two + 1) - MIN_BET)) + MIN_BET;
		ret_val = round_up(ret_bet, 100);
	} else if (i >= 101 && i <= 170) {
		int max_three = (40 * max) / 100;
		max_two = (10 * max) / 100;
		ret_bet = (rand() % ((max_three + 1) - max_two)) + max_two;
		ret_val = round_up(ret_bet, 100);
	} else if (i >= 171 && i <= 190) {
		int max_three = (80 * max) / 100;
		max_two = (40 * max) / 100;
		ret_bet = (rand() % ((max_three + 1) - max_two)) + max_two;
		ret_val = round_up(ret_bet, 100);
	} else {
		max_two = (80 * max) / 100;
		ret_bet = (rand() % ((max + 1) - max_two)) + max_two;
		ret_val = round_up(ret_bet, 100);
	}
	if (ret_val > max) ret_val = max;
	return ret_val;
}

int round_up(int num, int multiple) {
	if(multiple == 0) return num; 
	int r = num % multiple;
	if (r == 0) return num;
	return num + multiple - r;
}

int check_game_over(int players[], int total_players) {
	if (players[0] <= 0) {
		return 1; // Player lost
	} else {
		int players_left = 0;
		for (int i = 0; i < total_players; i++) {
			if (!players[i] <= 0)
				players_left++;
		}

		if (players_left == 1) {
			if (players[0] > 0)
				return 2;
			else
				return 1;
		} else if (players_left > 1)
			return 0;
		else
			return 1;
	}
}

void Pause(unsigned int seconds) {
#if defined USING_WINDOWS
	Sleep(seconds * 1000);
#else // Assume POSIX
	sleep(seconds);
#endif
}
