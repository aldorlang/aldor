package aldor.test;

public class ExceptionExample {
    int value;

    public ExceptionExample(int n) {
	this.value = n;
    }

    public int value() {
	return value;
    }

    public static ExceptionExample random() {
	return new ExceptionExample(22); // 22 is random. officially.
    }

    public void decrement() {
	this.value--;
    }

    static class DubiousException extends Exception {
    }
}
