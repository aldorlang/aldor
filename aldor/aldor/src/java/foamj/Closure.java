
class Closure {
    Env env;
    Fn fn;

    Value call(Value[] vals) {
	return fn.ocall(env, vals)
    }
}

interface Fn {
    Value ocall(Env env, Value vals);
}

abstract class FnWxWWW implements Fn {
    abstract Word call(Env env, Word b, Word c);

    Value ocall(Env env, Value[] vals) {
	Word r = call(env, vals[0].asWord(), vals[1].asWord());
    }
}

class Record {
    Value[] arr;
    
    Record(Format fmt) {
    }

    Word getWord(int idx, String name) {
	return arr[idx].asWord();
    }
}

class Record {
    Value[] arr;
    
    Record(Format fmt) {
	arr = new Value[fmt.size()];
    }

    Word getWord(int idx, String name) {
	return arr[idx].asWord();
    }
}

class Format {
    int size;
    
    int size() { return size; }
}

interface Value {
    Word asWord();
    int  toInteger();
    double toDouble();
    Array  toArray();
    Record toRecord();
}

abstract class AbstractValue {
    Word asWord() { throw new CastException(); }
    int  toInteger() { throw new CastException(); }
    double toDouble() { throw new CastException(); }
    Array  toArray() { throw new CastException(); }
    Record toRecord() { throw new CastException(); }
}

interface Word {
    Word asWord();
}

class SInt extends AbstractValue implements Value, Word {
    int value;
    Word asWord() { return this; }
    int  toInteger() { return value; }
}

x = ccall(a, b, c);

x = ccallWxWWW(a, b, c);

static Word callWxWWW(Closure a, Word b, Word c) {
    Value[] args = {b,c};
    Value r = a.call(args);
    return r.asWord();
}

clos c = new Closure(fn, env);
