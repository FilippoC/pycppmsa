from setuptools import setup, Extension

setup(
    name="pycppmsa",
    version="0.1",
    include_dirs=["./"],
    ext_modules=[
        Extension(
            "pycppmsa",
            sources=[
                "pycppmsa.cpp",
                "ad3_msa.cpp"
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
    ]
)

setup(name='pycppmsa_utils',
      version='0.1',
      py_modules=['pycppmsa_utils'],
      scripts=['pycppmsa_utils.py'],
      license='',
      install_requires=['pycppmsa'],
)