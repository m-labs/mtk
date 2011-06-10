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

#ifndef __MTK_INCLUDE_MTKI18N_H_
#define __MTK_INCLUDE_MTKI18N_H_

struct mtk_i18n_entry {
	const char *original;
	const char *translated;
};

void mtk_set_language(struct mtk_i18n_entry *table);
const char *mtk_translate(char *original);

#endif /* __MTK_INCLUDE_MTKI18N_H_ */
