/*
 * \brief   DOpE hash table module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 *
 * This component provides a generic interface
 * to handle hash  tables. New elements can be
 * added while specifying  an identifier. This
 * identifier is  also used to retrieve a hash
 * table element.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "hashtab.h"
#include "list_macros.h"

#define MIN(a,b) ((a)<(b)?(a):(b))

struct hashtab_entry;
struct hashtab_entry {
	char *ident;
	void *value;
	struct hashtab_entry *next;
	void (*destroy_elem_function) (void *value);
};

struct hashtab {
	int ref_cnt;                    /* reference counter                   */
	u32 tab_size;                   /* hash table size                     */
	u32 max_hash_length;            /* number of chars for building hashes */
	struct hashtab_entry **tab;     /* hash table itself                   */
};

int init_hashtable(struct dope_services *d);
void hashtab_print_info(HASHTAB *h);


/********************************
 ** Functions for internal use **
 ********************************/

/**
 * Calculate hash value for a given string by adding its characters
 */
static u32 hash_value(char *ident, u32 max_cnt)
{
	u32 result = 0;
	if (!ident) return 0;
	while ((*ident) && (max_cnt>0)) {
		result += *ident;
		ident++;
		max_cnt--;
	}
	return result;
}


/***********************
 ** Service functions **
 ***********************/

/**
 * Create a new hash table of the specified size
 */
static HASHTAB *hashtab_create(u32 tab_size, u32 max_hash_length)
{
	INFO(printf("Entering HashTable(create)!\n");)
	struct hashtab *new_hashtab;
	new_hashtab = (struct hashtab *)zalloc(sizeof(struct hashtab)
	                                     + sizeof(void *)*tab_size);
	INFO(printf("HashTable(create): zalloc done\n");)
	if (!new_hashtab) {
		INFO(printf("HashTable(create): out of memory!\n");)
		return NULL;
	}

	new_hashtab->ref_cnt  = 1;
	new_hashtab->tab_size = tab_size;
	new_hashtab->max_hash_length = max_hash_length;
	new_hashtab->tab = (struct hashtab_entry **)((long)new_hashtab + sizeof(struct hashtab));
	return new_hashtab;
}


/**
 * Free hashtab entry data structure
 */
static inline void free_hashtab_entry(struct hashtab_entry *e)
{
	if (e->ident)
		free(e->ident);
	if (e->destroy_elem_function)
		e->destroy_elem_function(e->value);
	free(e);
}


/**
 * Increment reference counter
 */
static void hashtab_inc_ref(HASHTAB *h)
{
	if (h) h->ref_cnt++;
}


/**
 * Decrement refcounter and destroy hash table with no references left
 */
static void hashtab_dec_ref(HASHTAB *h)
{
	u32 i;
	if (!h) return;

	/* decrement reference counter and return if there are any references left */
	if (--h->ref_cnt > 0) return;

	/* go through all elements of the hash table */
	for (i=0; i<h->tab_size; i++) {
		FREE_CONNECTED_LIST(struct hashtab_entry, h->tab[i], free_hashtab_entry);
	}
	free(h);
}


/**
 * Requests an element of a hash table
 */
static void *hashtab_get_elem(HASHTAB *h, char *ident, int max_len)
{
	u32 hashval;
	struct hashtab_entry *ce;

	if (!h) return NULL;
	hashval = hash_value(ident, MIN(max_len, h->max_hash_length)) % (h->tab_size);
	INFO(printf("hashval for %s is %ld, max_len = %d\n", ident, hashval, max_len));
	ce = h->tab[hashval];
	if (!ce) return NULL;
	while (!dope_streq(ident, ce->ident, max_len)) {
		ce = ce->next;
		if (!ce) return NULL;
	}
	return ce->value;
}


/**
 * Removes an element from a hash table
 */
static void hashtab_remove_elem(HASHTAB *h, char *ident)
{
	u32 hashval;
	struct hashtab_entry *ce, *re;
	if (!h) return;
	hashval = hash_value(ident, h->max_hash_length) % (h->tab_size);
	ce = h->tab[hashval];

	/* if element is first element of bucket */
	if (dope_streq(ident, ce->ident, 255)) {
		h->tab[hashval] = ce->next;
		free_hashtab_entry(ce);
		return;
	}
	
	/* search for for element in bucket list */
	while (ce->next && !dope_streq(ident, ce->next->ident, 255)) {
		ce = ce->next;
	}
	
	re = ce->next;   /* element to remove */
	
	/* return if desired element is not in bucket list */
	if (!re) return;

	/* unchain element */
	ce->next = re->next;
	
	/* deallocate and zero element */
	free_hashtab_entry(re);
}


/**
 * Add new hash table entry
 */
static void hashtab_add_elem(HASHTAB *h, char *ident, void *value)
{
	u32 hashval;
	struct hashtab_entry *ne;

	if (!h) return;
	if (hashtab_get_elem(h, ident, 255)) hashtab_remove_elem(h, ident);
	hashval = hash_value(ident, h->max_hash_length) % (h->tab_size);
	ne = (struct hashtab_entry *)zalloc(sizeof(struct hashtab_entry));
	ne->ident = dope_strdup(ident);
	ne->value = value;
	ne->next  = h->tab[hashval];
	h->tab[hashval] = ne;
}


/**
 * Print information about a hash table (only for debugging issues)
 */
void hashtab_print_info(HASHTAB *h)
{
	int i;
	struct hashtab_entry *e;
	if (!h) {
		printf(" hashtab is zero!\n");
		return;
	}
	printf(" tab_size=%d\n", (int)h->tab_size);
	printf(" max_hash_length=%d\n", (int)h->max_hash_length);
	for (i=0; i<h->tab_size; i++) {
		printf(" hash #%d: ", i);
		e = h->tab[i];
		if (!e) printf("empty");
		else {
			while (e) {
				printf("%s, ", e->ident);
				e = e->next;
			}
		}
		printf("\n");
	}
}


/**
 * Returns first element of a hash table
 */
static void *hashtab_get_first(HASHTAB *h)
{
	int i;
	for (i=0; i < h->tab_size; i++) {
		if (h->tab[i]) return h->tab[i]->value;
	}
	return NULL;
}


/**
 * Returns successor of a given hash table entry
 */
static void *hashtab_get_next(HASHTAB *h, void *value)
{
	struct hashtab_entry *e = NULL;
	int i;

	/* find first occurence of value in hash table lists */
	for (i=0; i < h->tab_size; i++) {
		e = h->tab[i];
		while (e && (e->value != value)) e = e->next;
		if (e && (e->value == value)) break;
	}

	if (!e || (i == h->tab_size)) return NULL;

	/* is the next element in current hash list? */
	if (e->next) return e->next->value;

	/* otherwise take first element of next hash list */
	for (i++; i < h->tab_size; i++) {
		if (h->tab[i]) return h->tab[i]->value;
	}
	return NULL;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct hashtab_services services = {
	hashtab_create,
	hashtab_inc_ref,
	hashtab_dec_ref,
	hashtab_add_elem,
	hashtab_get_elem,
	hashtab_remove_elem,
	hashtab_get_first,
	hashtab_get_next,
};


/************************
 ** Module entry point **
 ************************/

int init_hashtable(struct dope_services *d)
{
	d->register_module("HashTable 1.0", &services);
	return 1;
}
