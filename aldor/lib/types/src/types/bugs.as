
-- Workaround: Use '==>' instead of '==' for Rep
StreamOp: with
    op: (%, Integer) -> Generator Integer
== add
    Rep == Integer -> Generator Integer

    op(o: %, n: Integer): Generator Integer ==
        f := rep o
	f n
