Omitting the typename identifier for templated functions  {#omittype}
========================================================

There are cases where the compiler can infer the type to use from the input arguments. 
For example, consider: 

	void PostMessage<T>(string msg_name, T msg);
	
The second argument can be any datatype as long it matches the typename identifier 
`<T>`. In this case, the compiler can infer which type to use based on the type of the 
second argument. We are therefore free to omit the typename identifier. 

	location_message msg;

	// the following two calls are equivalent
	FLAME.PostMessage("location", msg);
	FLAME.PostMessage<location_message_t>("location", msg);
	
The same goes for `SetMem` which is define as: 

	void SetMem<T>(string var_name, T value);
	
Consider the following example, where `x` has been declared as a memory variable of 
type `double`: 

	double val = 0.1;
	FLAME.SetMem("x", value);  // OK. "value" is of type double
	FLAME.SetMem<double>("x", value);  // OK. equivalent to above

	// Literal values work as well, but not obvious 
	// (we recommend that you DO NOT omit type identifiers when using literal values)
	FLAME.SetMem<double>("x", 10.0);  // OK
	FLAME.SetMem<double>("x", 10);  // OK but not ideal. literal 10 is an int, so will be cast to double

	FLAME.SetMem("x", 10.0);  // OK. SetMem<double>() inferred. Matches definition of "x"
	FLAME.SetMem("x", 10);  // NOT OK! SetMem<int>() inferred. Will cause runtime failures.
	

When it does not work
---------------------

There are other FLAME-II functions where you cannot omit the identifiers. Take for 
example: 

	T GetMem<T>(string var_name);

The only argument is `var_name` which does not give any indication of what type to use. 
The compiler cannot infer T from it and you therefore have to always specify the type, 
e.g. `GetMem<int>("id")`.

Note that the return type cannot be used to infer the typename. It in fact works the 
other way round where the typename identifier determines which specialised function to 
use which then determines the return type.

The same goes for `iterator.GetMessage<T>()` which does not have any input arguments 
and the type cannot therefor be deduced automatically. 