/* contrib/uniqueidentifier/uniqueidentifier--unpackaged--1.0.sql */

ALTER EXTENSION uniqueidentifier ADD type uniqueidentifier;
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_in(cstring);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_out(uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_recv(internal);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_send(uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_lt(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_le(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_eq(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_ge(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_gt(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_ne(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function newid();
ALTER EXTENSION uniqueidentifier ADD function text(uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function newid(text);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_cmp(uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifier_hash(uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function textuniqueidentifiercat(text, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD function uniqueidentifiertextcat(uniqueidentifier, text);
ALTER EXTENSION uniqueidentifier ADD function quote_literal(uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator < (uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator <= (uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator = (uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator >= (uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator > (uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator <> (uniqueidentifier, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD cast (uniqueidentifier AS text);
ALTER EXTENSION uniqueidentifier ADD cast (text AS uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator || (text, uniqueidentifier);
ALTER EXTENSION uniqueidentifier ADD operator || (uniqueidentifier, text);

ALTER EXTENSION uniqueidentifier ADD operator family uniqueidentifier_ops_btree USING btree;
ALTER EXTENSION uniqueidentifier ADD operator class uniqueidentifier_ops_btree USING btree;

ALTER EXTENSION uniqueidentifier ADD operator family uniqueidentifier_ops_hash USING hash;
ALTER EXTENSION uniqueidentifier ADD operator class uniqueidentifier_ops_hash USING hash;

