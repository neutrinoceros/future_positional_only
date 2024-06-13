# license: MIT
'''Set defaults for any positional-only parameter.'''
__all__ = ['fpo']

from functools import partial, update_wrapper
from inspect import Signature
from typing import Any, Callable, TypeVar, Union, overload

from _future_positional_only import wrap

F = TypeVar('F', bound=Callable[..., Any])


@overload
def fpo(func: None, /, *names: str) -> Callable[[F], F]:
    ...


@overload
def fpo(func: F, /, *names: str) -> F:
    ...


def fpo(func: Union[F, None] = None, /, *, names: tuple[str, ...] = ()) -> Any:
    if func is None:
        return partial(fpo, names=names)

    if not callable(func):
        raise TypeError('not a callable')

    sig = Signature.from_callable(func)
    wrapper = wrap(func, names, sig)
    update_wrapper(wrapper, func)
    return wrapper
