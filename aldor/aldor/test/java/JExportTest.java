import org.junit.*;
import aldorcode.jexport;
import aldor.stuff.Foo;
import foamj.FoamContext;
import foamj.FoamHelper;
import foamj.FoamClass;
import foamj.Clos;

public class JExportTest {

    @Test
    public void testJExport() {
	FoamContext ctxt = new FoamContext();
        Clos fn = ctxt.createLoadFn("jexport");
	fn.call();

	FoamHelper.setContext(ctxt);
	Foo foo = Foo.wrap(10);
	Assert.assertEquals(20, foo.plus(foo).unwrap());
	Assert.assertEquals("hello", Foo.something());

	Assert.assertEquals(1, Foo.fromBar(Foo.wrap(1).toBar()).unwrap());
	Assert.assertEquals(22, Foo.fromString("hello").unwrap());
    }
}
