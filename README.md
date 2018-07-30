# v5
# Debug MSVC
-DBLAS_LIBRARY="D:/Developent/CPP/libs/OpenBLAS PREB 0.2.14/lib/x64/libopenblas.dll.a"
-DLAPACK_LIBRARY="D:/Developent/CPP/libs/OpenBLAS PREB 0.2.14/lib/x64/libopenblas.dll.a"
-DARMADILLO_INCLUDE_DIR="D:/Developent/CPP/libs/armadillo-8.600.0/include"
-DARMADILLO_LIBRARY:FILEPATH="D:/Developent/CPP/libs/armadillo-8.600.0/build/Debug/armadillo.lib"
-DSIGPACK_INCLUDE_DIR="D:\Developent\CPP\libs\sigpack-1.2.4\sigpack"
-DFFTW_ROOT="D:\Developent\CPP\libs\libfftw"

# Debug mingw
-DGMOCK_ROOT=D:\Developent\CPP\libs\googletest-release-1.8.0\googlemock
-Dpybind11_INCLUDE_DIR=D:\Developent\CPP\libs\pybind11-2.2.3\include
-DARMADILLO_LIBRARY=D:\Developent\CPP\libs\armadillo-8.600.0
-DARMADILLO_INCLUDE_DIR=D:\Developent\CPP\libs\armadillo-8.600.0\include
-DSIGPACK_INCLUDE_DIR=D:\Developent\CPP\libs\sigpack-1.2.4\sigpack
-DPYTHON_EXECUTABLE=D:\msys64\mingw64\bin\python3.6m.exe
-DPYTHON_LIBRARY=D:\msys64\mingw64\lib\libpython3.6m.dll.a
-DPYTHON_INCLUDE_DIRS=D:\msys64\mingw64\include\python3.6m
-DLAPACK_LIBRARY="D:/msys64/mingw64/lib/libopenblas.dll.a"


Delete
sp::sp_version from base.h

# TODO
take this library
https://github.com/terrygta/SignalResampler/tree/master/src
remove boost. add armadillo