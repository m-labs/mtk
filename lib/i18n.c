/*
 * MTK
 * Copyright (C) 2011 Sebastien Bourdeauducq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdio.h>
#include <mtki18n.h>

static struct mtk_i18n_entry *current_table;

void mtk_set_language(struct mtk_i18n_entry *table)
{
	current_table = table;
	/* TODO: notify widgets */
}

const char *mtk_translate(char *original)
{
	int i;
	
	if(original == NULL)
		return NULL;
	/* Don't translate strings starting with escape */
	if(*original == '\e')
		return original+1;
	/* Don't translate empty strings */
	if(*original == 0)
		return original;
	if(current_table == NULL)
		return original;
	i = 0;
	while(current_table[i].original != NULL) {
		if(strcmp(current_table[i].original, original) == 0)
			return current_table[i].translated;
		i++;
	}
	printf("Warning: no translation found for '%s'\n", original);
	return original;
}
