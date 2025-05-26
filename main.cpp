#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;
enum Color {
    NONE, RED, BLUE, GREEN, YELLOW
};
enum CardType {
    NUMBER, SKIP, REVERSE, DRAW_TWO, WILD, WILD_DRAW_FOUR
};
string colortostring(Color color){
    switch(color) {
        case RED: return "Red ";
        case BLUE: return "Blue ";
        case GREEN: return "Green ";
        case YELLOW: return "Yellow ";
        case NONE: return "Wild ";
    }}
class Player;//line 44 reference to player
class Card{
public:
    Color color;
    CardType cardType;
    Card( Color color , CardType cardType){
        this->color = color;
        this->cardType = cardType;
    }
    virtual void print() {
        cout << "Card: "<<colortostring(color)<<" ";

        switch(cardType) {
            case NUMBER: cout << "Number"; break;
            case SKIP: cout << "Skip"; break;
            case REVERSE: cout << "Reverse"; break;
            case DRAW_TWO: cout << "Draw Two"; break;
            case WILD: cout << "Wild"; break;
            case WILD_DRAW_FOUR: cout << "Wild Draw Four"; break;
        }
    }

    virtual bool playCard(Card* topCard) = 0;
    virtual void applyEffect(Player*& currentPlayer, Player*& nextPlayer, int& direction, int& stack) {}
    virtual ~Card() {}
    friend class Player;

};
class NumberCard: public Card{
public:
    int num;
    NumberCard(Color color,CardType CardTypes, int num ):Card(color , CardTypes){
        this->num = num;
    }
    bool playCard(Card* topCard) {
        if(color == topCard->color){
            return true;
        }
        if(cardType == topCard->cardType){NumberCard* t = (NumberCard*) topCard;
            if(t->num == num) return true;}
        return false;
    }
    void print() override{
        Card::print();
        cout << " " << num;
    }

};
class WildCard:public Card{
public:
    WildCard(CardType t) : Card(NONE, t) {};
    bool playCard(Card* topCard) override {
        if (topCard->cardType == DRAW_TWO || topCard->cardType == WILD_DRAW_FOUR) {
            if (cardType == WILD_DRAW_FOUR && topCard->cardType == WILD_DRAW_FOUR) {
                return true;
            }
            return false;
        }
        return true;
    }
    void chooseColor() {
        int c = 0;
        while (true) {
            cout << "choose color:(red=1,blue=2,green=3,yellow=4)" << endl;
            cin >> c;
            if (c == 1 || c == 2 || c == 3 || c == 4) {
                color = (Color) c;
                cout<<"Color chosen: "<<colortostring(color)<<endl;
                break;
            } else
                cout << "error" << endl;
        }
    }
    void print() {Card::print();}
    void applyEffect(Player*& currentPlayer, Player*& nextPlayer, int& direction, int& stack) override {
        if (cardType == WILD_DRAW_FOUR) {
            cout << "Next player draws 4 cards!\n";
            stack += 4;
        }
    }};
class ActionCard: public Card {
public:
    ActionCard(Color color, CardType cardtype) : Card(color, cardtype) {}
    bool playCard(Card* topCard) {
        if (topCard->cardType == DRAW_TWO || topCard->cardType == WILD_DRAW_FOUR) {
            if (cardType == DRAW_TWO || cardType == WILD_DRAW_FOUR) {
                return true;
            }
            return false;
        }
        if (color == topCard->color) {
            return true;
        }
        if (cardType == topCard->cardType) {
            return true;
        }
        return false;
    }
    void print() { Card::print(); }

    void applyEffect(Player*& currentPlayer, Player*& nextPlayer, int& direction, int& stack) override {
        switch (cardType) {
            case SKIP:
                cout << "Next player's turn skipped!\n";
                break;
            case DRAW_TWO:
                cout << "Next player draws 2 cards!\n";
                stack += 2;
                break;
            case REVERSE:
                direction *= -1;
                cout << "Direction reversed!\n";
                break;
            default:
                break;
        }
    }};

class Player {
public:
    int id;
    vector<Card *> hand;
    Player(int id) {
        this->id = id;
    }
    virtual void playTurn(Card*& topCard, int& stack) = 0;
    void drawCard(int stack) {
        for (int i = 1; i <= stack; i++) {
            Color randomColor = (Color) ((rand() % 4)+1);
            CardType randomCardType = (CardType) (rand() % 6);
            if (randomCardType == 0) {
                int randomNumber = rand() % 10;
                hand.push_back(new NumberCard(randomColor, randomCardType, randomNumber));
            } else if (randomCardType == WILD or randomCardType == WILD_DRAW_FOUR) {
                hand.push_back(new WildCard(randomCardType));
            } else {
                hand.push_back(new ActionCard(randomColor, randomCardType));

            }
        }

    }
};

class HumanPlayer : public Player {
public:
    HumanPlayer(int id) : Player(id) {};
    void playTurn(Card*& topCard, int& stack) {
        vector<int> playableCards;
        cout << "Player " << id << "'s turn. Top card: ";
        topCard->print();
        cout << "\n\nYour hand:\n";
        for (int i = 0; i < hand.size(); i++) {
            cout << i + 1 << ": ";
            hand[i]->print();
            cout << endl;
            if (hand[i]->playCard(topCard) && stack==0) {
                playableCards.push_back(i);
            }
        }
        if (stack > 0) {    //shows stackable cards
            bool canStack = false;
            for (int i = 0; i < hand.size(); i++) {
                if ((topCard->cardType == DRAW_TWO or topCard->cardType == WILD_DRAW_FOUR) ||
                    (hand[i]->cardType == DRAW_TWO  or hand[i]->cardType == WILD_DRAW_FOUR)) {
                    canStack = true;
                    cout << i + 1 << ": ";
                    hand[i]->print();
                    cout << " (stackable)\n";
                    playableCards.push_back(i);
                }
            }

            if (!canStack) {
                cout << "No stackable card. Drawing " << stack << " cards.\n";
                drawCard(stack);
                stack = 0;
                for (int i = 0; i < hand.size(); i++) {
                    if (hand[i]->playCard(topCard)) {
                        playableCards.push_back(i);
                    }
                }
                if (playableCards.empty()) {
                    return;
                }
            }
        }
        if (playableCards.empty()) {
            cout << "\nNo playable cards. You must draw. \n";
            drawCard(1);
            for (int i = 0; i < hand.size(); i++) {
                if (hand[i]->playCard(topCard)) {
                    playableCards.push_back(i);
                }}
            if(playableCards.empty()){
                return;
            }}
        cout << "\nPlayable cards: \n";
        for (int j = 0; j < playableCards.size(); j++) {
            int index = playableCards[j];
            cout << j + 1 << ": ";// index to j
            hand[index]->print();
            cout << endl;
        }
        int choice;
        while (true) {
            cout << "\nEnter the index of the card you want to play: ";
            cin >> choice;
            choice--;
            if (choice >= 0 && choice < (int)playableCards.size()) {
                int realIndex = playableCards[choice];
                if (hand[realIndex]->playCard(topCard)) {//double check
                    topCard = hand[realIndex];
                    if (topCard->cardType == WILD || topCard->cardType == WILD_DRAW_FOUR) {
                        ((WildCard *) topCard)->chooseColor();
                    }
                    cout << "Played: ";
                    topCard->print();
                    cout << "\n";
                    vector<Card *> newHand;
                    for (int i = 0; i < hand.size(); i++) {
                        if (i != realIndex) {
                            newHand.push_back(hand[i]);
                        }
                    }
                    hand = newHand;
                    break;
                }
            }
            cout << "Invalid input, Try again.\n";
        }
    }

};
class BotPlayer : public Player {
public:
    BotPlayer(int id) : Player(id) {};
    void playTurn(Card*& topCard, int& stack) {
        cout << "Bot " << id << "'s turn. Top card: ";
        topCard->print();
        cout << endl;
        int bestIndex = -1;
        if (stack > 0) {
            for (int i = 0; i < hand.size(); i++) {
                if ((topCard->cardType == DRAW_TWO or topCard->cardType == WILD_DRAW_FOUR) ||
                    (hand[i]->cardType == DRAW_TWO  or hand[i]->cardType == WILD_DRAW_FOUR)) {
                    bestIndex = i;
                    break;
                }
            }

            if(bestIndex == -1){
                cout << "Bot has no stackable card. Drawing " << stack << " cards.\n";
                drawCard(stack);
                stack = 0;
                for (int i = 0; i < hand.size(); i++) {
                    if (hand[i]->playCard(topCard)) {
                        bestIndex = i;
                    }
                }
                if (bestIndex == -1) {
                    return;
                }


            }}
        for (int i = 0; i < hand.size(); i++) {
            if (hand[i]->playCard(topCard)) {
                bestIndex = i;
            }
        }
        if (bestIndex == -1) {
            cout << "Bot has no playable card. Drawing...\n";
            drawCard(1);
            for (int i = 0; i < hand.size(); i++) {
                if (hand[i]->playCard(topCard)) {
                   bestIndex = i;
                }}
            if(bestIndex == -1){
                return;
            }
        }
        Card* chosen = hand[bestIndex];
        topCard = chosen;
        cout << "Bot played: ";
        topCard->print();
        cout << endl;
        if (topCard->cardType == WILD || topCard->cardType == WILD_DRAW_FOUR) {
            Color randomColor = (Color) ((rand() % 4) + 1);
            ((WildCard *) topCard)->color = randomColor;
            cout << "Bot randomly chose color: " << colortostring(randomColor) << endl;
        }
        vector<Card *> newHand;
        for (int i = 0; i < hand.size(); i++) {
            if (i != bestIndex) {
                newHand.push_back(hand[i]);
            }
        }
        hand = newHand;
    }
};

class Game {
public:
    vector<Player *> players;
    Card* topCard;
    int direction = 1;
    int currentIndex = 0;
    int stack = 0;

    Game() {
        players.push_back(new HumanPlayer(1));
        players.push_back(new BotPlayer(2));
        players.push_back(new BotPlayer(3));
        players.push_back(new BotPlayer(4));
        for (int i = 0; i < players.size(); i++) {
            players[i]->drawCard(7);
        }
        Color c = (Color) ((rand() % 4)+1);
        topCard = new NumberCard(c, NUMBER, rand() % 10);
    }

    void start() {
        while (true) {
            Player*& current = players[currentIndex];
            Player*& next = players[(currentIndex + direction + players.size()) % players.size()];
            current->playTurn(topCard, stack);
            CardType lastCardType = topCard->cardType;
            topCard->applyEffect(current , next, direction, stack);
            if (current->hand.empty()) {
                cout << "\nPlayer " << current->id << " wins!\n";
                break;
            }
            if (lastCardType == SKIP) {//removed from applyeffect()
                currentIndex = (currentIndex + 2 * direction + players.size()) % players.size();
            } else {
                currentIndex = (currentIndex + direction + players.size()) % players.size();
            }
        }
    }
};

int main() {
    srand(time(0));
    Game game;
    game.start();
    return 0;
}
