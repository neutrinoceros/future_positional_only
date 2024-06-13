from future_positional_only import fpo
import pytest


def noop(a, b, c, d):
    return


depr1 = fpo(kw=["a"])(noop)
depr2 = fpo(kw=["a", "b"])(noop)
depr4 = fpo(kw=["a", "a", "c", "d"])(noop)


@pytest.mark.parametrize("func", [depr1, depr2, depr4])
def test_no_warn(func):
    func(1, 2, 3, 4)


@pytest.mark.parametrize(
    "func, args, kwargs, match",
    [
        pytest.param(
            depr1,
            (),
            {"a": 1, "b": 2, "c": 3, "d": 4},
            r"Passing 'a' as keyword \(at position 0\) is deprecated",
            id="1 deprecation, 1 warn",
        ),
        pytest.param(
            depr2,
            (1,),
            {"b": 2, "c": 3, "d": 4},
            r"Passing 'b' as keyword \(at position 1\) is deprecated",
            id="2 deprecation, 1 warn",
        ),
        pytest.param(
            depr2,
            (),
            {"a": 1, "b": 2, "c": 3, "d": 4},
            r"Passing \['a', 'b'\] arguments as keywords "
            r"\(at positions \[0, 1\] respectively\) is deprecated",
            id="2 deprecations, 2 warns",
        ),
        pytest.param(
            depr4,
            (),
            {"a": 1, "b": 2, "c": 3, "d": 4},
            r"Passing \['a', 'b', 'c', 'd'\] arguments as keywords "
            r"\(at positions \[0, 1, 2, 3\] respectively\) is deprecated",
            id="4 deprecation, 4 warns",
        ),
        pytest.param(
            depr4,
            (1,),
            {"b": 2, "c": 3, "d": 4},
            r"Passing \['b', 'c', 'd'\] arguments as keywords "
            r"\(at positions \[1, 2, 3\] respectively\) is deprecated",
            id="4 deprecation, 3 warns",
        ),
        pytest.param(
            depr4,
            (1, 2),
            {"c": 3, "d": 4},
            r"Passing \['c', 'd'\] arguments as keywords "
            r"\(at positions \[0, 1\] respectively\) is deprecated",
            id="4 deprecation, 2 warns",
        ),
        pytest.param(
            depr4,
            (1, 2, 3),
            {"d": 4},
            r"Passing 'd' as keyword \(at position 3\) is deprecated",
            id="4 deprecation, 1 warn",
        ),
    ],
)
def test_warn(func, args, kwargs, match):
    with pytest.warns(FutureWarning, match=match):
        func(*args, **kwargs)
