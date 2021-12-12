from setuptools import setup, Extension

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
            extra_compile_args=[
                '-std=c++11',
                '-Wfatal-errors',
                '-Wall',
                '-Wextra',
                '-pedantic',
                '-O3',
                '-funroll-loops',
                '-march=native',
                '-fPIC',
                #'-fopenmp'
            ],
        )
    ],
    py_modules=['pycppmsa_utils'],
    install_requires=['numpy', 'torch']
)
