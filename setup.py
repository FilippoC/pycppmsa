import sys
from setuptools import setup, Extension

extra_compile_args = [
    '-std=c++11',
    '-Wall',
    '-O3',
    # '-fopenmp'
]

if sys.platform != "win32":
    extra_compile_args.extend(
        [
            '-Wfatal-errors',
            '-Wextra',
            '-pedantic',
            '-funroll-loops',
            '-march=native',
            '-fPIC',
            # '-fopenmp',
        ]
    )




setup(
    name="pycppmsa",
    version="0.1",
    ext_modules=[
        Extension(
            "pycppmsa",
            sources=[
                "pycppmsa/pycppmsa.cpp",
                "pycppmsa/ad3_msa.cpp"
            ],
            language='c++',
            #extra_link_args=["-fopenmp"],
            extra_compile_args=extra_compile_args,
        )
    ],
    py_modules=['pycppmsa_utils'],
    install_requires=['numpy', 'torch']
)
