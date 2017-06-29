/*
 * PostgreSQL type definitions for uniqueidentifiers.
 *
 * $Id: uniqueidentifier.c,v 1.7 2003/12/01 20:22:35 dimagog Exp $
 *
 * Copyright (C) 2001-2003 Dmitry G. Mastrukov.
 * Code for _uid_compare is based on libuuid 
 *					Copyright (C) 1996, 1997 Theodore Ts'o.
 *
 * Code for uniqueidentifier_text Copyright (C) 2002 Hussein Patni
 * Code for uniqueidentifier_from_text Copyright (C) 2003 David Bradford
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU 
 * Library General Public License.
 * %End-Header%
 *
 */

#include <string.h>
#include <uuid/uuid.h>
#include "postgres.h"
#include "libpq/pqformat.h"
#include "fmgr.h"
#include "access/hash.h"

PG_MODULE_MAGIC;

const char hex_a[256] ={
			'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',
			'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1',
			'2','2','2','2','2','2','2','2','2','2','2','2','2','2','2','2',
			'3','3','3','3','3','3','3','3','3','3','3','3','3','3','3','3',
			'4','4','4','4','4','4','4','4','4','4','4','4','4','4','4','4',
			'5','5','5','5','5','5','5','5','5','5','5','5','5','5','5','5',
			'6','6','6','6','6','6','6','6','6','6','6','6','6','6','6','6',
			'7','7','7','7','7','7','7','7','7','7','7','7','7','7','7','7',
			'8','8','8','8','8','8','8','8','8','8','8','8','8','8','8','8',
                        '9','9','9','9','9','9','9','9','9','9','9','9','9','9','9','9',
			'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',
			'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b',
			'c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c',
			'd','d','d','d','d','d','d','d','d','d','d','d','d','d','d','d',
			'e','e','e','e','e','e','e','e','e','e','e','e','e','e','e','e',
			'f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f' 
			};
const char hex_b[256] ={
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
			'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
			};
                     

/*
 *	Here is the internal storage format for uniqueidentifiers.
 */

typedef struct uniqueidentifier
{
	uuid_t		uid;
}			uniqueidentifier;


/*
 *	Various forward declarations:
 */

Datum		uniqueidentifier_in(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_out(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_recv(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_send(PG_FUNCTION_ARGS);

Datum		uniqueidentifier_lt(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_le(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_eq(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_ge(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_gt(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_ne(PG_FUNCTION_ARGS);

Datum		uniqueidentifier_cmp(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_hash(PG_FUNCTION_ARGS);

Datum		newid(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_text(PG_FUNCTION_ARGS);
Datum		uniqueidentifier_from_text(PG_FUNCTION_ARGS);

static int32	_uid_compare(uniqueidentifier *u1, uniqueidentifier *u2);

/*
 *	uniqueidentifier input function:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_in);
Datum
uniqueidentifier_in(PG_FUNCTION_ARGS)
{
	uniqueidentifier *result;
	char *in = PG_GETARG_CSTRING(0);
	
	if (strlen(in) != 36)
	{
		elog(ERROR, "uniqueidentifier_in: invalid uniqueidentifier \"%s\"", in);
		PG_RETURN_POINTER (NULL);
	}

	result = (uniqueidentifier *) palloc(sizeof(uniqueidentifier));

	if (uuid_parse(in, result->uid) != 0)
	{
		elog(ERROR, "uniqueidentifier_in: wrong uniqueidentifier \"%s\"", in);
		PG_RETURN_POINTER (NULL);
	}
	PG_RETURN_POINTER (result);
}

PG_FUNCTION_INFO_V1(uniqueidentifier_recv);
Datum
uniqueidentifier_recv(PG_FUNCTION_ARGS)
{

	StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
	uniqueidentifier * result;
	char * byteUuid;

	byteUuid = (char *) palloc(buf->len + 1);
	result = (uniqueidentifier *) palloc(sizeof(uniqueidentifier));
	memcpy(byteUuid,pq_getmsgbytes(buf, buf->len), buf->len);
	byteUuid[36] = 0;

	if (uuid_parse(byteUuid, result->uid) != 0)
	{
		elog(ERROR, "uniqueidentifier_recv: wrong uniqueidentifier \"%s\"", byteUuid);
		PG_RETURN_POINTER (NULL);
	}
	PG_RETURN_POINTER(result);

}



/*
 *	uniqueidentifier output function:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_out);
Datum
uniqueidentifier_out(PG_FUNCTION_ARGS)
{
	char *result;
	uniqueidentifier *in = (uniqueidentifier *) PG_GETARG_POINTER(0);

	if (in == NULL)
		PG_RETURN_CSTRING (NULL);

	result = (char *) palloc(37);

	result[0] = '\0';

	uuid_unparse(in->uid, result);

	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(uniqueidentifier_send);
Datum
uniqueidentifier_send(PG_FUNCTION_ARGS)
{

	StringInfoData buffer;
	char *result;
	uniqueidentifier *in = (uniqueidentifier *) PG_GETARG_POINTER(0);

	if (in == NULL)
		PG_RETURN_BYTEA_P(NULL);

	result = (char *) palloc(37);

	result[0] = '\0';

	uuid_unparse(in->uid, result);

	pq_begintypsend(&buffer);
	pq_sendbytes(&buffer, (char *) result, 36);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));  

}




/*
 *	uniqueidentifier to text conversion function:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_text);
Datum
uniqueidentifier_text(PG_FUNCTION_ARGS)
{
	char *s;
	uniqueidentifier *val = (uniqueidentifier *) PG_GETARG_POINTER(0);
	int len;
	text *result;

	s = (char *) palloc(37);
	s[0] = '\0';
	uuid_unparse(val->uid, s);

	len = strlen(s);

	result = (text *) palloc(VARHDRSZ + len);
        SET_VARSIZE(result, len + VARHDRSZ); 
	memcpy(VARDATA(result),s,len);
	pfree(s);

	PG_RETURN_TEXT_P(result);
}

/*
 *	text to uniqueidentifier conversion function:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_from_text);
Datum
uniqueidentifier_from_text(PG_FUNCTION_ARGS)
{
	uniqueidentifier *result;
	char *in  = NULL;
	int len = 0;
	text *data = PG_GETARG_TEXT_P(0);
	
	len = (VARSIZE(data)-VARHDRSZ);
	if (len != 36)
	{
		elog(ERROR, "uniqueidentifier_from_text: invalid uniqueidentifier length: \"%d\"", len);
		PG_RETURN_POINTER (NULL);
	}

	in = (char*)palloc( sizeof(char)*(len+1) );
	if( !in )
	{
	    elog(ERROR, "not enough memory in uniqueidentifier_from_text");
	    PG_RETURN_POINTER(NULL);
	}
	memset(in, 0, len+1);
	memcpy(in, VARDATA(data), len);
	in[len+1]='\0';

	result = (uniqueidentifier *) palloc(sizeof(uniqueidentifier));
	if (uuid_parse(in, result->uid) != 0)
	{
		elog(ERROR, "uniqueidentifier_from_text: wrong uniqueidentifier \"%s\"", in);
		pfree(in);
		PG_RETURN_POINTER (NULL);
	}
	pfree(in);
	PG_RETURN_POINTER (result);
}

/*
 *	Boolean tests:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_lt);
Datum
uniqueidentifier_lt(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL (_uid_compare(u1, u2) < 0);
};

PG_FUNCTION_INFO_V1(uniqueidentifier_le);
Datum
uniqueidentifier_le(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL (_uid_compare(u1, u2) <= 0);
};

PG_FUNCTION_INFO_V1(uniqueidentifier_eq);
Datum
uniqueidentifier_eq(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	
	if ((u1 == NULL) || (u2 == NULL)) {
	    if ((u1 == NULL) && (u2 == NULL)) 
		PG_RETURN_BOOL (true);
	    else
		PG_RETURN_BOOL (false);
	}		
	    
	if (memcmp(u1->uid, u2->uid, 16) == 0)
	    PG_RETURN_BOOL (true);
	else
	    PG_RETURN_BOOL (false);    
};

PG_FUNCTION_INFO_V1(uniqueidentifier_ge);
Datum
uniqueidentifier_ge(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL (_uid_compare(u1, u2) >= 0);
};

PG_FUNCTION_INFO_V1(uniqueidentifier_gt);
Datum
uniqueidentifier_gt(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL (_uid_compare(u1, u2) > 0);
};

PG_FUNCTION_INFO_V1(uniqueidentifier_ne);
Datum
uniqueidentifier_ne(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	if ((u1 == NULL) || (u2 == NULL)) {
	    if ((u1 == NULL) && (u2 == NULL)) 
		PG_RETURN_BOOL (false);
	    else
		PG_RETURN_BOOL (true);
	}		
	if (memcmp(u1->uid, u2->uid, 16) == 0)
	    PG_RETURN_BOOL (false);
	else
	    PG_RETURN_BOOL (true);    
};


/*
 *	Comparison function for B-tree sorting:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_cmp);
Datum
uniqueidentifier_cmp(PG_FUNCTION_ARGS)
{
	uniqueidentifier *u1 = (uniqueidentifier *) PG_GETARG_POINTER(0);
	uniqueidentifier *u2 = (uniqueidentifier *) PG_GETARG_POINTER(1);
	PG_RETURN_INT32 (_uid_compare(u1, u2));
};


/*
 *	Hash function for hash sorting:
 */

PG_FUNCTION_INFO_V1(uniqueidentifier_hash);
Datum
uniqueidentifier_hash(PG_FUNCTION_ARGS)
{
	uniqueidentifier *key = (uniqueidentifier *) PG_GETARG_POINTER(0);
	return hash_any ((unsigned char *) key, sizeof(uniqueidentifier));
};


/*
 *	New uniqueidentifier:
 */

PG_FUNCTION_INFO_V1(newid);
Datum
newid(PG_FUNCTION_ARGS)
{
	uniqueidentifier *result = (uniqueidentifier *) palloc(sizeof(uniqueidentifier));

	uuid_generate(result->uid);
	
	PG_RETURN_POINTER(result);    
};


/*
 *	Internal compare function:
 */

#define UUCMP(u1,u2) if (u1 != u2) return((u1 < u2) ? -1 : 1);

static int32
_uid_compare(uniqueidentifier *u1, uniqueidentifier *u2)
{
	uint8	*ptr1 = u1->uid;
	uint32 time_low1;
	uint16 time_mid1;
	uint16 time_hi_and_version1;
	uint16 clock_seq1;
	uint8	*ptr2 = u2->uid;
	uint32 time_low2;
	uint16 time_mid2;
	uint16 time_hi_and_version2;
	uint16 clock_seq2;
	
	if ((u1 == NULL) || (u2 == NULL)) {
	    if ((u1 == NULL) && (u2 == NULL)) 
		return 0;
	    if (u1 == NULL)
		return 1;
	    else
		return -1;	
	}		

	if(memcmp(u1->uid, u2->uid, 16) == 0)
	    return 0;

	time_low1 = *ptr1++;
	time_low1 = (time_low1 << 8) | *ptr1++;
	time_low1 = (time_low1 << 8) | *ptr1++;
	time_low1 = (time_low1 << 8) | *ptr1++;
	time_low2 = *ptr2++;
	time_low2 = (time_low2 << 8) | *ptr2++;
	time_low2 = (time_low2 << 8) | *ptr2++;
	time_low2 = (time_low2 << 8) | *ptr2++;
	UUCMP(time_low1, time_low2);

	time_mid1 = *ptr1++;
	time_mid1 = (time_mid1 << 8) | *ptr1++;
	time_mid2 = *ptr2++;
	time_mid2 = (time_mid2 << 8) | *ptr2++;
	UUCMP(time_mid1, time_mid2);
	
	time_hi_and_version1 = *ptr1++;
	time_hi_and_version1 = (time_hi_and_version1 << 8) | *ptr1++;
	time_hi_and_version2 = *ptr2++;
	time_hi_and_version2 = (time_hi_and_version2 << 8) | *ptr2++;
	UUCMP(time_hi_and_version1, time_hi_and_version2);


	clock_seq1 = *ptr1++;
	clock_seq1 = (clock_seq1 << 8) | *ptr1++;
	clock_seq2 = *ptr2++;
	clock_seq2 = (clock_seq2 << 8) | *ptr2++;
	UUCMP(clock_seq1, clock_seq2);


	return memcmp(ptr1, ptr2, 6);
}

