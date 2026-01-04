import org.junit.*;
import aldorcode.jthrow;
import aldor.stuff.ExnThrow;
import foamj.*;

public class JThrowTest {

    @Test
    public void testSimpleThrow() {
	FoamContext ctxt = new FoamContext();
        Clos fn = ctxt.createLoadFn("jthrow");
	fn.call();

	FoamHelper.setContext(ctxt);

	try {
	    ExnThrow.fn(22);
	    Assert.fail();
	}
	catch (FoamUserException e) {
	}
    }

    @Test
    public void testConditional() {
	FoamContext ctxt = new FoamContext();
        Clos fn = ctxt.createLoadFn("jthrow");
	fn.call();

	FoamHelper.setContext(ctxt);

	Assert.assertEquals(1, ExnThrow.fn2(1));
	
	try {
	    ExnThrow.fn2(0);
	    Assert.fail();
	}
	catch (FoamUserException e) {
	}
    }

    @Test
    public void testLoop() {
	FoamContext ctxt = new FoamContext();
        Clos fn = ctxt.createLoadFn("jthrow");
	fn.call();

	FoamHelper.setContext(ctxt);

	Assert.assertEquals(-1, ExnThrow.fn3(-1));
	
	try {
	    int n = ExnThrow.fn3(10);
	    Assert.fail();
	}
	catch (FoamUserException e) {
	}
    }
}
