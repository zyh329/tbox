/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		sqlite3.c
 * @ingroup 	database
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"sqlite3"
#define TB_TRACE_MODULE_DEBUG 			(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sql.h"
#include "sqlite3.h"
#include "../libc/libc.h"
#include "../stream/stream.h"
#include <sqlite3.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the sqlite3 result row type
typedef struct __tb_database_sqlite3_result_row_t
{
	// the iterator
	tb_iterator_t 						itor;

	// the row
	tb_size_t 							row;

	// the col count
	tb_size_t 							count;

	// the col value
	tb_database_sql_value_t 			value;

}tb_database_sqlite3_result_row_t;

// the sqlite3 result type
typedef struct __tb_database_sqlite3_result_t
{
	// the iterator
	tb_iterator_t 						itor;

	// the result
	tb_char_t** 						result;

	// the stmt
	sqlite3_stmt* 						stmt;

	// the row count
	tb_size_t 							count;

	// the row
	tb_database_sqlite3_result_row_t 	row;

}tb_database_sqlite3_result_t;

// the sqlite3 type
typedef struct __tb_database_sqlite3_t
{
	// the base
	tb_database_sql_t 					base;

	// the database
	sqlite3* 							database;

	// the result
	tb_database_sqlite3_result_t 		result;

}tb_database_sqlite3_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator implementation
 */
static tb_size_t tb_sqlite3_result_row_iterator_size(tb_iterator_t* iterator)
{
	// check
	tb_database_sqlite3_result_t* result = (tb_database_sqlite3_result_t*)iterator;
	tb_assert_return_val(result, 0);

	// size
	return result->count;
}
static tb_size_t tb_sqlite3_result_row_iterator_head(tb_iterator_t* iterator)
{
	// head
	return 0;
}
static tb_size_t tb_sqlite3_result_row_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_database_sqlite3_result_t* result = (tb_database_sqlite3_result_t*)iterator;
	tb_assert_return_val(result, 0);

	// tail
	return result->count;
}
static tb_size_t tb_sqlite3_result_row_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_sqlite3_result_t* result = (tb_database_sqlite3_result_t*)iterator;
	tb_assert_return_val(result, 0);
	tb_assert_and_check_return_val(itor && itor <= result->count, result->count);

	// cannot be the stmt result
	tb_assert_and_check_return_val(!result->stmt, result->count);

	// prev
	return itor - 1;
}
static tb_size_t tb_sqlite3_result_row_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_sqlite3_result_t* result = (tb_database_sqlite3_result_t*)iterator;
	tb_assert_return_val(result, 0);
	tb_assert_and_check_return_val(itor < result->count, result->count);

	// stmt result?
	if (result->stmt)
	{
		// step stmt
		tb_int_t ok = sqlite3_step(result->stmt);
		tb_check_return_val(ok == SQLITE_ROW, result->count);
	}

	// next
	return itor + 1;
}
static tb_pointer_t tb_sqlite3_result_row_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_sqlite3_result_t* result = (tb_database_sqlite3_result_t*)iterator;
	tb_assert_and_check_return_val(result && (result->result || result->stmt) && itor < result->count, tb_null);

	// save the row
	result->row.row = itor;

	// the row iterator
	return (tb_pointer_t)&result->row;
}
static tb_size_t tb_sqlite3_result_col_iterator_size(tb_iterator_t* iterator)
{
	// check
	tb_database_sqlite3_result_row_t* row = (tb_database_sqlite3_result_row_t*)iterator;
	tb_assert_and_check_return_val(row, 0);

	// size
	return row->count;
}
static tb_size_t tb_sqlite3_result_col_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_database_sqlite3_result_row_t* row = (tb_database_sqlite3_result_row_t*)iterator;
	tb_assert_and_check_return_val(row, 0);

	// head
	return 0;
}
static tb_size_t tb_sqlite3_result_col_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_database_sqlite3_result_row_t* row = (tb_database_sqlite3_result_row_t*)iterator;
	tb_assert_and_check_return_val(row, 0);

	// tail
	return row->count;
}
static tb_size_t tb_sqlite3_result_col_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_sqlite3_result_row_t* row = (tb_database_sqlite3_result_row_t*)iterator;
	tb_assert_and_check_return_val(row && itor && itor <= row->count, 0);

	// prev
	return itor - 1;
}
static tb_size_t tb_sqlite3_result_col_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_sqlite3_result_row_t* row = (tb_database_sqlite3_result_row_t*)iterator;
	tb_assert_and_check_return_val(row && itor < row->count, row->count);

	// next
	return itor + 1;
}
static tb_pointer_t tb_sqlite3_result_col_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_database_sqlite3_result_row_t* row = (tb_database_sqlite3_result_row_t*)iterator;
	tb_assert_and_check_return_val(row && itor < row->count, tb_null);

	// the sqlite
	tb_database_sqlite3_t* sqlite = (tb_database_sqlite3_t*)iterator->priv;
	tb_assert_and_check_return_val(sqlite, tb_null);

	// result?
	if (sqlite->result.result)
	{
		// init value
		tb_database_sql_value_name_set(&row->value, (tb_char_t const*)sqlite->result.result[itor]);
		tb_database_sql_value_set_text32(&row->value, (tb_char_t const*)sqlite->result.result[((1 + sqlite->result.row.row) * row->count) + itor], 0);
		return (tb_pointer_t)&row->value;
	}
	// stmt result?
	else if (sqlite->result.stmt)
	{
		// init name
		tb_database_sql_value_name_set(&row->value, sqlite3_column_name(sqlite->result.stmt, itor));

		// init type
		tb_size_t type = sqlite3_column_type(sqlite->result.stmt, itor);
		switch (type)
		{
		case SQLITE_INTEGER:
			tb_database_sql_value_set_int32(&row->value, sqlite3_column_int(sqlite->result.stmt, itor));
			break;
		case SQLITE_TEXT:
			tb_database_sql_value_set_text32(&row->value, (tb_char_t const*)sqlite3_column_text(sqlite->result.stmt, itor), sqlite3_column_bytes(sqlite->result.stmt, itor));
			break;
		case SQLITE_FLOAT:
#ifdef TB_CONFIG_TYPE_FLOAT
			tb_database_sql_value_set_double(&row->value, sqlite3_column_double(sqlite->result.stmt, itor));
			break;
#else
			// trace
			tb_trace1_e("float type is not supported, at col: %lu, please enable float config!", itor);
			return tb_null;
#endif
		case SQLITE_BLOB:
			tb_database_sql_value_set_blob32(&row->value, sqlite3_column_blob(sqlite->result.stmt, itor), sqlite3_column_bytes(sqlite->result.stmt, itor));
			break;
		case SQLITE_NULL:
			tb_database_sql_value_set_null(&row->value);
			break;
		default:
			tb_trace_e("unknown field type: %s, at col: %lu", type, itor);
			return tb_null;
		}

		// ok
		return (tb_pointer_t)&row->value;
	}

	// failed
	tb_assert(0);
	return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_database_sqlite3_t* tb_database_sqlite3_cast(tb_database_sql_t* database)
{
	// check
	tb_assert_and_check_return_val(database && database->type == TB_DATABASE_SQL_TYPE_SQLITE3, tb_null);

	// cast
	return (tb_database_sqlite3_t*)database;
}
static tb_bool_t tb_database_sqlite3_open(tb_database_sql_t* database)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite, tb_false);

	// done
	tb_bool_t 			ok = tb_false;
	tb_char_t const* 	path = tb_null;
	do
	{
		// the database path
		path = tb_url_path_get(&database->url);
		tb_assert_and_check_break(path);

		// open database
		if (SQLITE_OK != sqlite3_open(path, &sqlite->database) || !sqlite->database) 
		{
			// trace
			if (sqlite->database) tb_trace_e("open: %s failed, error: %s", path, sqlite3_errmsg(sqlite->database));
			break;
		}

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("open: %s: %s", path, ok? "ok" : "no");

	// ok?
	return ok;
}
static tb_void_t tb_database_sqlite3_clos(tb_database_sql_t* database)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return(sqlite);
	
	// exit result first if exists
	if (sqlite->result.result) sqlite3_free_table(sqlite->result.result);
	sqlite->result.result = tb_null;

	// close database
	if (sqlite->database) sqlite3_close(sqlite->database);
	sqlite->database = tb_null;
}
static tb_void_t tb_database_sqlite3_exit(tb_database_sql_t* database)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return(sqlite);

	// close it first
	tb_database_sqlite3_clos(database);

	// exit url
	tb_url_exit(&database->url);

	// exit it
	tb_free(sqlite);
}
static tb_bool_t tb_database_sqlite3_done(tb_database_sql_t* database, tb_char_t const* sql)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite && sqlite->database && sql, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// exit result first if exists
		if (sqlite->result.result) sqlite3_free_table(sqlite->result.result);
		sqlite->result.result = tb_null;

		// clear the lasr stmt first
		sqlite->result.stmt = tb_null;

		// clear the result row count first
		sqlite->result.count = 0;

		// clear the result col count first
		sqlite->result.row.count = 0;

		// done sql
		tb_int_t 	row_count = 0;
		tb_int_t 	col_count = 0;
		tb_char_t* 	error = tb_null;
		if (SQLITE_OK != sqlite3_get_table(sqlite->database, sql, &sqlite->result.result, &row_count, &col_count, &error))
		{
			// trace
			tb_trace_e("done: sql: %s failed, error: %s", sql, error);

			// exit error
			if (error) sqlite3_free(error);
			break;
		}

		// no result?
		if (!row_count)
		{
			// exit result
			if (sqlite->result.result) sqlite3_free_table(sqlite->result.result);
			sqlite->result.result = tb_null;

			// trace
			tb_trace_d("done: sql: %s: ok", sql);

			// ok
			ok = tb_true;
			break;
		}

		// save the result iterator mode
		sqlite->result.itor.mode = TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_READONLY;

		// save result row count
		sqlite->result.count = row_count;

		// save result col count
		sqlite->result.row.count = col_count;

		// trace
		tb_trace_d("done: sql: %s: ok", sql);

		// ok
		ok = tb_true;
	
	} while (0);
	
	// ok?
	return ok;
}
static tb_void_t tb_database_sqlite3_result_exit(tb_database_sql_t* database, tb_iterator_t* result)
{
	// check
	tb_database_sqlite3_result_t* sqlite3_result = (tb_database_sqlite3_result_t*)result;
	tb_assert_and_check_return(sqlite3_result);

	// exit result
	if (sqlite3_result->result) sqlite3_free_table(sqlite3_result->result);
	sqlite3_result->result = tb_null;

	// clear the stmt
	sqlite3_result->stmt = tb_null;

	// clear result
	sqlite3_result->count = 0;
	sqlite3_result->row.count = 0;
}
static tb_iterator_t* tb_database_sqlite3_result_load(tb_database_sql_t* database, tb_bool_t ball)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite && sqlite->database, tb_null);

	// ok?
	return (sqlite->result.result || sqlite->result.stmt)? (tb_iterator_t*)&sqlite->result : tb_null;
}
static tb_void_t tb_database_sqlite3_stmt_exit(tb_database_sql_t* database, tb_handle_t stmt)
{
	// exit stmt
	if (stmt) sqlite3_finalize((sqlite3_stmt*)stmt);
}
static tb_handle_t tb_database_sqlite3_stmt_init(tb_database_sql_t* database, tb_char_t const* sql)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite && sqlite->database && sql, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	sqlite3_stmt* 	stmt = tb_null;
	do
	{
		// init stmt
		if (SQLITE_OK != sqlite3_prepare_v2(sqlite->database, sql, -1, &stmt, 0))
		{
			// trace
			tb_trace_e("stmt: init %s failed, error: %s", sql, sqlite3_errmsg(sqlite->database));
		}

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return (tb_handle_t)stmt;
}
static tb_bool_t tb_database_sqlite3_stmt_done(tb_database_sql_t* database, tb_handle_t stmt)
{
	// check
	tb_database_sqlite3_t* sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite && sqlite->database && stmt, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// exit result first if exists
		if (sqlite->result.result) sqlite3_free_table(sqlite->result.result);
		sqlite->result.result = tb_null;

		// clear the last stmt first
		sqlite->result.stmt = tb_null;

		// clear the result row count first
		sqlite->result.count = 0;

		// clear the result col count first
		sqlite->result.row.count = 0;

		// step stmt
		tb_int_t result = sqlite3_step((sqlite3_stmt*)stmt);
		tb_assert_and_check_break(result == SQLITE_DONE || result == SQLITE_ROW);

		// exists result?
		if (result == SQLITE_ROW)
		{
			// save the result iterator mode
			sqlite->result.itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_READONLY;

			// save stmt for iterating it
			sqlite->result.stmt = (sqlite3_stmt*)stmt;

			// save result row count
			sqlite->result.count = (tb_size_t)-1;

			// save result col count
			sqlite->result.row.count = sqlite3_column_count((sqlite3_stmt*)stmt);
		}

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_database_sqlite3_stmt_bind(tb_database_sql_t* database, tb_handle_t stmt, tb_database_sql_value_t const* list, tb_size_t size)
{
	// check
	tb_database_sqlite3_t* 		sqlite = tb_database_sqlite3_cast(database);
	tb_assert_and_check_return_val(sqlite && sqlite->database && stmt && list && size, tb_false);

	// walk
	tb_size_t i = 0;
	for (i = 0; i < size; i++)
	{
		// the value
		tb_database_sql_value_t const* value = &list[i];

		// done
		tb_int_t ok = 0;
		switch (value->type)
		{
		case TB_DATABASE_SQL_VALUE_TYPE_TEXT32:
		case TB_DATABASE_SQL_VALUE_TYPE_TEXT16:
		case TB_DATABASE_SQL_VALUE_TYPE_TEXT8:
			ok = sqlite3_bind_text((sqlite3_stmt*)stmt, (tb_int_t)(i + 1), value->text.data, (tb_int_t)tb_database_sql_value_size(value), tb_null);
			break;
		case TB_DATABASE_SQL_VALUE_TYPE_INT64:
			ok = sqlite3_bind_int64((sqlite3_stmt*)stmt, (tb_int_t)(i + 1), value->i64);
			break;
		case TB_DATABASE_SQL_VALUE_TYPE_INT32:
		case TB_DATABASE_SQL_VALUE_TYPE_INT16:
		case TB_DATABASE_SQL_VALUE_TYPE_INT8:
			ok = sqlite3_bind_int((sqlite3_stmt*)stmt, (tb_int_t)(i + 1), (tb_int_t)tb_database_sql_value_int32(value));
			break;
		case TB_DATABASE_SQL_VALUE_TYPE_BLOB32:
		case TB_DATABASE_SQL_VALUE_TYPE_BLOB16:
		case TB_DATABASE_SQL_VALUE_TYPE_BLOB8:
			ok = sqlite3_bind_blob((sqlite3_stmt*)stmt, (tb_int_t)(i + 1), value->blob.data, (tb_int_t)value->blob.size, tb_null);
			break;
#ifdef TB_CONFIG_TYPE_FLOAT
		case TB_DATABASE_SQL_VALUE_TYPE_FLOAT:
		case TB_DATABASE_SQL_VALUE_TYPE_DOUBLE:
			ok = sqlite3_bind_double((sqlite3_stmt*)stmt, (tb_int_t)(i + 1), (tb_double_t)tb_database_sql_value_double(value));
			break;
#endif
		case TB_DATABASE_SQL_VALUE_TYPE_NULL:
			ok = sqlite3_bind_null((sqlite3_stmt*)stmt, (tb_int_t)(i + 1));
			break;
		default:
			tb_trace_e("bind: unknown value type: %lu", value->type);
			break;
		}

		// failed?
		if (SQLITE_OK != ok)
		{
			// trace
			tb_trace_e("stmt: bind value[%lu] failed, error: %s", i, sqlite3_errmsg(sqlite->database));
			break;
		}
	}

	// ok?
	return (i == size)? tb_true : tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_size_t tb_database_sqlite3_probe(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, 0);

	// done
	tb_size_t 			score = 0;
	tb_basic_stream_t* 	stream = tb_null;
	do
	{
		// the url arguments
		tb_char_t const* args = tb_url_args_get(url);
		if (args)
		{
			// find the database type
			tb_char_t const* ptype = tb_stristr(args, "type=");
			if (ptype && !tb_strnicmp(ptype + 5, "sqlite3", 7))
			{
				// ok
				score = 100;
				break;
			}
		}

		// has host or port? no sqlite3
		if (tb_url_host_get(url) || tb_url_port_get(url)) break;

		// the database path
		tb_char_t const* path = tb_url_path_get(url);
		tb_assert_and_check_break(path);

		// init stream
		stream = tb_basic_stream_init_from_url(path);
		tb_assert_and_check_break(stream);

		// open stream
		if (!tb_basic_stream_open(stream)) break;

		// read head
		tb_char_t head[16] = {0};
		if (!tb_basic_stream_bread(stream, (tb_byte_t*)head, sizeof(head))) break;

		// is sqlite3?
		if (!tb_stricmp(head, "SQLite format 3")) score = 100;

	} while (0);

	// exit stream
	if (stream) tb_basic_stream_exit(stream);
	stream = tb_null;

	// trace
	tb_trace_d("probe: %s, score: %lu", tb_url_get((tb_url_t*)url), score);

	// ok?
	return score;
}
tb_database_sql_t* tb_database_sqlite3_init(tb_url_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 				ok = tb_false;
	tb_database_sqlite3_t* 	sqlite = tb_null;
	do
	{
		// make database
		sqlite = tb_malloc0(sizeof(tb_database_sqlite3_t));
		tb_assert_and_check_break(sqlite);

		// init database
		sqlite->base.type 			= TB_DATABASE_SQL_TYPE_SQLITE3;
		sqlite->base.open 			= tb_database_sqlite3_open;
		sqlite->base.clos 			= tb_database_sqlite3_clos;
		sqlite->base.exit 			= tb_database_sqlite3_exit;
		sqlite->base.done 			= tb_database_sqlite3_done;
		sqlite->base.result_load 	= tb_database_sqlite3_result_load;
		sqlite->base.result_exit 	= tb_database_sqlite3_result_exit;
		sqlite->base.stmt_init 		= tb_database_sqlite3_stmt_init;
		sqlite->base.stmt_exit 		= tb_database_sqlite3_stmt_exit;
		sqlite->base.stmt_done 		= tb_database_sqlite3_stmt_done;
		sqlite->base.stmt_bind 		= tb_database_sqlite3_stmt_bind;

		// init result row iterator
		sqlite->result.itor.mode 	= 0;
		sqlite->result.itor.priv 	= (tb_pointer_t)sqlite;
		sqlite->result.itor.step 	= 0;
		sqlite->result.itor.size 	= tb_sqlite3_result_row_iterator_size;
		sqlite->result.itor.head 	= tb_sqlite3_result_row_iterator_head;
		sqlite->result.itor.tail 	= tb_sqlite3_result_row_iterator_tail;
		sqlite->result.itor.prev 	= tb_sqlite3_result_row_iterator_prev;
		sqlite->result.itor.next 	= tb_sqlite3_result_row_iterator_next;
		sqlite->result.itor.item 	= tb_sqlite3_result_row_iterator_item;
		sqlite->result.itor.copy 	= tb_null;
		sqlite->result.itor.comp 	= tb_null;

		// init result col iterator
		sqlite->result.row.itor.mode = TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_READONLY;
		sqlite->result.row.itor.priv = (tb_pointer_t)sqlite;
		sqlite->result.row.itor.step = 0;
		sqlite->result.row.itor.size = tb_sqlite3_result_col_iterator_size;
		sqlite->result.row.itor.head = tb_sqlite3_result_col_iterator_head;
		sqlite->result.row.itor.tail = tb_sqlite3_result_col_iterator_tail;
		sqlite->result.row.itor.prev = tb_sqlite3_result_col_iterator_prev;
		sqlite->result.row.itor.next = tb_sqlite3_result_col_iterator_next;
		sqlite->result.row.itor.item = tb_sqlite3_result_col_iterator_item;
		sqlite->result.row.itor.copy = tb_null;
		sqlite->result.row.itor.comp = tb_null;

		// init url
		if (!tb_url_init(&sqlite->base.url)) break;

		// copy url
		tb_url_copy(&sqlite->base.url, url);

		// init state
		sqlite->base.state = TB_STATE_OK;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit database
		if (sqlite) tb_database_sqlite3_exit((tb_database_sql_t*)sqlite);
		sqlite = tb_null;
	}

	// ok?
	return (tb_database_sql_t*)sqlite;
}

