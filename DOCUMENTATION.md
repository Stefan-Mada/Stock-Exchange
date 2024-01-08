Documentation
=============

Documentation is generated with Doxygen. Here are some pages to quickly check out.

If you are on GitHub, the links below will not appear. You can view the latest
main branch documentation [here](https://stefan-mada.github.io/Stock-Exchange/).

If you want to build the latest documentation, you will need to download the repo, build the
documentation, then open `docs/html/index.html` to have the interactive documentation.

You can build the documentation following the same steps as build the program normally, but then running the following command in your build directory:

    cmake --build . --target=docs

Or simply run this in the source directory:

    doxygen Doxyfile



### Headers
* limitPrice.hpp
* order.hpp
* orderBook.hpp
* orderExecution.hpp