PROGRAM main( input ) ;

VAR x, y, a, b, c, d, e, f, g, h, i : INTEGER ;

BEGIN

   { binary operators }
   a := 10 + 15;
   print a;

   b := 30 - 10;
   print b;

   c := 5 * 5;
   print c;
   
   d := 10 / 2;
   print d;
   
   e:= 12 MOD 5;
   print e;

   { relational operators }
   f := 30 < 50;
   print f;

   g := 30 > 50;
   print g;

   h := 10 = 10;
   print h;

   i := 10 <> 10;
   print i;

   x := 5;

   WHILE ( (x < 10) AND 1 ) DO
   BEGIN
      IF ( x = 9 ) THEN y := 511 ELSE y := 42;
      x := x + 1;
      print x
   END;

   print y

END.
