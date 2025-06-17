import os
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

__version__ = '0.1.0'


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path"""
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


ext_modules = [
    Extension(
        'speedquant.core',
        ['speedquant_lib/bridge/core_bindings.cpp'],
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
            'core/include',
        ],
        language='c++'
    ),
]


# As of Python 3.6, C++17 is required by default
class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
    }
    l_opts = {
        'msvc': [],
        'unix': [],
    }

    if sys.platform == 'darwin':
        darwin_opts = ['-stdlib=libc++', '-mmacosx-version-min=10.14']
        c_opts['unix'] += darwin_opts
        l_opts['unix'] += darwin_opts

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        link_opts = self.l_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append('-std=c++17')
            if self.compiler.has_flag('-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts
        build_ext.build_extensions(self)


setup(
    name='speedquant',
    version=__version__,
    author='SpeedQuant Team',
    author_email='info@speedquant.example',
    url='https://github.com/yourusername/speedquant',
    description='High-performance AI-driven quantitative trading system',
    long_description='',
    packages=find_packages(exclude=['tests']),
    ext_modules=ext_modules,
    install_requires=[
        'numpy>=1.20.0',
        'pandas>=1.3.0',
        'torch>=1.10.0',
        'fastapi>=0.68.0',
        'uvicorn>=0.15.0',
        'redis>=4.0.0',
    ],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
    python_requires='>=3.10',
)
