-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -laxllib
--> testgen y -laxllib

#library langlib "lang.ao"
import from langlib;

export Boolean: Type;

Field : Category == with;

PType ==> with {
       Field@Category;
       Field: % -> %;
       x: %;
};

P(R: Type) : PType == add pretend PType;
