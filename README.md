# TiXeL

Implementation of [Transactional Data Structure Libraries](https://iditkeidar.com/wp-content/uploads/files/ftp/TransactionalLibrariesPLDI16.pdf) in C++.

### Documentation

See 'Documentation' directory for the key project details, including all the supporting documents created over the course of the project.

### Clone

To clone properly, run the following commands:

```
 git clone https://github.com/ofirkov/TiXeL.git
 cd TiXeL
 git submodule update --recursive
```

### Build

While TiXeL is an open-source, cross-platform foundation, designed to build, test, and package across OS retails – its compilation process is (currently) bound to MSVC.

To use MSBuild.exe, one should invoke the BuildProject.bat, from an administrative prompt on a Windows10(+) device. That will, in turn, include several CL switches to specify various aspects of our compilation process.

```
$ BuildProject.bat
```

### Test

See 'Tests' directory for the benchmark performance testing, the metric against which TDSL can be compared to assess our quality measures. These benchmarks set a standard that helps to determine the quality of our product.






