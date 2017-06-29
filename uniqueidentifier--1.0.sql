--
--	PostgreSQL code for uniqueidentifierss.
--
--	$Id: uniqueidentifier.sql.in,v 1.8 2003/12/02 12:45:20 dimagog Exp $
--

--BEGIN TRANSACTION;

--
--	Input and output functions and the type itself:
--

CREATE OR REPLACE FUNCTION uniqueidentifier_in(cstring)
	returns uniqueidentifier
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_out(uniqueidentifier)
	returns cstring
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_recv(internal)
	returns uniqueidentifier
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_send(uniqueidentifier)
	returns bytea
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

create type uniqueidentifier (
	internallength = 16,
	input = uniqueidentifier_in,
	output = uniqueidentifier_out,
        send = uniqueidentifier_send,
        receive = uniqueidentifier_recv
);

--
--	The various boolean tests:
--

CREATE OR REPLACE FUNCTION uniqueidentifier_lt(uniqueidentifier, uniqueidentifier)
	returns bool
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_le(uniqueidentifier, uniqueidentifier)
	returns bool
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_eq(uniqueidentifier, uniqueidentifier)
        returns bool
        as 'MODULE_PATHNAME'
        language 'c' IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION uniqueidentifier_ge(uniqueidentifier, uniqueidentifier)
	returns bool
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_gt(uniqueidentifier, uniqueidentifier)
	returns bool
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION uniqueidentifier_ne(uniqueidentifier, uniqueidentifier)
	returns bool
	as 'MODULE_PATHNAME'
	language 'c' IMMUTABLE STRICT;

--
--	Now the operators.  Note how some of the parameters to some
--	of the 'create operator' commands are commented out.  This
--	is because they reference as yet undefined operators, and
--	will be implicitly defined when those are, further down.
--

create operator < (
	leftarg = uniqueidentifier,
	rightarg = uniqueidentifier,
	commutator = >,
	negator = >=,
	procedure = uniqueidentifier_lt
);

create operator <= (
	leftarg = uniqueidentifier,
	rightarg = uniqueidentifier,
	commutator = >=,
	negator = >,
	procedure = uniqueidentifier_le
);

create operator = (
	leftarg = uniqueidentifier,
	rightarg = uniqueidentifier,
	commutator = =,
	negator = <>,
	procedure = uniqueidentifier_eq,
	hashes,
	restrict = eqsel,
	join = eqjoinsel,
	sort1 = <,
	sort2 = <
);

create operator >= (
	leftarg = uniqueidentifier,
	rightarg = uniqueidentifier,
	commutator = <=,
	negator = <,
	procedure = uniqueidentifier_ge
);

create operator > (
	leftarg = uniqueidentifier,
	rightarg = uniqueidentifier,
	commutator = <,
	negator = <=,
	procedure = uniqueidentifier_gt
);

create operator <> (
	leftarg = uniqueidentifier,
	rightarg = uniqueidentifier,
	negator = =,
	procedure = uniqueidentifier_ne
);

--
--	Create new uniqueidentifier:
--

CREATE OR REPLACE FUNCTION newid()
	returns uniqueidentifier
	as 'MODULE_PATHNAME'
	language 'c' VOLATILE;

--
--	Convert from uniqueidentifier to text:
--

CREATE OR REPLACE FUNCTION text(uniqueidentifier)
	returns text
	as 'MODULE_PATHNAME', 'uniqueidentifier_text'
	language 'c' IMMUTABLE;
	
--
--	Convert from text to uniqueidentifier:
--

CREATE OR REPLACE FUNCTION newid(text)
	returns uniqueidentifier
	as 'MODULE_PATHNAME', 'uniqueidentifier_from_text'
	language 'c' IMMUTABLE STRICT;
	
--
--	Create cast from uniqueidentifier to text:
--

create cast (uniqueidentifier AS text)
	with function text(uniqueidentifier)
	as implicit;

--
--	Create cast from text to uniqueidentifier:
--

create cast (text AS uniqueidentifier)
	with function newid(text)
	as implicit;

---
---	Index related stuff
---

CREATE OR REPLACE FUNCTION uniqueidentifier_cmp(uniqueidentifier, uniqueidentifier)
	RETURNS int4
	AS 'MODULE_PATHNAME'
	LANGUAGE 'C' IMMUTABLE STRICT;

CREATE OPERATOR CLASS uniqueidentifier_ops_btree
    DEFAULT FOR TYPE uniqueidentifier USING btree AS
	OPERATOR	1	<,
	OPERATOR	2	<=,
	OPERATOR	3	=,
	OPERATOR	4	>=,
	OPERATOR	5	>,
	FUNCTION	1	uniqueidentifier_cmp(uniqueidentifier, uniqueidentifier);
    

CREATE OR REPLACE FUNCTION uniqueidentifier_hash(uniqueidentifier)
	RETURNS int4
	AS 'MODULE_PATHNAME'
	LANGUAGE 'C' IMMUTABLE;

CREATE OPERATOR CLASS uniqueidentifier_ops_hash
    DEFAULT FOR TYPE uniqueidentifier USING hash AS
	OPERATOR	1	=,
	FUNCTION	1	uniqueidentifier_hash(uniqueidentifier);


CREATE OR REPLACE FUNCTION textuniqueidentifiercat(text, uniqueidentifier)
  RETURNS text AS
'select $1 || cast( $2 as pg_catalog.text)'
  LANGUAGE 'sql' IMMUTABLE STRICT
  COST 1;
ALTER FUNCTION textuniqueidentifiercat(text, uniqueidentifier) OWNER TO postgres;
COMMENT ON FUNCTION textuniqueidentifiercat(text, uniqueidentifier) IS 'concatenate';

CREATE OR REPLACE FUNCTION uniqueidentifiertextcat(uniqueidentifier, text)
  RETURNS text AS
'select cast ( $1 as pg_catalog.text) || $2'
  LANGUAGE 'sql' IMMUTABLE STRICT
  COST 1;
ALTER FUNCTION uniqueidentifiertextcat(uniqueidentifier, text) OWNER TO postgres;
COMMENT ON FUNCTION uniqueidentifiertextcat(uniqueidentifier, text) IS 'concatenate';

CREATE OPERATOR ||(
  PROCEDURE = public.textuniqueidentifiercat,
  LEFTARG = "text",
  RIGHTARG = uniqueidentifier);

CREATE OPERATOR ||(
  PROCEDURE = public.uniqueidentifiertextcat,
  LEFTARG = uniqueidentifier,
  RIGHTARG = "text");

CREATE OR REPLACE FUNCTION quote_literal(uniqueidentifier)
  RETURNS text AS
'select pg_catalog.quote_literal(cast( $1 as pg_catalog.text))'
  LANGUAGE 'sql' VOLATILE STRICT
  COST 1;
ALTER FUNCTION quote_literal(uniqueidentifier) OWNER TO postgres;
COMMENT ON FUNCTION quote_literal(uniqueidentifier) IS 'quote a data value for usage in a querystring';
        

--END TRANSACTION;
	
--
--	eof
--
	
