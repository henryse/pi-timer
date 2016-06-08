/*
 *    strmap version 2.0.1
 *
 *    ANSI C hash table for strings.
 *
 *	  Version history:
 *	  1.0.0 - initial release
 *	  2.0.0 - changed function prefix from strmap to sm to ensure
 *	      ANSI C compatibility
 *	  2.0.1 - improved documentation
 *    2.0.2 - henryse - integrated into Dino project
 *    2.0.3 - henryse - add support for comma delimited list.
 *
 *    strmap.c
 *
 *    Copyright (c) 2009, 2011, 2013 Per Ola Kristensson.
 *
 *    Per Ola Kristensson <pok21@cam.ac.uk>
 *    Inference Group, Department of Physics
 *    University of Cambridge
 *    Cavendish Laboratory
 *    JJ Thomson Avenue
 *    CB3 0HE Cambridge
 *    United Kingdom
 *
 *    strmap is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    strmap is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with strmap.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#define _GNU_SOURCE

#include <stdio.h>
#include "pi_strmap.h"
#include "pi_utils.h"

typedef struct pair_struct {
    char *key;
    char *value;
} pair_t;

typedef struct bucket_struct {
    unsigned int count;
    pair_t *pairs;
} bucket_t;

struct str_map_struct {
    unsigned int count;
    bucket_t *buckets;
};

static pair_t *get_pair(bucket_t *bucket, const char *key);

static unsigned long hash(const char *str);

str_map_t *pi_strmap_new(unsigned int capacity) {
    str_map_t *map = memory_alloc(sizeof(str_map_t));

    if (NULL == map) {
        return NULL;
    }

    map->count = capacity;
    map->buckets = memory_alloc(map->count * sizeof(bucket_t));

    if (NULL == map->buckets) {
        memory_free(map);
        return NULL;
    }

    memset(map->buckets, 0, map->count * sizeof(bucket_t));
    return map;
}

void pi_strmap_delete(str_map_t *map) {
    if (NULL == map) {
        return;
    }
    unsigned int n = map->count;
    bucket_t *bucket = map->buckets;
    unsigned int i = 0;
    while (i < n) {
        unsigned int m = bucket->count;
        pair_t *pair = bucket->pairs;
        unsigned int j = 0;
        while (j < m) {
            memory_free(pair->key);
            memory_free(pair->value);
            pair++;
            j++;
        }
        memory_free(bucket->pairs);
        bucket++;
        i++;
    }
    memory_free(map->buckets);
    memory_free(map);
}

const char *pi_strmap_get_value(const str_map_t *map, const char *key) {
    if (NULL == map || NULL == key) {
        return "";
    }

    unsigned int index = (unsigned int) (hash(key) % map->count);
    bucket_t *bucket = &(map->buckets[index]);
    pair_t *pair = get_pair(bucket, key);
    if (NULL == pair) {
        return "";
    }

    return pair->value;
}

size_t pi_strmap_get(const str_map_t *map, const char *key, char *out_buf, unsigned int n_out_buf) {
    if (NULL == map || NULL == key) {
        return 0;
    }

    unsigned int index = (unsigned int) (hash(key) % map->count);
    bucket_t *bucket = &(map->buckets[index]);
    pair_t *pair = get_pair(bucket, key);
    if (NULL == pair) {
        return 0;
    }
    if (NULL == out_buf && n_out_buf == 0) {
        return strlen(pair->value) + 1;
    }
    if (NULL == out_buf) {
        return 0;
    }
    if (strlen(pair->value) >= n_out_buf) {
        return 0;
    }
    strcpy(out_buf, pair->value);

    return 1;
}

bool pi_strmap_exists(const str_map_t *map, const char *key) {
    if (NULL == map || NULL == key) {
        return false;
    }

    unsigned int index = (unsigned int) (hash(key) % map->count);
    bucket_t *bucket = &(map->buckets[index]);
    pair_t *pair = get_pair(bucket, key);

    if (NULL == pair) {
        return false;
    }

    return true;
}

bool pi_strmap_add(str_map_t *map, const char *key, const char *value) {
    if (NULL == map || NULL == key || NULL == value) {
        return false;
    }

    size_t length = strlen(value);

    // If it an empty string, just ignore it.
    //
    if (0 == length) {
        return true;
    }

    // Create a local copy
    //
    char *local_value = alloca(length + 1);
    strcpy(local_value, value);

    // Trim out the "white space"
    //
    local_value = trim_whitespace(local_value);

    // Is there anything left?
    //
    if (0 == strlen(local_value)) {
        return true;
    }

    // Check to see if it exists.
    //
    if (pi_strmap_exists(map, key)) {
        // Get the current value
        //
        char *new_value = NULL;

        asprintf(&new_value, "%s, %s", pi_strmap_get_value(map, key), local_value);

        bool result = false;
        if (new_value) {
            result = pi_strmap_put(map, key, new_value);
            free(new_value);
        }

        return result;
    }

    return pi_strmap_put(map, key, value);
}

bool pi_strmap_put(str_map_t *map, const char *key, const char *value) {
    if (NULL == map || NULL == key || NULL == value) {
        return false;
    }

    size_t key_len = strlen(key);
    size_t value_len = strlen(value);
    // Get a pointer to the bucket the key string hashes to
    //
    size_t index = hash(key) % map->count;
    bucket_t *bucket = &(map->buckets[index]);

    // Check if we can handle insertion by simply replacing
    // an existing value in a key-value pair in the bucket.
    //
    pair_t *pair = NULL;
    if (NULL != (pair = get_pair(bucket, key))) {
        // The bucket contains a pair that matches the provided key,
        //change the value for that pair to the new value.
        //
        if (strlen(pair->value) < value_len) {
            // If the new value is larger than the old value, re-allocate
            // space for the new larger value.
            //
            char *tmp_value = memory_realloc(pair->value, (value_len + 1) * sizeof(char));
            if (NULL == tmp_value) {
                return false;
            }
            pair->value = tmp_value;
        }
        /* Copy the new value into the pair that matches the key */
        strcpy(pair->value, value);
        return true;
    }

    // Allocate space for a new key and value
    //
    char *new_key = memory_alloc((key_len + 1) * sizeof(char));
    if (NULL == new_key) {
        return false;
    }

    char *new_value = memory_alloc((value_len + 1) * sizeof(char));
    if (NULL == new_value) {
        memory_free(new_key);
        return false;
    }

    // Create a key-value pair
    //
    if (bucket->count == 0) {
        // The bucket is empty, lazily allocate space for a single
        // key-value pair.
        //
        bucket->pairs = memory_alloc(sizeof(pair_t));
        if (NULL == bucket->pairs) {
            memory_free(new_key);
            memory_free(new_value);
            return false;
        }
        bucket->count = 1;
    }
    else {
        // The bucket wasn't empty but no pair existed that matches the provided
        // key, so create a new key-value pair.
        //
        pair_t *tmp_pairs = memory_realloc(bucket->pairs, (bucket->count + 1) * sizeof(pair_t));
        if (NULL == tmp_pairs) {
            memory_free(new_key);
            memory_free(new_value);
            return false;
        }
        bucket->pairs = tmp_pairs;
        bucket->count++;
    }
    // Get the last pair in the chain for the bucket
    //
    pair = &(bucket->pairs[bucket->count - 1]);
    pair->key = new_key;
    pair->value = new_value;

    // Copy the key and its value into the key-value pair
    //
    strcpy(pair->key, key);
    strcpy(pair->value, value);
    return true;
}

int pi_strmap_get_count(const str_map_t *map) {
    if (NULL == map) {
        return 0;
    }

    bucket_t *bucket = map->buckets;
    unsigned int n = map->count;
    unsigned int i = 0;
    unsigned int count = 0;

    while (i < n) {
        pair_t *pair = bucket->pairs;
        unsigned int m = bucket->count;
        unsigned int j = 0;
        while (j < m) {
            count++;
            pair++;
            j++;
        }
        bucket++;
        i++;
    }
    return count;
}

bool pi_strmap_enum(const str_map_t *map, pi_strmap_enum_func enum_func, const void *obj) {
    if (NULL == map) {
        return false;
    }

    if (NULL == enum_func) {
        return true;
    }
    bucket_t *bucket = map->buckets;
    unsigned int n = map->count;
    unsigned int i = 0;
    while (i < n) {
        pair_t *pair = bucket->pairs;
        unsigned int m = bucket->count;
        unsigned int j = 0;
        while (j < m) {
            if (!enum_func(pair->key, pair->value, obj)) {
                goto exit;
            }
            pair++;
            j++;
        }
        bucket++;
        i++;
    }
    exit:;
    return true;
}

// Returns a pair from the bucket that matches the provided key,
// or null if no such pair exist.
//
static pair_t *get_pair(bucket_t *bucket, const char *key) {
    unsigned int n = bucket->count;
    if (n == 0) {
        return NULL;
    }
    pair_t *pair = bucket->pairs;
    unsigned int i = 0;
    while (i < n) {
        if (NULL != pair->key && NULL != pair->value) {
            if (strcmp(pair->key, key) == 0) {
                return pair;
            }
        }
        pair++;
        i++;
    }

    return NULL;
}

// Returns a hash code for the provided string.
//
static unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c = 0;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/*

 GNU LESSER GENERAL PUBLIC LICENSE
 Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


 This version of the GNU Lesser General Public License incorporates
 the terms and conditions of version 3 of the GNU General Public
 License, supplemented by the additional permissions listed below.

 0. Additional Definitions.

 As used herein, "this License" refers to version 3 of the GNU Lesser
 General Public License, and the "GNU GPL" refers to version 3 of the GNU
 General Public License.

 "The Library" refers to a covered work governed by this License,
 other than an Application or a Combined Work as defined below.

 An "Application" is any work that makes use of an interface provided
 by the Library, but which is not otherwise based on the Library.
 Defining a subclass of a class defined by the Library is deemed a mode
 of using an interface provided by the Library.

 A "Combined Work" is a work produced by combining or linking an
 Application with the Library.  The particular version of the Library
 with which the Combined Work was made is also called the "Linked
 Version".

 The "Minimal Corresponding Source" for a Combined Work means the
 Corresponding Source for the Combined Work, excluding any source code
 for portions of the Combined Work that, considered in isolation, are
 based on the Application, and not on the Linked Version.

 The "Corresponding Application Code" for a Combined Work means the
 object code and/or source code for the Application, including any data
 and utility programs needed for reproducing the Combined Work from the
 Application, but excluding the System Libraries of the Combined Work.

 1. Exception to Section 3 of the GNU GPL.

 You may convey a covered work under sections 3 and 4 of this License
 without being bound by section 3 of the GNU GPL.

 2. Conveying Modified Versions.

 If you modify a copy of the Library, and, in your modifications, a
 facility refers to a function or data to be supplied by an Application
 that uses the facility (other than as an argument passed when the
 facility is invoked), then you may convey a copy of the modified
 version:

 a) under this License, provided that you make a good faith effort to
 ensure that, in the event an Application does not supply the
 function or data, the facility still operates, and performs
 whatever part of its purpose remains meaningful, or

 b) under the GNU GPL, with none of the additional permissions of
 this License applicable to that copy.

 3. Object Code Incorporating Material from Library Header Files.

 The object code form of an Application may incorporate material from
 a header file that is part of the Library.  You may convey such object
 code under terms of your choice, provided that, if the incorporated
 material is not limited to numerical parameters, data structure
 layouts and accessors, or small macros, inline functions and templates
 (ten or fewer lines in length), you do both of the following:

 a) Give prominent notice with each copy of the object code that the
 Library is used in it and that the Library and its use are
 covered by this License.

 b) Accompany the object code with a copy of the GNU GPL and this license
 document.

 4. Combined Works.

 You may convey a Combined Work under terms of your choice that,
 taken together, effectively do not restrict modification of the
 portions of the Library contained in the Combined Work and reverse
 engineering for debugging such modifications, if you also do each of
 the following:

 a) Give prominent notice with each copy of the Combined Work that
 the Library is used in it and that the Library and its use are
 covered by this License.

 b) Accompany the Combined Work with a copy of the GNU GPL and this license
 document.

 c) For a Combined Work that displays copyright notices during
 execution, include the copyright notice for the Library among
 these notices, as well as a reference directing the user to the
 copies of the GNU GPL and this license document.

 d) Do one of the following:

 0) Convey the Minimal Corresponding Source under the terms of this
 License, and the Corresponding Application Code in a form
 suitable for, and under terms that permit, the user to
 recombine or relink the Application with a modified version of
 the Linked Version to produce a modified Combined Work, in the
 manner specified by section 6 of the GNU GPL for conveying
 Corresponding Source.

 1) Use a suitable shared library mechanism for linking with the
 Library.  A suitable mechanism is one that (a) uses at run time
 a copy of the Library already present on the user's computer
 system, and (b) will operate properly with a modified version
 of the Library that is interface-compatible with the Linked
 Version.

 e) Provide Installation Information, but only if you would otherwise
 be required to provide such information under section 6 of the
 GNU GPL, and only to the extent that such information is
 necessary to install and execute a modified version of the
 Combined Work produced by recombining or relinking the
 Application with a modified version of the Linked Version. (If
 you use option 4d0, the Installation Information must accompany
 the Minimal Corresponding Source and Corresponding Application
 Code. If you use option 4d1, you must provide the Installation
 Information in the manner specified by section 6 of the GNU GPL
 for conveying Corresponding Source.)

 5. Combined Libraries.

 You may place library facilities that are a work based on the
 Library side by side in a single library together with other library
 facilities that are not Applications and are not covered by this
 License, and convey such a combined library under terms of your
 choice, if you do both of the following:

 a) Accompany the combined library with a copy of the same work based
 on the Library, uncombined with any other library facilities,
 conveyed under the terms of this License.

 b) Give prominent notice with the combined library that part of it
 is a work based on the Library, and explaining where to find the
 accompanying uncombined form of the same work.

 6. Revised Versions of the GNU Lesser General Public License.

 The Free Software Foundation may publish revised and/or new versions
 of the GNU Lesser General Public License from time to time. Such new
 versions will be similar in spirit to the present version, but may
 differ in detail to address new problems or concerns.

 Each version is given a distinguishing version number. If the
 Library as you received it specifies that a certain numbered version
 of the GNU Lesser General Public License "or any later version"
 applies to it, you have the option of following the terms and
 conditions either of that published version or of any later version
 published by the Free Software Foundation. If the Library as you
 received it does not specify a version number of the GNU Lesser
 General Public License, you may choose any version of the GNU Lesser
 General Public License ever published by the Free Software Foundation.

 If the Library as you received it specifies that a proxy can decide
 whether future versions of the GNU Lesser General Public License shall
 apply, that proxy's public statement of acceptance of any version is
 permanent authorization for you to choose that version for the
 Library.

 */
#pragma clang diagnostic pop