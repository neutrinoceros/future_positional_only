# type: ignore

import sys
import os
from setuptools import setup, Extension

py_modules = ['future_positional_only', '_future_positional_only']
ext_modules = []

if 'FPO_PY_IMPL' not in os.environ:
    py_modules.remove('_future_positional_only')
    ext_modules.append(Extension('_future_positional_only',
                                 ['_future_positional_only.c'],
                                 extra_compile_args=["-Werror"]), #, "-Wall", "-Wextra"]),
                       )

setup(py_modules=py_modules,
      ext_modules=ext_modules)
