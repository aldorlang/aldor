package foamj;

import org.junit.*;
import foamj.Word;

public class FoamTest {

    @Test
    public void testToJavaString() {
	Word w = new FoamJ.JavaObj<String>("hello");
	Word aldorString = Foam.javaStringToString(w);
	Assert.assertEquals("hello", Foam.stringToJavaString(aldorString).<String>toJavaObj());
    }
    
}
