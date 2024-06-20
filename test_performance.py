# type: ignore

from functools import wraps
from timeit import repeat

from future_positional_only import fpo
from positional_defaults import defaults
from more_itertools import mark_ends


def decorated(func):
    """Comparison: only call function with args & kwargs."""

    @wraps(func)
    def wrapper(*args, **kwargs):
        return func(*args, **kwargs)

    return wrapper


def test_performance(*funcs, **repeat_kwargs):
    """Test the performance of the passed functions."""
    results = []
    for func in funcs:
        times = repeat(func, **repeat_kwargs)
        results.append((func.__name__, times))
    return results


def func0():
    """Function with no arguments, doing nothing."""
    pass


def func1(a1=None, /):
    """Function with one argument, doing nothing."""
    pass


def func5(a1=None, a2=None, a3=None, a4=None, a5=None, /):
    """Function with five arguments, doing nothing."""
    pass


func0_decorated = decorated(func0)
func0_decorated.__name__ = "func0_decorated"

func0_def = defaults(func0)
func0_def.__name__ = "func0_def"

func0_fpo = fpo(func0)
func0_fpo.__name__ = "func0_fpo"

func1_decorated = decorated(func1)
func1_decorated.__name__ = "func1_decorated"

func1_def = defaults(func1)
func1_def.__name__ = "func1_def"

func1_fpo = fpo(func1)
func1_fpo.__name__ = "func1_fpo"

func5_decorated = decorated(func5)
func5_decorated.__name__ = "func5_decorated"

func5_def = defaults(func5)
func5_def.__name__ = "func5_def"

func5_fpo = fpo(func5)
func5_fpo.__name__ = "func5_fpo"


results = {
    "func0": test_performance(func0, func0_decorated, func0_def, func0_fpo),
    "func1": test_performance(func1, func1_decorated, func1_def, func1_fpo),
    "func5": test_performance(func5, func5_decorated, func5_def, func5_fpo),
}
print("{")
for _, is_last, (label, res) in mark_ends(results.items()):
    print(f'"{label}": {{')
    for _, is_end, (name, times) in mark_ends(res):
        print(f'  "{name}": {sum(times)/len(times)}', end="\n" if is_end else ",\n")
    print("  }", end="\n" if is_last else ",\n")
print("}")
