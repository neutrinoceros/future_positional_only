positional_defaults
===================

**Python package to set defaults for any positional-only parameter**

This tiny Python package contains a decorator `@fpo` which lets you
specify default arguments for any positional-only parameter, no matter where
it appears in the argument list.

Installation
------------

    COMPILE_FPO=1 python -m pip install -e .

Usage
-----

Use the `@fpo` decorator to set default values for future-positional-only
parameters (i.e. those before the positional-only indicator `/`):

```py
from future_positional_only import fpo

@fpo({'start': 0})
def myrange(start, stop, step=1):
    ...
```

Signatures
----------

Left-defaulted functions come with the correct signature:

```py
>>> from inspect import signature
>>> signature(myrange)
<DefaultsSignature (start=0, stop, /, step=1)>
>>> signature(greet)
<DefaultsSignature (greeting='Welcome', prefix='Mrs', forename='Alice', surname, /, suffix='Esq')>
```

These show up correctly in the usual places such as `help()`:

```py
>>> help(myrange)

Help on function myrange:

myrange(start=0, stop, /, step=1)

>>> help(greet)

Help on function greet:

greet(greeting='Welcome', prefix='Mrs', forename='Alice', surname, /, suffix='Esq')

```

Performance
-----------

When the package is compiled as a native extension (which is what pip will
deliver in the majority of cases), functions with positional defaults have
comparable performance with respect to their undecorated functions.
