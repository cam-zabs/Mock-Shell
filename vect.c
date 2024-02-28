/**
 * Vector implementation.
 *
 * - Implement each of the functions to create a working growable array (vector).
 * - Do not change any of the structs
 * - When submitting, You should not have any 'printf' statements in your vector
 *   functions.
 *
 * IMPORTANT: The initial capacity and the vector's growth factor should be
 * expressed in terms of the configuration constants in vect.h
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"

/** Main data structure for the vector. */
struct vect {
    char **data;             /* Array containing the actual data. */
    unsigned int size;       /* Number of items currently in the vector. */
    unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
vect_t *vect_new() {
    // Allocate memory for the vector
    vect_t *v = (vect_t*)malloc(sizeof(vect_t));

    // If memory could not be allocated, return NULL
    if (v == NULL) {
        return NULL;
    }

    // Allocate memory for the data of the vector
    v->data = (char**)malloc(VECT_INITIAL_CAPACITY * sizeof(char*));

    // If memory could not be allocated, free the vector and return NULL
    if (v->data == NULL) {
        free(v);
        return NULL;
    }

    // Initialize the size of the vector to 0
    v->size = 0;

    // Initialize the capacity of the vector to the initial capacity constant
    v->capacity = VECT_INITIAL_CAPACITY;

    // Return the vector
    return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {
    // Assert the vector is not NULL
    assert(v != NULL);

    // Assert the vector's data array is not NULL
    assert(v->data != NULL);

    // Free the data inside the data array from memory
    for (unsigned int i = 0; i < v->size; i++) {
        free(v->data[i]);
    }

    // Free the data array itself from memory
    free(v->data);

    // Free the vector from memory
    free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
    assert(v != NULL);
    assert(idx < v->size);

    // Return the element at the given index
    return v->data[idx];
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
    assert(v != NULL);
    assert(idx < v->size);

    // Create a pointer to the element at the given index
    const char *element = vect_get(v, idx);

    // Allocate memory for a copy of the element
    char *copy = (char*)malloc(strlen(element) + 1); // The length of the element + 1 (to account for the null terminator)

    // If memory could not be allocated, return NULL
    if (copy == NULL) {
        return NULL;
    }

    // Otherwise, make a copy of the element
    strcpy(copy, element);

    // Return the copy
    return copy;
}

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
    assert(v != NULL);
    assert(idx < v->size);
    assert(elt != NULL); // Assert that the given element is not NULL

    // If there is already an element at the given index, free it from memory
    if (v->data[idx] != NULL) {
        free(v->data[idx]);
    }

    // Allocate memory for the given element
    char *element = (char*)malloc(strlen(elt) + 1); // The length of the element + 1 (to account for the null terminator)
    // If memory could not be allocated, return
    if (element == NULL) {
        return;
    }

    // Otherwise, copy the given element to the allocated memory
    strcpy(element, elt);

    // Set the element at the given index
    v->data[idx] = element;
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
    assert(v != NULL);

    // If the vector is already full, resize it
    if (v->size >= v->capacity) {
        unsigned int updatedCapacity = v->capacity * VECT_GROWTH_FACTOR;

        // Make sure the updated capacity is not greater than the maximum capacity (return if it is)
        if (updatedCapacity > VECT_MAX_CAPACITY) {
            return;
        }

        // Otherwise, reallocate memory for the resized data array
        char** updatedData = (char**)realloc(v->data, updatedCapacity * sizeof(char*));

        // If memory could not be allocated, return
        if (updatedData == NULL) {
            return;
        }

        // Otherwise, initialize the data and capacity of the new array to the updated values
        v->data = updatedData;
        v->capacity = updatedCapacity;
    }

    // Allocate memory for the given element
    char *element = (char*)malloc(strlen(elt) + 1); // The length of the element + 1 (to account for the null terminator)

    // If memory could not be allocated, return
    if (element == NULL) {
        return;
    }

    // Otherwise, copy the given element to the allocated memory
    strcpy(element, elt);

    // Set the element at the given index
    v->data[v->size] = element;

    // Update the size of the vector
    v->size++;
}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
    assert(v != NULL);

    // Free the memory of the last element in the given vector
    free(v->data[v->size - 1]);

    // Update the size of the vector
    v->size--;
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
    assert(v != NULL);

    // Return the number of items currently in the vector (In other words, the size of the vector)
    return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
    assert(v != NULL);

    // Return the maximum number of items the vector can hold before it has to grow (In other words, the capacity of the vector)
    return v->capacity;
}
