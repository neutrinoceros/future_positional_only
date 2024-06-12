# type: ignore

import sys
import os
from setuptools import setup, Extension

py_modules = ['future_positional_only', '_future_positional_only']
ext_modules = []

use_extension = False

if '--compile-extension' in sys.argv:
    sys.argv.remove('--compile-extension')
    use_extension = True
elif os.getenv('COMPILE_FPO', None) is not None:
    use_extension = True

if use_extension:
    py_modules.remove('_future_positional_only')
    ext_modules.append(Extension('_future_positional_only',
                                 ['_future_positional_only.c']))

setup(py_modules=py_modules,
      ext_modules=ext_modules)
