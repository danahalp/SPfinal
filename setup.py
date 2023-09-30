from setuptools import setup, find_packages, Extension

setup(name="symnmf",
      version="1.0.0",
      description="desc",
      author="<your name>",
      author_email="a@b.com",
      ext_modules=[Extension("symnmfmodule",["symnmfmodule.c"])])

module = Extension(
    'symnmfmodule',  # The name of your module
    sources=['symnmf.c'],  # List of your C source files
)