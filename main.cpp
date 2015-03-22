#include <string>
#include <iostream>
#include "hash.h"

using namespace std;

/***
   * 	COP3530 - Exercise 3
   *	Chris Pergrossi
   *
   * This exercise was particularly easy, as I wrote about 95% of this code 13 years ago
   * and had it published online (dated 19 August 2002, when I was 15!):
   *
   *			http://flipcode.com/archives/Hash_Table.shtml
   *
   * 
   ***/

int main (void)
{
	int opcode;
	string key;
	int waste;


	// grab the initial table size
	int table_size;

	cin >> table_size;

	// allocate the hash table
	CHashTable<int> table(table_size);


	// input loop
	do
	{
		// grab the next instruction and the key
		cin >> opcode;
		cin >> key;

		// add this hash, if it doesn't exist
		if (opcode == 1) {

			if (!table.getHash( toHash( key.c_str() ), &waste )) {
				table.addHash( toHash( key.c_str() ), waste );
			}

		// check if a hash is in the table
		} else if (opcode == 2) {

			if (table.getHash( toHash( key.c_str() ), &waste )) {
				cout << "Yes " << endl;
			} else {
				cout << "No "  << endl;
			}

		}

	// keep looping unless opcode was zero
	} while (opcode != 0);


	// return success
	return 0;
}