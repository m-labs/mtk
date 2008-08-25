/*
 * \brief   Macros to deal with connected lists
 * \date    2004-05-18
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_WIDGET_LIST_MACROS_H_
#define _DOPE_WIDGET_LIST_MACROS_H_


/**
 * Free all elements of a connected list
 *
 * \param list_type           data type of list elements
 * \param first_list_element  begin of the list
 *
 * List elements need to have a ->next member.
 */
#define FREE_CONNECTED_LIST(list_type, first_list_element, free_func) { \
	list_type *curr_entry, *next_entry;       \
	curr_entry = first_list_element;          \
	while (curr_entry && curr_entry->next) {  \
		next_entry = curr_entry->next;        \
		free_func(curr_entry);                \
		curr_entry = next_entry;              \
	}                                         \
	if (curr_entry) free_func(curr_entry);    \
}


/**
 * Remove element from a connected list
 *
 * \param list_type           data type of list elements
 * \param first_list_element  pointer to headpointer of the list
 * \param element_to_remove   guess what?
 * \param free_func           function to free list element
 */
#define REMOVE_LIST_ELEMENT(list_type, first_list_element,      \
                            element_to_remove, free_func) {     \
	list_type *curr_entry, *next_entry;                         \
	curr_entry = *first_list_element;                           \
	if (curr_entry == element_to_remove) {                      \
		next_entry = curr_entry->next;                          \
		free_func(curr_entry);                                  \
		*first_list_element = next_entry;                       \
	}                                                           \
	while (curr_entry && curr_entry->next) {                    \
		next_entry = curr_entry->next;                          \
		if (next_entry == element_to_remove) {                  \
			curr_entry->next = next_entry->next;                \
			free_func(next_entry);                              \
		}                                                       \
		curr_entry = curr_entry->next;                          \
	}                                                           \
}


#endif /* _DOPE_WIDGET_LIST_MACROS_H_ */
