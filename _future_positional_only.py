from typing import Any, Callable, Tuple
import warnings
from inspect import Signature


def wrap(
    wrapped: Callable[..., Any],
    names: tuple[str, ...],
    signature: Signature,
) -> Callable[..., Any]:
    def wrapper(*args, **kwargs):
        if deprecated_kwargs := set(kwargs).intersection(names):
            params = list(signature.parameters)
            pos2kw: list[tuple[int, str]] = sorted(
                (params.index(name), name) for name in deprecated_kwargs
            )

            deprecated_kw = [kw for _, kw in pos2kw]
            if len(deprecated_kw) > 1:
                names_str = str(deprecated_kw)
                s = "s"
                arguments = " arguments"
                respectively = ", respectively"
                pos = [pos for pos, _ in pos2kw]
                pronoun = "them"
            else:
                names_str = repr(deprecated_kw[0])
                s = arguments = respectively = ""
                pos, _ = pos2kw[0]
                pronoun = "it"

            msg = (
                f"Passing {names_str}{arguments} as keyword{s} (at position{s} {pos}{respectively}) "
                "is deprecated and will stop working in a future release. "
                f"Pass {pronoun} positionally to supress this warning."
            )
            warnings.warn(msg, FutureWarning, stacklevel=2)
        return wrapped(*args, **kwargs)

    return wrapper
