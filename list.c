#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ustr.h"
#include "list.h"

/*
Returns an empty list of initial_capacity
*/
List new_list(int32_t initial_capacity) {
    List list;
    list.size = 0;
    list.capacity = initial_capacity;
    list.data = malloc(initial_capacity * sizeof(struct UStr));
    return list;
}

/*
Initializes an list of length size with elements from array
*/
List new_list_from_array(struct UStr* array, int32_t size) {
    List list = new_list(size);
    for (int32_t i = 0; i < size; i++) {
        list.data[i] = array[i];
    }
    list.size = size;
    return list;
}

/*
Given a list of strings and a separator string, returns a single string
containing all the strings in list joined by the separator.
*/
UStr join(List* list, UStr separator) {
    if (list->size == 0) {
        return new_ustr("");
    }

    int total_bytes = 0;
    for (int i = 0; i < list->size; i++) {
        total_bytes += list->data[i].bytes;
        if (i < list->size - 1) {
            total_bytes += separator.bytes;
        }
    }

    char* joined = malloc(total_bytes + 1);
    int offset = 0;

    for (int i = 0; i < list->size; i++) {
        memcpy(joined + offset, list->data[i].contents, list->data[i].bytes);
        offset += list->data[i].bytes;

        if (i < list->size - 1) {
            memcpy(joined + offset, separator.contents, separator.bytes);
            offset += separator.bytes;
        }
    }

    joined[offset] = '\0';
    UStr result = new_ustr(joined);
    free(joined);
    return result;
}

/*
Inserts string s into list at index s, shifting elements to the right.
Expands the list's capacity if necessary (double the capacity, or set to 1 if 0).

Returns 1 on success, 0 if the index is invalid (out of bounds).
*/
int8_t insert(List* list, UStr s, int32_t index) {
    if (index < 0 || index > list->size) {
        return 0;
    }

    if (list->size >= list->capacity) {
        int new_capacity = list->capacity == 0 ? 1 : list->capacity * 2;
        list->data = realloc(list->data, new_capacity * sizeof(UStr));
        list->capacity = new_capacity;
    }

    for (int i = list->size; i > index; i--) {
        list->data[i] = list->data[i - 1];
    }

    list->data[index] = s;
    list->size++;
    return 1;
}

/*
Removes the element at the given index and shifts all subsequent elements left.

Returns 1 on success, 0 if the index is invalid (out of bounds).
*/
int8_t listRemoveAt(List* list, int32_t index) {
    if (index < 0 || index >= list->size) {
        return 0;
    }

    free_ustr(list->data[index]);

    for (int i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }

    list->size--;
    return 1;
}

/*
Splits the given string s into substrings separated by the given delimiter string.

Returns a List of String objects, each containing a segment between delimiters.

If the delimiter is the empty string, return a list containing the original
string as a single element.

If the input string ends with the delimiter, include an empty string at the
end of the result.

Note that the delimiter could be of a length of more than 1 character
*/
List split(UStr s, UStr separator) {
    if (separator.bytes == 0) {
        List list = new_list(1);
        list.data[0] = new_ustr(s.contents);
        list.size = 1;
        return list;
    }

    List result = new_list(4);
    char* str = s.contents;
    int str_len = s.bytes;
    int sep_len = separator.bytes;
    int start = 0;

    for (int i = 0; i <= str_len - sep_len; ) {
        if (memcmp(str + i, separator.contents, sep_len) == 0) {
            int segment_len = i - start;
            char* segment = malloc(segment_len + 1);
            memcpy(segment, str + start, segment_len);
            segment[segment_len] = '\0';
            insert(&result, new_ustr(segment), result.size);
            free(segment);
            i += sep_len;
            start = i;
        } else {
            i++;
        }
    }

    int segment_len = str_len - start;
    char* segment = malloc(segment_len + 1);
    memcpy(segment, str + start, segment_len);
    segment[segment_len] = '\0';
    insert(&result, new_ustr(segment), result.size);
    free(segment);

    return result;
}

