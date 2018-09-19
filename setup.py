import os
import re
import sys
import platform
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion
from sphinx.setup_command import BuildDoc

# Proj settings
name = 'libtempo_py'
version = '1.0.0'
release = '1.0.0'

# Cmake settings from env
cmake_extra_args = []
if os.path.exists('.env'):
    with open('.env', 'r') as f:
        lines = f.readlines()
    for line in lines:
        line = line.replace('\n', '').replace('"', '')
        tokens = line.split('=')
        if len(tokens) < 2 or len(tokens[1]) == 0: continue
        cmake_extra_args.append('-D' + line)

print('CMake Properties:\n' + '\n'.join(cmake_extra_args))

def readme():
    with open('README.md') as f:
        return f.read()


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.9.0':
                raise RuntimeError("CMake >= 3.9.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable] + cmake_extra_args

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2 ** 32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.', '--target', name] + build_args, cwd=self.build_temp)


cmdclass = {
    'build_ext': CMakeBuild,
    'build_sphinx': BuildDoc
}

setup(name=name,
      version=version,
      author='Egor Dmitriev',
      author_email='egordmitriev@gmail.com',
      description='C++ / Python library with signal processing and tempo estimation utilities.',
      long_description=readme(),
      long_description_content_type='text/markdown',
      url='https://github.com/EgorDm/libtempo',
      license='MIT',
      ext_modules=[CMakeExtension(name)],
      cmdclass=cmdclass,
      zip_safe=False,
      command_options={
          'build_sphinx': {
              'project': ('setup.py', name),
              'version': ('setup.py', version),
              'release': ('setup.py', release)
          }
      },
      classifiers=(
          "Programming Language :: Python :: 3",
          "License :: OSI Approved :: MIT License",
      ),
      )
