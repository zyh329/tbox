/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>


static tb_void_t tb_test_sha(tb_size_t mode, tb_char_t const* data)
{
	tb_byte_t ob[32];
	tb_size_t on = tb_sha_encode(mode, data, tb_strlen(data), ob, 32);
	tb_assert_and_check_return((on << 3) == mode);

	tb_size_t i = 0;
	tb_char_t sha[256] = {0};
	for (i = 0; i < on; ++i) tb_snprintf(sha + (i << 1), 3, "%02X", ob[i]);
	tb_printf("[sha]: %d = %s\n", mode, sha);
}
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!argv[1]) return 0;
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_test_sha(TB_SHA_MODE_SHA1_160, argv[1]);
	tb_test_sha(TB_SHA_MODE_SHA2_224, argv[1]);
	tb_test_sha(TB_SHA_MODE_SHA2_256, argv[1]);

	tb_exit();
	return 0;
}
