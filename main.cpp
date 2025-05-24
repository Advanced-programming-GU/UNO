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
    bool playCard(Card* topCard){
        if(topCard->cardType==DRAW_TWO || topCard->cardType==WILD_DRAW_FOUR){
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
    bool playCard(Card *topCard) {
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
                currentPlayer = nextPlayer;
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
    /* no Idea
    bool hasPlayable(Card* topCard) {
        for (int i = 0; i < hand.size(); i++) {
            Card* c = hand[i];
            if (c->playCard(topCard)) {
                return true;
            }
        }
        return false;
    }
    bool canStack(Card* topCard) {
        for (int i = 0;i<hand.size();i++) {
            Card* c = hand[i];
            if (c->cardType == topCard->cardType) return true;
        }
        return false;
    }
    */
};

class HumanPlayer : public Player {
public:
    HumanPlayer(int id) : Player(id) {};
    void playTurn(Card*& topCard, int& stack) {
        vector<int> playableCards;
        cout << "Player " << id << "'s turn. Top card: ";
        topCard->print();
        cout << "\n\nYour hand:\n";
        //drawcard rule?
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
                if ((topCard->cardType == DRAW_TWO && hand[i]->cardType == DRAW_TWO) ||
                    (topCard->cardType == WILD_DRAW_FOUR && hand[i]->cardType == WILD_DRAW_FOUR)) {
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
            cout << index + 1 << ": ";
            hand[index]->print();
            cout << endl;
        }
        int choice;
        while (true) {
            cout << "\nEnter the index of the card you want to play: ";
            cin >> choice;
            if (choice >= 1 && choice <= 100 && hand[choice - 1]->playCard(topCard)) {
                topCard = hand[choice - 1];
                if (topCard->cardType == WILD_DRAW_FOUR){
                    stack = stack + 4;
                }
                if (topCard->cardType == DRAW_TWO){
                    stack = stack + 2;
                }
                if (topCard->cardType == WILD || topCard->cardType == WILD_DRAW_FOUR) {
                    ((WildCard *) topCard)->chooseColor();//?
                }
                cout << "Played: ";
                topCard->print();
                cout << "\n";
                break;
            } else {
                cout << "Invalid input, Try again.\n";
            }
        }
        vector<Card *> newHand;
        for (int i = 0; i < hand.size(); i++) {
            if (i != choice - 1) {
                newHand.push_back(hand[i]);
            }
        }
        hand = newHand;
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
        vector<int>playableCards;
        for (int i = 0; i < hand.size(); i++) {
            if (hand[i]->playCard(topCard)) {
                bestIndex = i;
            }
        }
        if (stack > 0) {
            bool canStack = false;
            for (int i = 0; i < hand.size(); i++) {
                if ((topCard->cardType == DRAW_TWO && hand[i]->cardType == DRAW_TWO) ||
                    (topCard->cardType == WILD_DRAW_FOUR && hand[i]->cardType == WILD_DRAW_FOUR)) {
                    canStack = true;
                }
            }

            if(!canStack){
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
        if (bestIndex == -1) {
            cout << "Bot has no playable card. Drawing...\n";
            drawCard(1);
            return;
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
        if (topCard->cardType == WILD_DRAW_FOUR){
            stack = stack + 4;
        }
        if (topCard->cardType == DRAW_TWO){
            stack = stack + 2;
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
            Player*& next = players[(currentIndex + direction + players.size()) % players.size()];//value doesnt end up as negative (0 + -1)
            current->playTurn(topCard, stack);
            topCard->applyEffect(current , next, direction, stack);
            if (current->hand.empty()) {
                cout << "\nPlayer " << current->id << " wins!\n";
                break;
            }
            currentIndex = (currentIndex + direction + players.size()) % players.size();
        }
    }
};

int main() {
    srand(time(0));
    Game game;
    game.start();
    return 0;
}
//handprint before stack commands
//stack rule should change
//bot played card should be announced
