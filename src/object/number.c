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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		number.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the number type
typedef struct __tb_number_t
{
	// the object base
	tb_object_t 		base;

	// the number type
	tb_size_t 			type;

	// the number value
	union
	{
		// the uint8
		tb_uint8_t 		u8;
	
		// the sint8
		tb_sint8_t 		s8;
	
		// the uint16
		tb_uint16_t 	u16;
	
		// the sint16
		tb_sint16_t 	s16;
	
		// the uint32
		tb_uint32_t 	u32;
	
		// the sint32
		tb_sint32_t 	s32;
	
		// the uint64
		tb_uint64_t 	u64;
	
		// the sint64
		tb_sint64_t 	s64;
	
		// the float
		tb_float_t 		f;
	
		// the double
		tb_double_t 	d;
	
	}v;

}tb_number_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_number_t* tb_number_cast(tb_object_t* object)
{
	// check
	tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_NUMBER, tb_null);

	// cast
	return (tb_number_t*)object;
}
static tb_object_t* tb_number_copy(tb_object_t* object)
{
	// check
	tb_number_t* number = (tb_number_t*)object;
	tb_assert_and_check_return_val(number, tb_null);

	// copy
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		return tb_number_init_uint64(number->v.u64);
	case TB_NUMBER_TYPE_SINT64:
		return tb_number_init_sint64(number->v.s64);
	case TB_NUMBER_TYPE_UINT32:
		return tb_number_init_uint32(number->v.u32);
	case TB_NUMBER_TYPE_SINT32:
		return tb_number_init_sint32(number->v.s32);
	case TB_NUMBER_TYPE_UINT16:
		return tb_number_init_uint16(number->v.u16);
	case TB_NUMBER_TYPE_SINT16:
		return tb_number_init_sint16(number->v.s16);
	case TB_NUMBER_TYPE_UINT8:
		return tb_number_init_uint8(number->v.u8);
	case TB_NUMBER_TYPE_SINT8:
		return tb_number_init_sint8(number->v.s8);
	case TB_NUMBER_TYPE_FLOAT:
		return tb_number_init_float(number->v.f);
	case TB_NUMBER_TYPE_DOUBLE:
		return tb_number_init_double(number->v.d);
	default:
		break;
	}

	return tb_null;
}
static tb_void_t tb_number_exit(tb_object_t* object)
{
	if (object) tb_free(object);
}
static tb_number_t* tb_number_init_base()
{
	// make
	tb_number_t* number = tb_malloc0(sizeof(tb_number_t));
	tb_assert_and_check_return_val(number, tb_null);

	// init object
	if (!tb_object_init(number, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_NUMBER)) goto fail;

	// init base
	number->base.copy = tb_number_copy;
	number->base.exit = tb_number_exit;

	// ok
	return number;

	// no
fail:
	if (number) tb_free(number);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_object_t* tb_number_init_uint8(tb_uint8_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT8;
	number->v.u8 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_sint8(tb_sint8_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT8;
	number->v.s8 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_uint16(tb_uint16_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT16;
	number->v.u16 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_sint16(tb_sint16_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT16;
	number->v.s16 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_uint32(tb_uint32_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT32;
	number->v.u32 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_sint32(tb_sint32_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT32;
	number->v.s32 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_uint64(tb_uint64_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_UINT64;
	number->v.u64 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_sint64(tb_sint64_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_SINT64;
	number->v.s64 = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_float(tb_float_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_FLOAT;
	number->v.f = value;

	// ok
	return number;
}

tb_object_t* tb_number_init_double(tb_double_t value)
{
	// make
	tb_number_t* number = tb_number_init_base();
	tb_assert_and_check_return_val(number, tb_null);

	// init value
	number->type = TB_NUMBER_TYPE_DOUBLE;
	number->v.d = value;

	// ok
	return number;
}

tb_size_t tb_number_type(tb_object_t* number)
{
	// check
	tb_assert_and_check_return_val(number, TB_NUMBER_TYPE_NONE);

	// type
	return number->type;
}

tb_uint8_t tb_number_uint8(tb_object_t* number)
{
	return (tb_uint8_t)tb_number_uint64(number);
}
tb_sint8_t tb_number_sint8(tb_object_t* number)
{
	return (tb_sint8_t)tb_number_sint64(number);
}
tb_uint16_t tb_number_uint16(tb_object_t* number)
{
	return (tb_uint16_t)tb_number_uint64(number);
}
tb_sint16_t tb_number_sint16(tb_object_t* number)
{
	return (tb_sint16_t)tb_number_sint64(number);
}
tb_uint32_t tb_number_uint32(tb_object_t* number)
{
	return (tb_uint32_t)tb_number_uint64(number);
}
tb_sint32_t tb_number_sint32(tb_object_t* number)
{
	return (tb_sint32_t)tb_number_sint64(number);
}
tb_uint64_t tb_number_uint64(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// uint64
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		return number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return number->v.s64;
	case TB_NUMBER_TYPE_UINT32:
		return number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return number->v.s32;
	case TB_NUMBER_TYPE_UINT16:
		return number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return number->v.s16;
	case TB_NUMBER_TYPE_UINT8:
		return number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return number->v.s8;
	case TB_NUMBER_TYPE_FLOAT:
		return (tb_uint64_t)number->v.f;
	case TB_NUMBER_TYPE_DOUBLE:
		return (tb_uint64_t)number->v.d;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}

tb_sint64_t tb_number_sint64(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// sint64
	switch (number->type)
	{
	case TB_NUMBER_TYPE_UINT64:
		return number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return number->v.s64;
	case TB_NUMBER_TYPE_UINT32:
		return number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return number->v.s32;
	case TB_NUMBER_TYPE_UINT16:
		return number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return number->v.s16;
	case TB_NUMBER_TYPE_UINT8:
		return number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return number->v.s8;
	case TB_NUMBER_TYPE_FLOAT:
		return (tb_sint64_t)number->v.f;
	case TB_NUMBER_TYPE_DOUBLE:
		return (tb_sint64_t)number->v.d;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}
tb_float_t tb_number_float(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// float
	switch (number->type)
	{
	case TB_NUMBER_TYPE_FLOAT:
		return number->v.f;
	case TB_NUMBER_TYPE_DOUBLE:
		return (tb_float_t)number->v.d;
	case TB_NUMBER_TYPE_UINT8:
		return (tb_float_t)number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return (tb_float_t)number->v.s8;
	case TB_NUMBER_TYPE_UINT16:
		return (tb_float_t)number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return (tb_float_t)number->v.s16;
	case TB_NUMBER_TYPE_UINT32:
		return (tb_float_t)number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return (tb_float_t)number->v.s32;
	case TB_NUMBER_TYPE_UINT64:
		return (tb_float_t)number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return (tb_float_t)number->v.s64;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}
tb_double_t tb_number_double(tb_object_t* object)
{
	// check
	tb_number_t* number = tb_number_cast(object);
	tb_assert_and_check_return_val(number, 0);

	// double
	switch (number->type)
	{
	case TB_NUMBER_TYPE_DOUBLE:
		return number->v.d;
	case TB_NUMBER_TYPE_FLOAT:
		return (tb_double_t)number->v.f;
	case TB_NUMBER_TYPE_UINT8:
		return (tb_double_t)number->v.u8;
	case TB_NUMBER_TYPE_SINT8:
		return (tb_double_t)number->v.s8;
	case TB_NUMBER_TYPE_UINT16:
		return (tb_double_t)number->v.u16;
	case TB_NUMBER_TYPE_SINT16:
		return (tb_double_t)number->v.s16;
	case TB_NUMBER_TYPE_UINT32:
		return (tb_double_t)number->v.u32;
	case TB_NUMBER_TYPE_SINT32:
		return (tb_double_t)number->v.s32;
	case TB_NUMBER_TYPE_UINT64:
		return (tb_double_t)number->v.u64;
	case TB_NUMBER_TYPE_SINT64:
		return (tb_double_t)number->v.s64;
	default:
		break;
	}

	tb_assert(0);
	return 0;
}
