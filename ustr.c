#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ustr.h"
#include "pset1.h"

/*
Initializes a new UStr with contents
*/
UStr new_ustr(char* contents) {
	int32_t bytes = strlen(contents);
	int32_t codepoints = utf8_strlen(contents);
	uint8_t all_ascii = bytes == codepoints;
	char* contents_copy = malloc(bytes + 1);
	strcpy(contents_copy, contents);
	UStr s = {
		codepoints,
		bytes,
		all_ascii,
		contents_copy
	};
	return s;
}

/*
Returns length of string
*/
int32_t len(UStr s) {
	return s.codepoints;
}

/*
Returns a substring of str, starting at index start (inclusive) 
and ending at index end (exclusive).

Returns an empty string on invalid range.
*/
UStr substring(UStr s, int32_t start, int32_t end) {
	// TODO: implement this
	if (start < 0 || end > s.codepoints || start > end){
		return new_ustr("");
	}
	int byte_start = bi_of_cpi(s.contents, start);
	int byte_end = bi_of_cpi(s.contents,end);
	int new_len = byte_end - byte_start;

	char* sub = malloc(new_len + 1);
	memcpy(sub, s.contents + byte_start, new_len);
	sub[new_len] = '\0';

	UStr result = new_ustr(sub);
	free(sub);
	return result;
}

/*
Given 2 strings s1 and s2, returns a string that is the result of 
concatenating s1 and s2. 
*/
UStr concat(UStr s1, UStr s2) {
	// TODO: implement this
	int32_t total_bytes = s1.bytes + s2.bytes;
	char* combined = malloc(total_bytes +1);
	if (combined == NULL) {
		fprintf(stderr, "Memory allocation failed in concat()\n");
		exit(1);
	}
	memcpy(combined, s1.contents, s1.bytes);
	memcpy(combined + s1.bytes, s2.contents, s2.bytes);

	combined[total_bytes] = '\0';

	int32_t total_codepoints = utf8_strlen(combined);
	uint8_t all_ascii = s1.is_ascii && s2.is_ascii;

	UStr result = {
		.codepoints = total_codepoints,
		.bytes = total_bytes,
		.is_ascii = all_ascii,
		.contents = combined
	};
	return result;
}

/*
Given a string s and an index, return a string with the character at index 
removed from the original string. 

Returns the original string if index is out of bounds.
*/
UStr removeAt(UStr s, int32_t index) {
	if (index < 0 || index >= s.codepoints) {
        return new_ustr(s.contents);
    }
    int byte_index = 0;
    int current_cp = 0;
    while (current_cp < index) {
        byte_index += utf8_codepoint_size(s.contents[byte_index]);
        current_cp++;
    }
    int remove_start = byte_index;
    int remove_size = utf8_codepoint_size(s.contents[remove_start]);
    int new_bytes = s.bytes - remove_size;
    char* result = malloc(new_bytes + 1); 
    memcpy(result, s.contents, remove_start);
    memcpy(result + remove_start,
           s.contents + remove_start + remove_size,
           s.bytes - remove_start - remove_size);
    result[new_bytes] = '\0'; 
    UStr final = new_ustr(result);
    free(result);
    return final;
}

/*
Given a string s, return s reversed. 

Example: reverse("apples🍎 and bananas🍌") = "🍌sananab dna 🍎selppa")
*/
UStr reverse(UStr s) {

	if (s.codepoints <= 1) {
        return new_ustr(s.contents);
    }
    char* reversed = malloc(s.bytes + 1);
    int write_index = 0;
    int i = s.bytes - 1;
    while (i >= 0) {
        int cp_start = i;
        while (cp_start > 0 && (s.contents[cp_start] & 0xC0) == 0x80) {
            cp_start--;
        }
        int cp_size = utf8_codepoint_size(s.contents[cp_start]);
        memcpy(reversed + write_index, s.contents + cp_start, cp_size);
        write_index += cp_size;

        i = cp_start - 1;
    }
    reversed[write_index] = '\0';
    UStr result = new_ustr(reversed);
    free(reversed);
    return result;
}


void print_ustr(UStr s) {
	printf("%s [codepoints: %d | bytes: %d]", s.contents, s.codepoints, s.bytes);
}

void free_ustr(UStr s) {
	if (s.contents != NULL) {
		free(s.contents);
		s.contents = NULL;
	}
}

