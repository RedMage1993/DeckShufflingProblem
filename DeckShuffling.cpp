// Fritz Ammon
// March 7, 2014
// Shuffle Interview Problem

/*
	The exercise solves a coding problem that involves shuffling a deck of cards. 
	The problem description is as follows:
 
	You are given a deck containing 313 cards. While holding the deck: 
		1. Take the top card off the deck and set it on the table 
		2. Take the next card off the top and put it on the bottom of the deck in 
			your hand. 
		3. Continue steps 1 and 2 until all cards are on the table. This is a round. 
		4. Pick up the deck from the table and repeat steps 1-3 until the deck is in 
			the original order. 

	Write a program to determine how many rounds it will take to put the deck back 
into the original order. This will involve creating a data structure to represent the 
order of the cards. This program should be written in C. It should take a number 
of cards in the deck as a command line argument and write the result to stdout.
*/

// Explanation of my solution:
//
//		Trying to solve this problem without realizing the benefit of a simple mathematical concept
// and the use of a poor solution could result in an extremely large number of simulations of
// card shuffling for a deck with 313 cards. Thus, it would take too long to compute and I must
// find another way to solve this by finding a pattern which I have illustrated in the image below:
// http://f.cl.ly/items/223e2u063C3k0o460P3r/shuffling.png
// It takes 12 rounds to return 12 cards back to order; feel free to try that as input yourself as verification.
//
// 		You might also notice something about what happens when a card returns to its position.
// Its numbers for the card, or the numbers in that card's column, would repeat. We were lucky that
// all the numbers returned to their original position after 12 rounds, but that may not always happen.
// Thus, we count how many rounds each card takes, then calculate the LCM which tells us how many rounds
// are required by the cards altogether to get the deck back in order at the same time.

#include <stdio.h>
#include <stdlib.h>

typedef enum {FALSE, TRUE} BOOL;

typedef struct Card
{
	unsigned short int n; // card number
	struct Card *p; // next card in deck
	unsigned int r; // loop length
} cardType;

void createDeck(cardType **d, cardType **b, unsigned short s);
void destroyDeck(cardType **d);
void removeTop(cardType **d, cardType **t);
void shiftDeck(cardType **d, cardType **b);
BOOL checkDeck(cardType* d, unsigned int r);
void makeKey(cardType* d, unsigned short *k, unsigned short *c, unsigned short s);
void adjustDeck(cardType* d, unsigned short *k, unsigned short *c, unsigned short s);
unsigned int gcd(unsigned int a, unsigned int b);
unsigned int lcm(unsigned int a, unsigned int b);
unsigned int lcmLoop(cardType* d);

int main()//int argc, char **argv)
{
	cardType *topDeck = NULL, *bottomDeck = NULL, *topTable = NULL;
	cardType *nextBottom = NULL; // the topDeck
	unsigned int rounds = 0;
	BOOL done = FALSE;
	unsigned short deckSize = 0;
	unsigned short *key;
	unsigned short *copy;

	/*if (argc != 2)
		return 0;

	sscanf_s(argv[1], "%d", &deckSize);*/

	scanf("%d", &deckSize);

	key = (unsigned short*) malloc(sizeof(unsigned short) * deckSize);
	copy = (unsigned short*) malloc(sizeof(unsigned short) * deckSize);
	
	createDeck(&topDeck, &bottomDeck, deckSize);
	nextBottom = topDeck; // we know our current top will be the bottom of our next deck
	
	// keep removing and shifting until deck is empty
	do
	{
		removeTop(&topDeck, &topTable);
		
		if (topDeck)
		{
			if (topDeck != bottomDeck) // cannot shift if already bottom
				shiftDeck(&topDeck, &bottomDeck);
		}
		else
		{
			rounds++; // all cards on table
			topDeck = topTable; // table deck becomes our new deck
			topTable = NULL;

			bottomDeck = nextBottom;
			nextBottom = topDeck;

			done = TRUE;
		}
	} while (!done);

    // We only needed to analyze how cards moved around once to create a key
 	// of indexes which lets us get the next position of a card in O(1).
	makeKey(topDeck, key, copy, deckSize);

	// Next we keep adjusting the deck following the key until cards are back in place.
	while (!checkDeck(topDeck, rounds))
	{
		adjustDeck(topDeck, key, copy, deckSize);
		rounds++;
	}

	// find the lcm of all the r's
	rounds = lcmLoop(topDeck);

	destroyDeck(&topDeck);

	free(copy);
	free(key);
	
	printf("rounds for %d numbers = %d\n", deckSize, rounds);

	return 0;
}

void createDeck(cardType **d, cardType **b, unsigned short s)
// d is a pointer to the top of our deck (which is a pointer to a node).
// b is a pointer to the bottom of our deck (which is also a pointer to a node).
// s is the size of the deck.
// We will create the deck and save the corresponding node pointers.
{
	cardType *newCard, *t;
	unsigned short c;
	
	(*d) = (cardType*) malloc(sizeof(cardType));
	(*d)->n = 0;
	(*d)->r = 0;
	(*d)->p = NULL;
	t = (*b) = (*d); // copy of top, and bottom is deck top right now

	for (c = 1; c < s; c++)
	{
		newCard = (cardType*) malloc(sizeof(cardType));
		newCard->n = c;
		newCard->r = 0;
		newCard->p = NULL;
		(*d)->p = newCard;
		(*b) = (*d) = newCard;
	}

	(*d) = t; // return to top
}

void destroyDeck(cardType **d)
// d is the top of our deck.
// Where we create, we must destroy.
{
	cardType *nextCard;

	while ((*d))
	{
		nextCard = (*d)->p;
		free((*d));
		(*d) = nextCard;
	}
}

void removeTop(cardType **d, cardType **t)
// d is the top of our deck.
// t is the top of our table deck.
// We will remove the top of our deck and place at the top of our table deck.
{
	cardType *tempCard;

	// d becomes top next
	// make deck top next point to table top
	tempCard = (*d);
	(*d) = (*d)->p;
	
	tempCard->p = (*t);
	(*t) = tempCard;
}

void shiftDeck(cardType **d, cardType **b)
// d is the top of our deck.
// b is the bottom of our deck.
// We remove the top card and place it underneath the deck.
{
	cardType* tempCard;
	
	// top becomes next card
	tempCard = (*d);
	(*d) = (*d)->p;
	
	// bottom is updated with old top
	// new bottom points to null
	(*b)->p = tempCard;
	(*b) = (*b)->p;
	(*b)->p = NULL;
}

BOOL checkDeck(cardType* d, unsigned int r)
// d is the top of our deck.
// r is the number of rounds it took for a card to get back into its original position in the deck.
// We check each card to see if it its position n matches our index i which tells us it is back in the
// right position. Then we save the number of rounds r.
// We only return TRUE when all of the cards are back in place.
{
	unsigned short i = 0;
	BOOL done = TRUE;

	while (d)
	{
		if (d->n == i)
			d->r = r; // if back in original place then cycle finished, keep round count
		else if (d->r == 0) // if never reached original place and not equal to original place
			done = FALSE;
			
		d = d->p;
		i++;
	}
	
	return done; // done when all r's are not 0
}

void makeKey(cardType* d, unsigned short *k, unsigned short *c, unsigned short s)
// d is the top of our deck.
// k is the key.
// c is a copy of the key.
// Makes a key of indexes to use when adjusting our deck (simulate a round).
{
	unsigned short i;

	// we keep the positions the numbers came from
	// so that now we don't have to calculate them
	// and simply adjust future decks
	for (i = 0; i < s; i++)
	{
		c[i] = k[i] = d->n; // remember, this is after we've shifted (i = 0 does not => n = 0)
		d = d->p;
	}
}

void adjustDeck(cardType* d, unsigned short *k, unsigned short *c, unsigned short s)
// d is the top of our deck.
// k is our key.
// c is a copy of our key.
// s is the size of the deck.
// Simulate a round.
{
	unsigned short *newCopy = (unsigned short*) malloc(sizeof(unsigned short) * s);
	unsigned short i;

	// based on key, find next deck order
	for (i = 0; i < s; i++)
	{
		newCopy[i] = d->n = c[k[i]]; // Get new positions.
		d = d->p;
	}

	// make a copy of this new order for next order
	for (i = 0; i < s; i++)
		c[i] = newCopy[i];

	free(newCopy);
}

unsigned int gcd(unsigned int a, unsigned int b)
// a and b are operands.
// Calculates the greatest common divider.
{
	while (b)
	{
		// euclid's
		a %= b;
		b ^= a;
		a ^= b;
		b ^= a;
	}

	return a;
}

unsigned int lcm(unsigned int a, unsigned int b)
// a and b are operands.
// Calculates the lowest common multiple.
{
	return ((unsigned long long) a * b) / gcd(a, b);
}

unsigned int lcmLoop(cardType* d)
// d is the top of our deck.
// Recursively calculates the lowest common multiple of all the cards' rounds.
{
	if (d)
		return lcm(d->r, lcmLoop(d->p)); // recursively get lcm of all r's
	else
		return 1;
}
