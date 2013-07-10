-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#if TestErrorsToo
#pile

-- These lines have tabs in them but the errors should still point to the ")".

--a(x)  <Tab>== )
--b(x)   <Tab>== )
--c(x)    <Tab>== )
--d(x)     <Tab>== )

a(x)  	== )
b(x)   	== )
c(x)    	== )
d(x)     	== )

--A
--<Tab>e(x)  <Tab>== )
--<Tab>f(x)   <Tab>== )
--<Tab>g(x)    <Tab>== )
--<Tab>h(x)     <Tab>== )

A
	e(x)  	== )
	f(x)   	== )
	g(x)    	== )
	h(x)     	== )

--B
--        i(x)  <Tab>== )
--        j(x)   <Tab>== )
--        k(x)    <Tab>== )
--        l(x)     <Tab>== )

B
        i(x)  	== )
        j(x)   	== )
        k(x)    	== )
        l(x)     	== )
#endif
