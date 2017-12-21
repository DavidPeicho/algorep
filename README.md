# Distributed Algorithm

## What's this project?!

The goal is to provide an high-level & easy-to-use **API** for making distrubted algorithms. It is fully written in **C++** for performance reasons.

This project is part of our Master in Computer Science.

## Design Goals

The library tries to be as much typed as it can!

The library is conceived to let the compiler make as much static check as it can, before the data are actually sent over the network.

## API examples

```cpp
#include <algorep.h>

void
run();

int
main(int argc, char** argv)
{
    // This will setup the OpenMPI library.
    algorep::init(argc, argv);

    const auto& callback = std::function<void()>(run);
    // `callback` will be run on the master, and each slaves
    // will be given a maximum of 200 bytes.
    algorep::run(callback, 16);

    // This is in charge of liberating memory.
    algorep::terminate();
}

// The function below will be run onced,
void
run()
{
    // Gets the allocator, which is accessible everywhere.
    auto* allocator = algorep::Allocator::instance();

    // This is ~18 bytes, will not fit on a single slave, because
    // we provided at most 16 bytes per slave.
    std::vector<short> array({ 5, 98, -76, 10, 0, 0, -87, 19, 0 })

    // Allocates `array.size()` short values on the slaves, and copy
    // the pointer `&array[0]` to this allocated area.
    Element<short>* var = allocator->reserve<T>(array.size(), &array[0]);
}

```

## Features

### Allocation

You can allocate variable using:
```cpp
Element<my_type>* var = allocator->reserve<T>(data_size, pointer_to_data);
```

With `my_type` the type of your choice. It could be a `struct Student`, an `int`, a `double`, ...

### Read
```cpp
// var is of type Element<my_type>
my_type* read_back = allocator->read<my_type>(var);
```
Be careful here, you are reading **all** your data back to your master. If you had 5Go allocated on several slaves, your master will read them back in the `read_back` variable, there is no smart memory mapping system for now.

### Free
```cpp
// var is of type Element<my_type>
allocator->free(var);
```

Be careful here, when free returns, your variable `var` has also been free in order to prevent you to try to read back data that have been freed on slaves.
So, using `var` is undefined behaviour.

###

## Build

### Dependencies

You will need the following dependencies:
[OpenMPI](https://www.open-mpi.org/)

### Linux

You can build the project simply using `make`:

```sh
my_super_sh$ cd distrubted_algorithm/
my_super_sh$ make

```

### Windows

Currently not supported.

### MacOS

Currently not supported.

## Run

### Samples

You can test our basic sample by running:

```sh
my_super_sh$ make sample/simple_map_reduce
my_super_sh$ mpirun -n 4 sample/simple_map_reduce

```

### You code

When you have built the library, and that you compiled your own binary named `my_super_sample`, you can run it using the `mpirun` binary:

```sh
my_super_sh$ mpirun -n 4 my_super_sample

```

This command will run the sample `my_super_sample` in 4 different processus. If you plan to use different machine, take a closer look at the `mpirun` documentation.

## Test

We provide a simple test suite, which can be run as follow:
```sh
my_super_sh$ cd algorep && make check

```

For now, the test suite is simple, and it should be replaced either by using GoogleTest, or another famouse test framework such as libcheck.

