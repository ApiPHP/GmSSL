/* ====================================================================
 * Copyright (c) 2016 - 2018 The GmSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the GmSSL Project.
 *    (http://gmssl.org/)"
 *
 * 4. The name "GmSSL Project" must not be used to endorse or promote
 *    products derived from this software without prior written
 *    permission. For written permission, please contact
 *    guanzhi1980@gmail.com.
 *
 * 5. Products derived from this software may not be called "GmSSL"
 *    nor may "GmSSL" appear in their names without prior written
 *    permission of the GmSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the GmSSL Project
 *    (http://gmssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE GmSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE GmSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <openssl/sm9.h>
#include <openssl/err.h>
#include <openssl/objects.h>

int main(int argc, char **argv)
{
	int ret = -1;
	char *prog = basename(argv[0]);
	FILE *mpk_fp = NULL;
	FILE *in_fp = NULL;
	FILE *out_fp  = NULL;
	SM9PublicParameters *mpk = NULL;
	SM9Ciphertext *cipher = NULL;
	unsigned char in[256];
	unsigned char out[1024];
	size_t inlen = sizeof(in);
	size_t outlen = sizeof(out);

	if (argc != 5) {
		printf("usage: %s <in-file> <out-file> <mpk-file> <id>\n", prog);
		return 0;
	}

	if (!(in_fp = fopen(argv[1], "r"))) {
		fprintf(stderr, "%s: can not open file '%s'\n", prog, argv[1]);
		goto end;
	}
	inlen = fread(in, 1, sizeof(in), in_fp);
	if (inlen >= sizeof(in)) {
		fprintf(stderr, "%s: data to long\n", prog);
		goto end;
	}

	if (!(mpk_fp = fopen(argv[3], "r"))) {
		fprintf(stderr, "%s: can not open file '%s'\n", prog, argv[2]);
		goto end;
	}
	if (!(mpk = d2i_SM9PublicParameters_fp(mpk_fp, NULL))) {
		ERR_print_errors_fp(stderr);
		fprintf(stderr, "%s: parse public parameters failed\n", prog);
		goto end;
	}

	if (!SM9_encrypt(NID_sm9encrypt_with_sm3_xor,
		in, inlen, out, &outlen,
		mpk, argv[4], strlen(argv[4]))) {
		ERR_print_errors_fp(stderr);
		goto end;
	}

	if (!(out_fp = fopen(argv[2], "w"))) {
		fprintf(stderr, "%s: can not open file\n", prog);
		goto end;
	}
	if (fwrite(out, 1, outlen, out_fp) < 0) {
		fprintf(stderr, "%s: output failed\n", prog);
		goto end;
	}

	ret = 0;

end:
	SM9PublicParameters_free(mpk);
	SM9Ciphertext_free(cipher);
	fclose(mpk_fp);
	fclose(in_fp);
	fclose(out_fp);
	return ret;
}
