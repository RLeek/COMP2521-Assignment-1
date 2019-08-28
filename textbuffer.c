#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "textbuffer.h"


#define TRUE 1
#define FALSE 0

struct textbufferNode {
	struct textbufferNode *next;
	struct textbufferNode *prev;
	char *line;
}textbufferNode;

typedef struct textbufferNode *TBNode;

struct textbuffer{
	int nlines;
	struct textbufferNode* first;
	struct textbufferNode* last;
}textbuffer;

static TBNode newTBNode(char *line);
static int extract_line(char text[], int index, int length, char line[length]);
static char *addrich(int length, int array[length], char type[length], char *line, int index);
static int search_closer(int charIndex, int *new_start, char *line, int type);
static void free_nodes(TBNode start);
static int text_length(TB tb);
static int num_places(int n);

/* Allocate a new textbuffer whose contents is initialised with the text given
 * in the array.
 */
TB newTB (char text[]) {

	//Initialization
	TB newTB = malloc(sizeof(textbuffer));
	assert(newTB != NULL);
	newTB->nlines = 0;
	newTB->first = NULL;
	newTB->last = NULL;

	//Case 1: Empty String
	if (text[0] == '\0') {
		return newTB;
	}	

	//Case 2: Normal String
	int length = strlen(text) + 1;
	char line[length];
	int new_start = 0;

	//First node
	new_start = extract_line(text, new_start, length, line);
	if (new_start != -1) {
		newTB->nlines = 1;
		newTB->first = newTBNode(line);
		newTB->last = newTB->first;
		newTB->first->next = NULL;
		newTB->first->prev = NULL;
	}

	//Middle and End Node
	new_start = extract_line(text, new_start,length, line);
	while(new_start != -1) {
		newTB->nlines++;
		newTB->last->next = newTBNode(line);
		newTB->last->next->prev = newTB->last;
		newTB->last = newTB->last->next;
		new_start = extract_line(text, new_start, length, line);
	}
	newTB->last->next = NULL;
	return newTB;
}

/* Allocates a new textbufferNode whose contents is initialized with the string given
 * in the array.
 */
static TBNode newTBNode(char *line) {

	TBNode newL = malloc(sizeof(textbufferNode));
	assert(newL != NULL);
	newL->line = strdup(line);
	newL->next = NULL;
	newL->prev = NULL;
	return newL;
}

/* Given an initial starting value, increments through a given array, until it reaches 
 * a new line character, where it returns the index of the new line character. '\0' is
 * returned if the inital starting value never increments
 */
static int extract_line(char text[], int index, int length, char line[length]) {

	char curr_char = text[index];
	line[0] = text[index];
	int i = 0;
	while ((curr_char != '\n') && (curr_char != '\0')) {
		index++;
		i++;
		curr_char = text[index];
		line[i] = text[index];
	}
	if ((i == 0) && (curr_char == '\n')) {
		line[0] = '\0';
		index++;
		return index;
	}
	if ((i == 0) && (curr_char == '\0')) {
		return -1;
	}
	line[i++] = '\0';
	index++;
	return index;	
}

/* Free the memory occupied by the given textbuffer.  It is an error to access
 * the buffer afterwards.
 */
void releaseTB (TB tb) {

	//Case 1: Only one line
	if (tb->nlines == 1) {
		free(tb->first->line);
		free(tb->first);
	}

	//Case 2: Two or more lines
	if (tb->nlines > 1) {
		TBNode curr = tb->first->next;
		while (curr->next!= NULL) {
			free(curr->prev->line);
			free(curr->prev);
			curr = curr->next;
		}
		free(curr->prev->line);
		free(curr->prev);
		free(curr->line);
		free(curr);
	}
	free(tb);
}

/* Allocate and return an array containing the text in the given textbuffer.
 * add a prefix corrosponding to line number iff showLineNumbers == TRUE
 */
char *dumpTB (TB tb, int showLineNumbers){

	// Case 1: TB is empty
	if (tb->nlines == 0) {
		char *dump = malloc(sizeof(char) * 2);
		return strcpy(dump, "");
	}

	//Case 2: Normal String

	//Initialization
	int length = text_length(tb);
	if (showLineNumbers == TRUE) {
		length = length + (tb->nlines) * num_places(tb->nlines);
		//Ensures that there will always be space for line numbers
	}
	char *dump = malloc(sizeof(char) * length);
	dump[0] = '\0';

	//For first line;
	int num = 1;
	TBNode curr = tb->first;

	while (curr != NULL) {
		if (showLineNumbers == TRUE) {
			char line_num[10];
			snprintf(line_num, 10, "%d", num);
			num++;
			strcat(dump, line_num);
			strcat(dump, ". ");
		}
		strcat(dump, curr->line);
		strcat(dump, "\n");
		curr = curr->next;
	}
	return dump;
}

/* 
 * Determine length of whole string to 
 * Avoid reallocing
 */
static int text_length(TB tb) {
    
    TBNode curr = tb->first;
    int length = 0;
    while (curr != NULL) {
    	length = length + strlen(curr->line)+5;
    	curr = curr->next;
    }
    return length;
}

/* 
 * Fastest Way to get number of digits for highest line-number
 */
static int num_places(int n) {

    if (n < 10) {
    	return 1;
   	}
    if (n < 100) {
    	return 2;
    }
    if (n < 1000) {
    	return 3;
    }
    if (n < 10000) {
    	return 4;
    }
    if (n < 100000) {
    	return 5;
    }
    if (n < 1000000) {
    	return 6;
    }
    if (n < 10000000) {
    	return 7;
    }
    if (n < 100000000) {
    	return 8;
    }
    if (n < 1000000000) {
    	return 9;
    }
    return 10;
}

/* Return the number of lines of the given textbuffer.
 */
int linesTB (TB tb){
	return tb->nlines;
}

/* Add a given prefix to all lines between pos1 and pos2
 *
 * - The program is to abort() with an error message if line 'pos1' or line
 *   'pos2' is out of range.  The first line of a textbuffer is at position 0.
 */
void addPrefixTB (TB tb, int pos1, int pos2, char* prefix){

	//Case 1: Positions out of range
	if (pos1 > pos2) {
		printf("Invalid positions");
		abort();
	}

	if (pos1 < 0 || pos2 < 0) {
		printf("Positions out of range");
		abort();
	}

	if (pos1 > tb->nlines-1 || pos2 > tb->nlines-1) {
		printf("Positions out of range");
		abort();
	}

	//Case 2: Empty/null prefix
	if (strcmp(prefix, "") == 0) {
		return;
	}
	if (prefix == NULL) {
		printf("Invalid positions");
		abort();
	}

	//Case 3: Empty TB
	if (tb->nlines == 0) {
		return;
	}

	//Case 4: Normal scenario
	TBNode curr = tb->first;
	int position = 0;
	int prefix_length = strlen(prefix);
	while (curr != NULL) {
		if ((position >= pos1) && (position <= pos2)) {
			int length = strlen(curr->line) + prefix_length + 2;
			char *new_line = malloc(sizeof(char) * length);
			new_line[0] = '\0';
			new_line = strcat(new_line, prefix);
			new_line = strcat(new_line, curr->line);
			free(curr->line);
			curr->line = new_line;
		}
		position++;
		curr = curr->next;
	}
}

/* Merge 'tb2' into 'tb1' at line 'pos'.
 *
 * - Afterwards line 0 of 'tb2' will be line 'pos' of 'tb1'.
 * - The old line 'pos' of 'tb1' will follow after the last line of 'tb2'.
 * - After this operation 'tb2' can not be used anymore (as if we had used
 *   releaseTB() on it).
 * - The program is to abort() with an error message if 'pos' is out of range.
 */
void mergeTB (TB tb1, int pos, TB tb2){

	//Case 1: Merging with self
	if (tb1 == tb2) {
		return;
	}

	//Case 2: Tb2 is empty
	if (tb2->nlines == 0) {
		free(tb2);
		return;
	}

	//Case 3: pos is out of range
	if (pos > tb1->nlines||pos < 0) {
		printf("Positions out of range");
		abort();
	}
	
	//Case 4: Tb1 is empty
	if (tb1->nlines == 0) {
		//Only possible position is zero
		//Replace first
		tb1->first = tb2->first;
		tb1->last = tb2->last;
		tb1->nlines = tb2->nlines;
		free(tb2);
		return;
	}

	//Case 5: pos is zero
	if (pos == 0) {
		//First retain tb1->first;
		TBNode tb1first = tb1->first;
		//Then link the end of tb2 to tb1->first
		tb2->last->next = tb1first;
		tb1->first = tb2->first;
		tb1first->prev = tb2->last;
		tb1->nlines = tb1->nlines + tb2->nlines;
		free(tb2);
		return;
	}

	//Case 6: pos is nlines
	if (pos == tb1->nlines) {
		tb1->last->next = tb2->first;
		tb2->first->prev = tb1->last;
		tb1->last = tb2->last;
		tb1->nlines = tb1->nlines + tb2->nlines;
		free(tb2);
		return;
	}

	//Case 7: pos is in middle
	TBNode prev_link = tb1->first;
	TBNode after_link = tb1->last;
	TBNode curr = tb1->first;
	int line_num = 0;
	while(curr != NULL) {
		if (line_num == pos) {
			prev_link = curr->prev;
			after_link = curr;
		} 	
		line_num++;
		curr = curr->next;
	}
	prev_link->next = tb2->first;
	tb2->first->prev = prev_link;
	tb2->last->next = after_link;
	after_link->prev = tb2->last;
	tb1->nlines = tb1->nlines + tb2->nlines;
	free(tb2);
	return;
}

/* Copy 'tb2' into 'tb1' at line 'pos'.
 *
 * - Afterwards line 0 of 'tb2' will be line 'pos' of 'tb1'.
 * - The old line 'pos' of 'tb1' will follow after the last line of 'tb2'.
 * - After this operation 'tb2' is unmodified and remains usable independent
 *   of 'tb1'.
 * - The program is to abort() with an error message if 'pos' is out of range.
 */
void pasteTB (TB tb1, int pos, TB tb2) {

	//Case 1: Merging with self
	if (tb1 == tb2) {
		return;
	}

	//Case 2: Tb2 is empty
	if (tb2->nlines == 0) {
		return;
	}

	//Case 3: pos is out of range
	if ((pos > tb1->nlines) || (pos < 0)) {
		printf("Positions out of range");
		abort();
	}

	//Recreate TB2
	TBNode first = newTBNode(tb2->first->line);
	TBNode new_curr = first;
	new_curr->prev = NULL;
	new_curr->next = NULL;
	TBNode tb2curr = tb2->first->next;
	while(tb2curr != NULL) {
		new_curr->next = newTBNode(tb2curr->line);
		new_curr->next->prev = new_curr;
		new_curr->next->next = NULL;
		new_curr = new_curr->next;
		tb2curr = tb2curr->next;
	}

	//Case 4: Tb1 is empty
	if (tb1->nlines == 0) {
		tb1->first = tb2->first;
		tb1->last = tb2->last;
		tb1->nlines = tb2->nlines;

		tb2->first = first;
		tb2->last = new_curr;
		return;
	}

	//Case 5: pos is zero
	if (pos == 0) {
		TBNode tb1first = tb1->first;
		tb2->last->next = tb1first;
		tb1->first = tb2->first;
		tb1first->prev = tb2->last;
		tb1->nlines = tb1->nlines + tb2->nlines;
		tb2->first = first;
		tb2->last = new_curr;
		return;
	}

	//Case 6: pos is nlines
	if (pos == tb1->nlines) {
		tb1->last->next = tb2->first;
		tb2->first->prev = tb1->last;
		tb1->last = tb2->last;
		tb1->nlines = tb1->nlines + tb2->nlines;

		tb2->first = first;
		tb2->last = new_curr;
		return;
	}

	//Case 7: pos is in middle
	TBNode prev_link = tb1->first;
	TBNode after_link = tb1->last;
	TBNode curr = tb1->first;
	int line_num = 0;
	while(curr != NULL) {
		if (line_num == pos) {
			prev_link = curr->prev;
			after_link = curr;
		} 	
		line_num++;
		curr = curr->next;
	}
	prev_link->next = tb2->first;
	tb2->first->prev = prev_link;
	tb2->last->next = after_link;
	after_link->prev = tb2->last;
	tb1->nlines = tb1->nlines + tb2->nlines;
	tb2->first = first;
	tb2->last = new_curr;
	return;
}

/* Cut the lines between and including 'from' and 'to' out of the textbuffer
 * 'tb'.
 *
 * - The result is a new textbuffer (much as one created with newTB()).
 * - The cut lines will be deleted from 'tb'.
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range.
 */
TB cutTB (TB tb, int from, int to){

	//Case 1: tb is empty
	if (tb->nlines == 0) {
		return NULL;
	}

	//Case 2: to is larger than from
	if (to < from) {
		return NULL;
	}

	//Case 3: If to or from is out of range
	if (from < 0 || to < 0) {
		printf("Positions out of range");
		abort();
	}
	if (from > tb->nlines-1 || to > tb->nlines-1) {
		printf("Positions out of range");
		abort();
	}

	TB tb2 = malloc(sizeof(textbuffer));
	assert(tb2!= NULL);
	tb2->first = NULL;
	tb2->last = NULL;
	tb2->nlines = to - from + 1;

	//Case 4: Deleting first node
	if ((from == 0) && (to != tb->nlines - 1)) {

		TBNode first = tb->first;
		TBNode last = tb->first;
		int line_num = 0; 
		while (line_num != to) {
			last = last->next;
			line_num++;
		}
		tb->first = last->next;
		last->next->prev = NULL;
		tb2->first = first;
		tb2->last = last;
		last->next = NULL;
		tb->nlines = tb->nlines - (tb2->nlines);
		return tb2;
	}

	//Case 5: Deleting last node
	if ((from != 0) && (to == tb->nlines - 1)) {
		TBNode first = tb->first;
		TBNode last = tb->last;
		int line_num = 0; 
		while (line_num!= from) {
			first = first->next;
			line_num++;
		}
		tb->last = first->prev;
		first->prev->next = NULL;
		tb2->first = first;
		tb2->last = last;
		first->prev = NULL;
		tb->nlines = tb->nlines - (tb2->nlines);
		return tb2;

	}

	//Case 6: moving whole text
	if ((from == 0) && (to == tb->nlines - 1)) {
		tb2->first = tb->first;
		tb2->last = tb->last;
		tb->last = NULL;
		tb->first = NULL;
		tb->nlines = tb->nlines - (tb2->nlines);
		return tb2;
	}

	//Case 6: Deleting normally
	TBNode first = tb->first;
	TBNode last = tb->last;
	TBNode curr = tb->first;
	int line_num = 0; 
	while (curr != NULL) {
		if (line_num == from) {
			first = curr;
		} 
		if (line_num == to) {
			last = curr;
		}
		line_num++;
		curr = curr->next;
	}
	first->prev->next = last->next;
	last->next->prev = first->prev;
	first->prev = NULL;
	last->next = NULL;
	tb2->first = first;
	tb2->last = last;
	tb->nlines = tb->nlines - (tb2->nlines);
	return tb2;
}

/*  Return a linked list of Match nodes of all the matches of string search
 *  in tb
 *
 * - The textbuffer 'tb' will remain unmodified.
 * - The user is responsible of freeing the returned list
 */
Match searchTB (TB tb, char* search){

	//Case 1: search for nothing;
	if (strcmp(search, "") == 0) {
		return NULL;
	}

	//Case 2: search for NULL;
	if (search == NULL) {
		printf("Invalid search");
		abort();
	}

	//Case 3: No lines;
	if (tb->nlines == 0) {
		return NULL;
	}

	//Case 4: Normal search:
	//First create match
	Match new_node = malloc(sizeof(matchNode));
	Match new_match = new_node;
	new_node->next = NULL;

	TBNode curr = tb->first;
	int line_num = 1;
	int found = 0;
	int search_length = strlen(search);
	while (curr!= NULL) {
		char *charindex = strstr(curr->line,search);
		if (found == 0) {
			//Do first line;
			if (charindex != NULL) {
				new_node->lineNumber = line_num;
				new_node->charIndex = (charindex - curr->line);
				new_node->next = NULL;
				found++;
			}	
			if (charindex != NULL) {
				charindex = strstr(curr->line + new_node->charIndex + search_length, search);
			}
		}	

		while (charindex != NULL) {
			new_node->next = malloc(sizeof(matchNode));
			new_node->next->lineNumber = line_num;
			new_node->next->charIndex = (charindex - curr->line);
			new_node = new_node->next;
			new_node->next = NULL;
			charindex = strstr(curr->line + new_node->charIndex + search_length, search);
		}
		line_num++;
		curr = curr->next;
	}

	if (found == 0) {
		free(new_match);
		new_match = NULL;
	}
	return new_match;
}

/* Remove the lines between and including 'from' and 'to' from the textbuffer
 * 'tb'.
 *
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range.
 */
void deleteTB (TB tb, int from, int to){

	//Case 1: tb is empty
	if (tb->nlines == 0) {
		return;
	}

	//Case 2: to is larger than from
	if (to < from) {
		printf("Invalid position");
		abort();
	}

	//Case 3: If to or from is out of range
	if (from < 0 || to < 0) {
		printf("Positions out of range");
		abort();	
	}
	if (from > tb->nlines-1 || to > tb->nlines-1) {
		printf("Positions out of range");
		abort();	
	}

	//Case 4: Deleting first node
	if ((from == 0) && (to != tb->nlines - 1)) {
		TBNode first = tb->first;
		TBNode last = tb->first;
		int line_num = 0; 
		while (line_num != to) {
			last = last->next;
			line_num++;
		}
		//adjust tb
		tb->first = last->next;
		last->next->prev = NULL;
		//make tb2
		last->next = NULL;
		free_nodes(first);
		//free from first to last
		tb->nlines = tb->nlines - (to-from + 1);
		return;
	}
 
	//Case 5: Deleting last node
	if ((from != 0) && (to == tb->nlines - 1)) {
		TBNode first = tb->first;
		//TBNode last = tb->last;
		int line_num = 0; 
		while (line_num != from) {
			first = first->next;
			line_num++;
		}
		tb->last = first->prev;
		first->prev->next = NULL;
		free_nodes(first);
		tb->nlines = tb->nlines - (to - from + 1);
		return;

	}

	//Case 6: moving whole text
	if ((from == 0) && (to == tb->nlines - 1)) {
		free_nodes(tb->first);
		tb->first = NULL;
		tb->last = NULL;
		tb->nlines = tb->nlines - (to - from + 1);
		return;
	}

	//Case 6: Deleting normally
	TBNode first = tb->first;
	TBNode last = tb->last;
	TBNode curr = tb->first;
	int line_num = 0; 
	while (curr != NULL) {
		if (line_num == from) {
			first = curr;
		} 
		if (line_num == to) {

			last = curr;
		}
		line_num++;
		curr = curr->next;
	}
	first->prev->next = last->next;
	last->next->prev = first->prev;
	first->prev = NULL;
	last->next = NULL;
	free_nodes(first);
	tb->nlines = tb->nlines - (to - from + 1);
	return;
}

/* 
 * Frees nodes starting from a start up until it reaches NULL
 * Essentially releaseTB if we didn't have the free component
 */ 
static void free_nodes(TBNode start) {

	TBNode curr = start;
	if (curr->next == NULL){
		free(curr->line);
		free(curr);
		return;
	}
	curr = start->next;
	while (curr->next!= NULL) {
		free(curr->prev->line);
		free(curr->prev);
		curr = curr->next;
	}
	free(curr->prev->line);
	free(curr->prev);
	free(curr->line);
	free(curr);

}

/* Search every line of tb for each occurrence of a set of specified subsitituions
 * and alter them accordingly
 *
 * refer to spec for table.
 */
void formRichText (TB tb){

	//Case 1: Empty tb
	if (tb->nlines == 0) {
		return;
	}

	//Case 2: normal case;
	TBNode curr = tb->first;
	while (curr != NULL) {
		int index = 0;
		int length = strlen(curr->line);
		//Tells you if it is '_' or '*'
		char type[length];
		type[0]= '\0';
		//Tells you where to find these
		int array[length];
		int charIndex = 0;
		char  letter = curr->line[charIndex];
		while (letter != '\0') {
			int found = 0;
			if (letter == '*') {
				int prev = charIndex;
				found = search_closer(charIndex, &charIndex, curr->line, 1);
				if (found == 1) {
					type[index *2] = '1';
					type[index *2 +1] = '1';
					array[index *2] = prev;
					array[index *2 +1] = charIndex;
					index++;
				}
			} else if (letter == '_') {
				int prev = charIndex;
				found = search_closer(charIndex, &charIndex, curr->line, 2);
				if (found == 1) {
					type[index *2] = '2';
					type[index *2 +1] = '2';
					array[index *2] = prev;
					array[index *2 +1] = charIndex;
					index++;
				}
			} else if ((letter == '#') && (charIndex == 0) && (length > 1)) {
				type[index *2] = '3';
				type[index *2 +1] = '3';
				array[0] = 0;
				array[1] = length;
				index++;
				charIndex = length-1;
			}	
			charIndex++;
			letter = curr->line[charIndex];
		}
		if (index > 0) {
			curr->line = addrich(length, array, type, curr->line, index * 2);
		}
		curr = curr->next;
	}
}

/* Searches through a line to find the next valid '*' or '_'
 * If none found, it returns FALSE
 * If found, it returns TRUE and increments charIndex in the caller
 */
static int search_closer(int charIndex, int *new_start, char *line, int type) {

	charIndex = charIndex + 1;
	char letter = line[charIndex];

	if ((type == 1) && (letter == '*')) {
		return FALSE;
	} 
	if ((type == 2) && (letter == '_')) {
		return FALSE;
	} 

	while (letter != '\0') {
		if ((letter == '*') && (type == 1)) {
			*new_start = charIndex;
			return TRUE;
		} else if ((letter == '_') && (type == 2)) {
			*new_start = charIndex;
			return TRUE;
		}
		charIndex++;
		letter = line[charIndex];
	}
	return FALSE;
}
	


/* 
 * First go through all of the text and null terminate
 * any '*' and '_' to use strcat. Then puts in the headers and closers
 */
static char *addrich(int length, int array[length], char type[length], char *line, int index) {

	char *new_line = malloc(length * 5 * sizeof(char));
	new_line[0] = '\0';

	int i = 0 ;
	while(i<index) {
		line[array[i]] = '\0';
		i++;
	}

	i = 0 ;
	int check = 0;
	int first = 0;
	while(i<index) {
		if ((type[i] == '1') && (check == 0)) {
			if (first == 0) {
				strcat(new_line, line);
				first = 1;
			}
			strcat(new_line, line + array[i]);
			strcat(new_line,"<b>");
			strcat(new_line, line + array[i] + 1);
			check = 1;

		} else if ((type[i] == '1') && (check == 1)) {
			strcat(new_line, line + array[i]);
			strcat(new_line,"</b>");
			strcat(new_line, line + array[i] + 1);
			check = 0;
		}else if ((type[i] == '2') && (check == 0)) {
			if (first == 0) {
				strcat(new_line, line);
				first = 1;
			}
			strcat(new_line, line + array[i]);
			strcat(new_line,"<i>");
			strcat(new_line, line + array[i] + 1);
			check = 1;
		}else if ((type[i] == '2') && (check == 1)) {
			strcat(new_line, line + array[i]);
			strcat(new_line,"</i>");
			strcat(new_line, line + array[i] + 1);
			check = 0;
		} else if ((type[i] == '3') && (check == 0)) {
			strcat(new_line,"<h1>");
			strcat(new_line, line + array[i] + 1);
			check = 1;
		}else if ((type[i] == '3') && (check == 1)) {
			strcat(new_line,"</h1>");
			check = 0;
		}
		i++;
	}
	free(line);
	return new_line;
}




/* Your whitebox tests
 */
void whiteBoxTests() {

	//Tests for newTB and LinesTB

	//Boundary Condition tests

	//Test an empty string
	TB testtb = newTB("");
	assert(testtb->nlines == 0);
	assert(linesTB(testtb) == 0);
	assert(testtb->first == testtb->last);
	releaseTB (testtb);

	//Test a new line character only
	testtb = newTB("\n");
	assert(testtb->nlines == 1);
	assert(linesTB(testtb) == 1);
	assert(testtb->first == testtb->last);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB (testtb);

	//Test 2 new line characters only
	testtb = newTB("\n\n");
	assert(testtb->nlines == 2);
	assert(linesTB(testtb) == 2);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB (testtb);

	//Test multiple new line characters
	testtb = newTB("\n\n\n\n");
	assert(testtb->nlines == 4);
	assert(linesTB(testtb) == 4);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB (testtb);

	//Test a large string
	testtb = newTB("Another thing that got forgotten was the fact that against all probability a sperm whale had suddenly been called into existence several miles above the surface of an alien planet.\n");
	assert(testtb->nlines == 1);
	assert(linesTB(testtb) == 1);
	assert(testtb->first == testtb->last);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "Another thing that got forgotten was the fact that against all probability a sperm whale had suddenly been called into existence several miles above the surface of an alien planet.") == 0);
	releaseTB (testtb);

	//Normal Condition tests

	//Test 1 line:
	testtb = newTB("Line01\n");
	assert(testtb->nlines == 1);
	assert(linesTB(testtb) == 1);
	assert(testtb->first == testtb->last);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	releaseTB (testtb);

	//Test 2 line:
	testtb = newTB("Line01\nLine02\n");
	assert(testtb->nlines == 2);
	assert(linesTB(testtb) == 2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Line02") == 0);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	releaseTB (testtb);

	//Test 2 line, with empty string:
	testtb = newTB("Line01\n\n");
	assert(testtb->nlines == 2);
	assert(linesTB(testtb) == 2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	releaseTB (testtb);

	//Test 3 line:
	testtb = newTB("Line01\nLine02\nLine03\n");
	assert(testtb->nlines == 3);
	assert(linesTB(testtb) == 3);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Line03") == 0);
	releaseTB (testtb);

	//Test 3 line, with empty middle string:
	testtb = newTB("Line01\n\nLine02\n");
	assert(testtb->nlines == 3);
	assert(linesTB(testtb) == 3);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Line02") == 0);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	releaseTB (testtb);

	//Test 3 line, with empty string:
	testtb = newTB("Line01\nLine02\n\n");
	assert(testtb->nlines == 3);
	assert(linesTB(testtb) == 3);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	releaseTB (testtb);

	//Test 20 lines, normal condition:
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	assert(testtb->nlines == 20);
	assert(linesTB(testtb) == 20);
	assert(testtb->first->prev == NULL);
	assert(testtb->last->next == NULL);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Line20") == 0);
	releaseTB (testtb);
	
	//Tests for dumpTB:

	//Boundary Condition tests:

	//Test an empty string
	testtb = newTB("");
	assert(strcmp(dumpTB(testtb, FALSE),"") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"") == 0);
	releaseTB (testtb);

	//Test a new line character 
	testtb = newTB("\n");
	assert(strcmp(dumpTB(testtb, FALSE),"\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. \n") == 0);
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB(testtb);

	//Test 2 new line character]
	testtb = newTB("\n\n");
	assert(strcmp(dumpTB(testtb, TRUE),"1. \n2. \n") == 0);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	releaseTB(testtb);

	//Test multiple new line characters
	testtb = newTB("\n\n\n\n\n");
	assert(strcmp(dumpTB(testtb, FALSE),"\n\n\n\n\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. \n2. \n3. \n4. \n5. \n") == 0);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	releaseTB(testtb);

	//Test a large string
	testtb = newTB("All assassins had a full-length mirror in their rooms, because it would be a terrible insult to anyone to kill them when you were badly dressed\n");
	assert(strcmp(dumpTB(testtb, FALSE),"All assassins had a full-length mirror in their rooms, because it would be a terrible insult to anyone to kill them when you were badly dressed\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. All assassins had a full-length mirror in their rooms, because it would be a terrible insult to anyone to kill them when you were badly dressed\n") == 0);
	assert(strcmp(testtb->first->line, "All assassins had a full-length mirror in their rooms, because it would be a terrible insult to anyone to kill them when you were badly dressed") == 0);
	releaseTB(testtb);

	//Normal Condition tests:

	//Test 1 line
	testtb = newTB("Line01\n");
	assert(strcmp(dumpTB(testtb, FALSE),"Line01\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. Line01\n") == 0);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	releaseTB(testtb);

	//Test 2 lines
	testtb = newTB("Line01\nLine02\n");
	assert(strcmp(dumpTB(testtb, FALSE),"Line01\nLine02\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. Line01\n2. Line02\n") == 0);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	releaseTB(testtb);

	//Test 20 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	assert(strcmp(dumpTB(testtb, FALSE),"Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. Line01\n2. Line02\n3. Line03\n4. Line04\n5. Line05\n6. Line06\n7. Line07\n8. Line08\n9. Line09\n10. Line010\n11. Line11\n12. Line12\n13. Line13\n14. Line14\n15. Line15\n16. Line16\n17. Line17\n18. Line18\n19. Line19\n20. Line20\n") == 0);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	releaseTB(testtb);

	//Test 10 lines, with both empty and filled strings
	testtb = newTB("\nLine01\n\nLine02\n\nLine03\n\nLine04\n\n");
	assert(strcmp(dumpTB(testtb, FALSE),"\nLine01\n\nLine02\n\nLine03\n\nLine04\n\n") == 0);
	assert(strcmp(dumpTB(testtb, TRUE),"1. \n2. Line01\n3. \n4. Line02\n5. \n6. Line03\n7. \n8. Line04\n9. \n") == 0);
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB(testtb);

	//Tests for AddPrefixTB:

	//Boundary Condition tests:

	//Test a new line and append nothing
	testtb = newTB("\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"");
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB(testtb);

	//Test a new line and append smalls string
	testtb = newTB("\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Small prefix");
	assert(strcmp(testtb->first->line, "Small prefix") == 0);
	releaseTB(testtb);

	//Test a new line and append a large string
	testtb = newTB("\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Very very very very very very very very very very large prefix");
	assert(strcmp(testtb->first->line, "Very very very very very very very very very very large prefix") == 0);
	releaseTB(testtb);

	//Test multiple new lines and append nothing
	testtb = newTB("\n\n\n\n\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"");
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB(testtb);

	//Test multiple new lines and append small string
	testtb = newTB("\n\n\n\n\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Small prefix");
	assert(strcmp(testtb->first->line, "Small prefix") == 0);
	releaseTB(testtb);

	//Test multiple new lines and append large string
	testtb = newTB("\n\n\n\n\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Very very very very very very very very very very large prefix");
	assert(strcmp(testtb->first->line, "Very very very very very very very very very very large prefix") == 0);
	releaseTB(testtb);

	//Test large line and append nothing
	testtb = newTB("Only priests and fools are fearless and I've never been on the best of terms with God.\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"");
	assert(strcmp(testtb->first->line, "Only priests and fools are fearless and I've never been on the best of terms with God.") == 0);
	releaseTB(testtb);

	//Test large line and append small string
	testtb = newTB("Only priests and fools are fearless and I've never been on the best of terms with God.\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Small prefix ");
	assert(strcmp(testtb->first->line, "Small prefix Only priests and fools are fearless and I've never been on the best of terms with God.") == 0);
	releaseTB(testtb);

	//Test large line and append large string
	testtb = newTB("Only priests and fools are fearless and I've never been on the best of terms with God.\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Very very very very very very very very very very large prefix ");
	assert(strcmp(testtb->first->line, "Very very very very very very very very very very large prefix Only priests and fools are fearless and I've never been on the best of terms with God.") == 0);
	releaseTB(testtb);

	//Normal conditions:

	//Test 1 line and append nothing
	testtb = newTB("Line01\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"");
	assert(strcmp(testtb->first->line, "Line01") == 0);
	releaseTB(testtb);

	//Test 1 line and append small string
	testtb = newTB("Line01\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Small prefix ");
	assert(strcmp(testtb->first->line, "Small prefix Line01") == 0);
	releaseTB(testtb);

	//Test 1 line and append large string
	testtb = newTB("Line01\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Very very very very very very very very very very large prefix ");
	assert(strcmp(testtb->first->line, "Very very very very very very very very very very large prefix Line01") == 0);
	releaseTB(testtb);

	//Test 20 lines and append nothing
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"");
	assert(strcmp(testtb->first->line, "Line01") == 0);
	releaseTB(testtb);

	//Test 20 lines and append small string
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Small prefix ");
	assert(strcmp(testtb->first->line, "Small prefix Line01") == 0);
	releaseTB(testtb);

	//Test 20 lines and append large string
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Very very very very very very very very very very large prefix ");
	assert(strcmp(testtb->first->line, "Very very very very very very very very very very large prefix Line01") == 0);
	releaseTB(testtb);

	//Test empty and filled lines and append nothing
	testtb = newTB("\nLine01\n\nLine02\n\nLine03\n\nLine04\n\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"");
	assert(strcmp(testtb->first->line, "") == 0);
	releaseTB(testtb);

	//Test empty and filled lines and append small string
	testtb = newTB("\nLine01\n\nLine02\n\nLine03\n\nLine04\n\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Small prefix");
	assert(strcmp(testtb->first->line, "Small prefix") == 0);
	releaseTB(testtb);

	//Test empty and filled lines and append large string
	testtb = newTB("\nLine01\n\nLine02\n\nLine03\n\nLine04\n\n");
	addPrefixTB(testtb, 0, testtb->nlines-1,"Very very very very very very very very very very large prefix ");
	assert(strcmp(testtb->first->line, "Very very very very very very very very very very large prefix ") == 0);
	releaseTB(testtb);

	//Test appending at the first line only
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\n");
	addPrefixTB(testtb, 0, 0,"Small prefix");
	assert(strcmp(testtb->first->next->line, "Line02") == 0);
	releaseTB(testtb);

	//Test appending at the last line only
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\n");
	addPrefixTB(testtb,testtb->nlines-1, testtb->nlines-1,"Small prefix ");
	assert(strcmp(testtb->last->line,"Small prefix Line010") == 0);
	releaseTB(testtb);

	//Test appending in middle
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\n");
	addPrefixTB(testtb, 5, 9,"Small prefix ");
	assert(strcmp(testtb->first->next->next->next->next->next->line,"Small prefix Line06") == 0);
	assert(strcmp(testtb->first->next->next->next->next->line,"Line05") == 0);
	releaseTB(testtb);

	//Tests for merge TB:

	//Boundary Condition tests:

	//Merge two empty strings
	testtb = newTB("");
	TB testtb2= newTB("");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert((testtb->nlines) == 0);
	releaseTB(testtb);

	//Merge an empty string and a new line string
	testtb = newTB("");
	testtb2= newTB("\n\n\n\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 4);
	releaseTB(testtb);

	//Merge an empty string and a full string
	testtb = newTB("");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Linen") == 0);
	assert((testtb->nlines) == 11);
	releaseTB(testtb);

	//Merge an empty string and one large string
	testtb = newTB("");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 1);
	releaseTB(testtb);

	//Merge a new line and an empty string
	testtb = newTB("\n");
	testtb2= newTB("");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 1);
	releaseTB(testtb);

	//Merge multiple empty lines
	testtb = newTB("\n");
	testtb2 = newTB("\n\n\n\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 5);
	releaseTB(testtb);

	//Merge a new line and 10 lines
	testtb = newTB("\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 12);
	releaseTB(testtb);

	//Merge new string and one large string
	testtb = newTB("\n");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 2);
	releaseTB(testtb);


	//Merge new lines and an empty string
	testtb = newTB("\n\n\n");
	testtb2= newTB("");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 3);
	releaseTB(testtb);

	//Merge multiple new lines
	testtb = newTB("\n\n\n");
	testtb2= newTB("\n\n\n\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 7);
	releaseTB(testtb);

	//Merge multiple new lines and 10 lines
	testtb = newTB("\n\n\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "Linen+10") == 0);
	assert((testtb->nlines) == 14);
	releaseTB(testtb);

	//Merge multiple new lines and one large
	testtb = newTB("\n\n\n");
	testtb2 = newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 4);
	releaseTB(testtb);

	//normal conditions

	//Merge 1 line and an emptry string
	testtb = newTB("Line01\n");
	testtb2= newTB("");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Line01") == 0);
	assert((testtb->nlines) == 1);
	releaseTB(testtb);

	//Merge 1 line and multiple new lines
	testtb = newTB("Line01\n");
	testtb2= newTB("\n\n\n\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	assert((testtb->nlines) == 5);
	releaseTB(testtb);

	//Merge 1 line and 20 lines
	testtb = newTB("Line01\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Linen+10") == 0);
	assert((testtb->nlines) == 12);
	releaseTB(testtb);

	//Merge 1 line and a large line
	testtb = newTB("Line01\n");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 2);
	releaseTB(testtb);

	//Merge 20 line and an empty string
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert((testtb->nlines) == 20);
	releaseTB(testtb);

	//Merge 20 line and multiple newlines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("\n\n\n\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	assert((testtb->nlines) == 24);
	releaseTB(testtb);

	//Merge 20 line and 10 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Linen+10") == 0);
	assert((testtb->nlines) == 31);
	releaseTB(testtb);

	//Merge 20 line and a large line
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	mergeTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 21);
	releaseTB(testtb);

	//Merge at start
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, 0 ,testtb2);
	assert(strcmp(testtb->first->line, "Linen") == 0);
	assert(strcmp(testtb->last->line, "Line20") == 0);
	assert((testtb->nlines) == 31);
	releaseTB(testtb);

	//Merge in middle
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	mergeTB(testtb, 2 ,testtb2);
	assert(strcmp(testtb->first->next->next->line, "Linen") == 0);
	assert(strcmp(testtb->last->line, "Line20") == 0);
	assert((testtb->nlines) == 31);
	releaseTB(testtb);

	//Tests for paste TB:

	//Boundary Condition tests:

	//Pasting two empty strings
	testtb = newTB("");
	testtb2= newTB("");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert((testtb->nlines) == 0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting an empty string and newlines
	testtb = newTB("");
	testtb2= newTB("\n\n\n\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 4);
	assert((testtb2->nlines) == 4);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting an empty string and 10 lines
	testtb = newTB("");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Linen") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert((testtb->nlines) == 11);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting an empty string and a large string
	testtb = newTB("");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 1);
	assert((testtb2->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting a new line and empty string
	testtb = newTB("\n");
	testtb2= newTB("");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting a new line and multiple new lines
	testtb = newTB("\n");
	testtb2= newTB("\n\n\n\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb2->first->line, "") == 0);
	assert((testtb->nlines) == 5);
	assert((testtb2->nlines) == 4);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting a new line and 10 lines
	testtb = newTB("\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert((testtb->nlines) == 12);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting a new line and large string
	testtb = newTB("\n");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 2);
	assert((testtb2->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting multiple new line and empty string
	testtb = newTB("\n\n\n");
	testtb2= newTB("");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert((testtb->nlines) == 3);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting multiple new lines
	testtb = newTB("\n\n\n");
	testtb2= newTB("\n\n\n\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb2->first->line, "") == 0);
	assert((testtb->nlines) == 7);
	assert((testtb2->nlines) == 4);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting multiple new lines and 10 lines
	testtb = newTB("\n\n\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "Linen+10") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert(strcmp(testtb2->last->line, "Linen+10") == 0);
	assert((testtb->nlines) == 14);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting mutiple new lines and a large line
	testtb = newTB("\n\n\n");
	testtb2 = newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 4);
	assert((testtb2->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Normal conditions

	//Pasting 1 line and empty line
	testtb = newTB("Line01\n");
	testtb2= newTB("");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Line01") == 0);
	assert((testtb->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 1 line and multiple new lines
	testtb = newTB("Line01\n");
	testtb2= newTB("\n\n\n\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	assert(strcmp(testtb2->first->line, "") == 0);
	assert((testtb->nlines) == 5);
	assert((testtb2->nlines) == 4);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 1 line and 10 lines
	testtb = newTB("Line01\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Linen+10") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert((testtb->nlines) == 12);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 1 line and large line
	testtb = newTB("Line01\n");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 2);
	assert((testtb2->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 20 lines and empty string
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert((testtb->nlines) == 20);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 20 line and multiple new lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("\n\n\n\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "") == 0);
	assert(strcmp(testtb2->first->line, "") == 0);
	assert(strcmp(testtb2->last->line, "") == 0);
	assert((testtb->nlines) == 24);
	assert((testtb2->nlines) == 4);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 20 line and 10 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Linen+10") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert(strcmp(testtb2->last->line,"Linen+10")==0);
	assert((testtb->nlines) == 31);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Pasting 20 line and large line
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.\n");
	pasteTB(testtb, testtb->nlines ,testtb2);
	assert(strcmp(testtb->first->line, "Line01") == 0);
	assert(strcmp(testtb->last->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert(strcmp(testtb2->first->line, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.") == 0);
	assert((testtb->nlines) == 21);
	assert((testtb2->nlines) == 1);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Paste at start
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, 0 ,testtb2);
	assert(strcmp(testtb->first->line, "Linen") == 0);
	assert(strcmp(testtb->last->line, "Line20") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert(strcmp(testtb2->last->line, "Linen+10")==0);
	assert((testtb->nlines) == 31);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Paste in middle
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2= newTB("Linen\nLinen+1\nnLinen+2\nLinen+3\nLinen+4\nLinen+5\nLinen+6\nLinen+7\nLinen+8\nLinen+9\nLinen+10\n");
	pasteTB(testtb, 2 ,testtb2);
	assert(strcmp(testtb->first->next->next->line, "Linen") == 0);
	assert(strcmp(testtb->last->line, "Line20") == 0);
	assert(strcmp(testtb2->first->line, "Linen") == 0);
	assert(strcmp(testtb2->last->line, "Linen+10")==0);
	assert((testtb->nlines) == 31);
	assert((testtb2->nlines) == 11);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Tests for cutTB:

	//Boundary Condition tests:

	//Cuttb a new line character
	testtb = newTB("\n");
	testtb2 = cutTB(testtb, 0, 0);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 0);
	assert(strcmp(testtb2->last->line,"")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut multiple new line character
	testtb = newTB("\n\n\n");
	testtb2 = cutTB(testtb, 0, 0);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 2);
	assert(strcmp(testtb2->last->line,"")==0);
	assert(strcmp(testtb->last->line,"")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut multiple new line characters at end
	testtb = newTB("\n\n\n");
	testtb2 = cutTB(testtb, 2, 2);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 2);
	assert(strcmp(testtb2->last->line,"")==0);
	assert(strcmp(testtb->last->line,"")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut all new line characters
	testtb = newTB("\n\n\n");
	testtb2 = cutTB(testtb, 0, 2);
	assert(testtb2->nlines == 3);
	assert(testtb->nlines == 0);
	assert(strcmp(testtb2->last->line,"")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut new line character in middle
	testtb = newTB("\n\n\n");
	testtb2 = cutTB(testtb, 1, 1);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 2);
	assert(strcmp(testtb2->last->line,"")==0);
	assert(strcmp(testtb->last->line,"")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut 1 line at start
	testtb = newTB("Line01\n");
	testtb2 = cutTB(testtb, 0, 0);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 0);
	assert(strcmp(testtb2->last->line,"Line01")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//cut 20 lines at start
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2 = cutTB(testtb, 0, 0);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 19);
	assert(strcmp(testtb2->last->line,"Line01")==0);
	assert(strcmp(testtb->last->line,"Line20")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut 20 lines at end
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2 = cutTB(testtb, 19, 19);
	assert(testtb2->nlines == 1);
	assert(testtb->nlines == 19);
	assert(strcmp(testtb2->last->line,"Line20")==0);
	assert(strcmp(testtb->last->line,"Line19")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut all 20 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2 = cutTB(testtb, 0, 19);
	assert(testtb2->nlines == 20);
	assert(testtb->nlines == 0);
	assert(strcmp(testtb2->last->line,"Line20")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut 20 lines from start to middle
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2 = cutTB(testtb, 0, 10);
	assert(testtb2->nlines == 11);
	assert(testtb->nlines == 9);
	assert(strcmp(testtb2->last->line,"Line11")==0);
	assert(strcmp(testtb->last->line,"Line20")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut 20 lines from middle to end
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2 = cutTB(testtb, 10, 19);
	assert(testtb2->nlines == 10);
	assert(testtb->nlines == 10);
	assert(strcmp(testtb2->last->line,"Line20")==0);
	assert(strcmp(testtb2->first->line,"Line11")==0);
	assert(strcmp(testtb->last->line,"Line010")==0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Cut 20 lines from middle 
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	testtb2 = cutTB(testtb, 5, 8);
	assert(testtb->nlines == 16);
	assert(strcmp(testtb->last->line,"Line20")==0);
	assert(strcmp(testtb2->last->line, "Line09") == 0);
	releaseTB(testtb);
	releaseTB(testtb2);

	//Tests for deleteTB:

	//Boundary Condition tests:

	//delete new lines
	testtb = newTB("\n");
	deleteTB(testtb, 0, 0);
	assert(testtb->nlines == 0);
	releaseTB(testtb);

	//delete new lines at start
	testtb = newTB("\n\n\n");
	deleteTB(testtb, 0, 0);
	assert(testtb->nlines == 2);
	assert(strcmp(testtb->last->line,"")==0);
	releaseTB(testtb);

	//delete new lines at end
	testtb = newTB("\n\n\n");
	deleteTB(testtb, 2, 2);
	assert(testtb->nlines == 2);
	assert(strcmp(testtb->last->line,"")==0);
	releaseTB(testtb);

	//delete new lines from start to end
	testtb = newTB("\n\n\n");
	deleteTB(testtb, 0, 2);
	assert(testtb->nlines == 0);
	releaseTB(testtb);

	//delete new lines at middle
	testtb = newTB("\n\n\n");
	deleteTB(testtb, 1, 1);
	assert(testtb->nlines == 2);
	assert(strcmp(testtb->last->line,"")==0);
	releaseTB(testtb);

	//delete 1 line at start
	testtb = newTB("Line01\n");
	deleteTB(testtb, 0, 0);
	assert(testtb->nlines == 0);
	releaseTB(testtb);

	//delete 20 lines at start
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	deleteTB(testtb, 0, 0);
	assert(testtb->nlines == 19);
	assert(strcmp(testtb->last->line,"Line20")==0);
	releaseTB(testtb);

	//delete 20 lines at end
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	deleteTB(testtb, 19, 19);
	assert(testtb->nlines == 19);
	assert(strcmp(testtb->last->line,"Line19")==0);
	releaseTB(testtb);

	//delete 20 lines for whole text
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	deleteTB(testtb, 0, 19);
	assert(testtb->nlines == 0);
	releaseTB(testtb);

	//delete 20 lines from start to end
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	deleteTB(testtb, 0, 10);
	assert(testtb->nlines == 9);
	assert(strcmp(testtb->last->line,"Line20")==0);
	releaseTB(testtb);

	//delete 20 lines from middle to end
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	deleteTB(testtb, 10, 19);
	assert(testtb->nlines == 10);
	assert(strcmp(testtb->last->line,"Line010")==0);
	releaseTB(testtb);

	//delete 20 lines at middle
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\nLine11\nLine12\nLine13\nLine14\nLine15\nLine16\nLine17\nLine18\nLine19\nLine20\n");
	deleteTB(testtb, 5, 8);
	assert(testtb->nlines == 16);
	assert(strcmp(testtb->last->line,"Line20")==0);
	releaseTB(testtb);

	//Tests for searchTB

	//Search for nothing in an empty string
	testtb = newTB("");
	Match testmatch = searchTB(testtb, "");
	assert(testmatch == NULL);
	releaseTB(testtb);

	//Search for a small string in an empty string
	testtb = newTB("");
	testmatch = searchTB(testtb, "Hello");
	assert(testmatch == NULL);
	releaseTB(testtb);

	//Search for a large string in an empty string
	testtb = newTB("");
	testmatch = searchTB(testtb, "Long long long long long long long long string");
	assert(testmatch == NULL);
	releaseTB(testtb);

	//Search for an empty string in 10 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\n");
	testmatch = searchTB(testtb, "");
	assert(testmatch == NULL);
	releaseTB(testtb);

	//Search for a small string in 10 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\n");
	testmatch = searchTB(testtb,"Line");
	assert(testmatch->next->next->next->next->next->next->next->next->next->lineNumber == 10);
	assert(testmatch->next->lineNumber == 2);
	assert(testmatch->next->charIndex == 0);
	releaseTB(testtb);

	//Search for a specific string in 10 lines
	testtb = newTB("Line01\nLine02\nLine03\nLine04\nLine05\nLine06\nLine07\nLine08\nLine09\nLine010\n");
	testmatch = searchTB(testtb, "Line06");
	assert(testmatch->next== NULL);
	releaseTB(testtb);

	//Search for large string in large string
	testtb = newTB("Lorem ipsum dolor sit amet, Lorem ipsum dolor sit amet adipiscing elit. Aenean commodo ligula eget dolor. Aenean Lorem.\n");
	testmatch = searchTB(testtb, "Lorem ipsum dolor sit amet, Lorem ipsum dolor sit amet adipiscing elit. Aenean commodo ligula eget dolor. Aenean Lorem.");
	assert(testmatch->next== NULL);
	assert(testmatch->lineNumber == 1);
	assert(testmatch->charIndex == 0);
	releaseTB(testtb);

	//Search at the start, search in the middle and search at the end;
	testtb = newTB("Lorem ipsum dolor sit amet, Lorem ipsum dolor sit amet adipiscing elit. Aenean commodo ligula eget dolor. Aenean Lorem.\n");
	testmatch = searchTB(testtb, "Lorem");
	assert(testmatch->next->next->next== NULL);
	assert(testmatch->next->lineNumber == 1);
	assert(testmatch->next->charIndex == 28);
	assert(testmatch->next->next->lineNumber == 1);
	assert(testmatch->next->next->charIndex == 113);
	releaseTB(testtb);

	//Search in the middle of two other words
	testtb = newTB("asdfasdLoremfasdfsda\n");
	testmatch = searchTB(testtb, "Lorem");
	assert(testmatch->next== NULL);
	assert(testmatch->charIndex == 7);
	releaseTB(testtb);

	//Search across multiple lines
	testtb = newTB("asdfasdLoremfasdfsda\nHey there Lorem me\n Dont play Lorem games with me\n");
	testmatch = searchTB(testtb, "Lorem");
	assert(testmatch->next->next->next == NULL);
	assert(testmatch->charIndex == 7);
	releaseTB(testtb);

	//Test greedy search

	//Search in the middle of two other words
	testtb = newTB("abababwhatwhat\n");
	testmatch = searchTB(testtb, "abab");
	assert(testmatch->next== NULL);
	assert(testmatch->charIndex == 0);
	releaseTB(testtb);

	//Test correct search
	testtb = newTB("irodsdfafniro dasewefd iro\n");
	testmatch = searchTB(testtb, "iron");
	assert(testmatch== NULL);
	releaseTB(testtb);

	//Tests for formrichtext


	//We are losing something here

	//Formrichtext in empty string
	testtb = newTB("");
	formRichText(testtb);
	assert(testtb->nlines == 0);
	releaseTB(testtb);


	//Formrichtext in small string
	testtb = newTB("** __\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "** __") == 0);
	releaseTB(testtb);

	//Formrichtext in small string
	testtb = newTB("#\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "#") == 0);
	releaseTB(testtb);

	//Formrichtext in multiple lines
	testtb = newTB("*a sd* _b_ \n*c* _d_\n _e_ *f*\n _g_\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<b>a sd</b> <i>b</i> ") == 0);
	releaseTB(testtb);

	//We lose space


	//Formrichtext for greedy
	testtb = newTB("*a*a*\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<b>a</b>a*") == 0);
	releaseTB(testtb);

	//Formrichtext for greedy
	testtb = newTB("_a_a_\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<i>a</i>a_") == 0);
	releaseTB(testtb);

	//Formrichtext for greedy
	testtb = newTB("##a\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<h1>#a</h1>") == 0);
	releaseTB(testtb);

	//Formrichtext for opening
	testtb = newTB("*aa\n_aa\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "*aa") == 0);
	assert(strcmp(testtb->last->line, "_aa") == 0);
	releaseTB(testtb);

	//Formrichtext for nested
	testtb = newTB("_a*a*\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "_a<b>a</b>") == 0);
	releaseTB(testtb);

	//Formrichtext for nested
	testtb = newTB("*a_a_\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "*a<i>a</i>") == 0);
	releaseTB(testtb);

	//Formrichtext for nested
	testtb = newTB("#a_a_\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<h1>a_a_</h1>") == 0);
		releaseTB(testtb);

	//Formrichtext for hash at start
	testtb = newTB("#aa\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<h1>aa</h1>") == 0);
	releaseTB(testtb);

	//Formrichtext for has at end
	testtb = newTB("a#a_\nab#cdefg\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "a#a_") == 0);
	releaseTB(testtb);

	//Formrichtext for nested 
	testtb = newTB("*abcd_e_*\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<b>abcd_e_</b>") == 0);
	releaseTB(testtb);

	//Formrichtext for nested
	testtb = newTB("_abcd*e*__\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "<i>abcd*e*</i>_") == 0);
	releaseTB(testtb);

	//Formrichtext for middle 
	testtb = newTB("***********hello*************\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line, "**********<b>hello</b>************") == 0);
	releaseTB(testtb);

	//Formrichtext for middle
	testtb = newTB("______________world______________\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line,"_____________<i>world</i>_____________") == 0);
	releaseTB(testtb);


	//Formrichtext for adjacent
	testtb = newTB("*_*_*_*_*_*_*_*\n");
	formRichText(testtb);
	assert(strcmp(testtb->first->line,"<b>_</b><i>*</i><b>_</b><i>*</i><b>_</b>") == 0);
	releaseTB(testtb);

	printf("success!\n");
}

