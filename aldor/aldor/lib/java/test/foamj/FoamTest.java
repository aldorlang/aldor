package foamj;

import org.junit.*;
import foamj.Word;

import java.math.BigInteger;

public class FoamTest {

    @Test
    public void testToJavaString() {
	Word aldorString = Foam.javaStringToString("hello");
	Assert.assertEquals("hello", Foam.stringToJavaString(aldorString));
    }

    @Test
    public void testMakeBigInt() {
	char[] arr = "1234".toCharArray();
	BigInteger number = Foam.arrToBInt(arr);
	Assert.assertEquals(new BigInteger("1234"), number);
    }

    @Test
    public void testMakeBigInt2() {
	char[] arr = "1234\0foo".toCharArray();
	BigInteger number = Foam.arrToBInt(arr);
	Assert.assertEquals(new BigInteger("1234"), number);
    }

    @Test
    public void testMakeDFlo() {
	char[] arr = "1234.5\0foo\0bar".toCharArray();
	Double number = Foam.arrToDFlo(arr);
	Assert.assertEquals(new Double("1234.5"), number);
    }
    
}
