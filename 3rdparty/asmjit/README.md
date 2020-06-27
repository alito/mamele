AsmJit
------

AsmJit is a lightweight library for machine code generation written in C++ language.

  * [Official Home Page (asmjit.com)](https://asmjit.com)
  * [Official Repository (asmjit/asmjit)](https://github.com/asmjit/asmjit)
  * [Public Chat Channel](https://gitter.im/asmjit/asmjit)
  * [Zlib License](./LICENSE.md)

See [asmjit.com](https://asmjit.com) page for more details, examples, and documentation.

Documentation
-------------

  * [Documentation Index](https://asmjit.com/doc/index.html)
  * [Build Instructions](https://asmjit.com/doc/group__asmjit__build.html)

Breaking Changes
----------------

Breaking the API is sometimes inevitable, what to do?

  * See [Breaking Changes Guide](https://asmjit.com/doc/group__asmjit__breaking__changes.html), which is now part of AsmJit documentation.
  * See asmjit tests, they always compile and provide implementation of many use-cases:
    * [asmjit_test_x86_asm.cpp](./test/asmjit_test_x86_asm.cpp) - Tests that demonstrate the purpose of emitters.
    * [asmjit_test_x86_cc.cpp](./test/asmjit_test_x86_cc.cpp) - A lot of tests targeting Compiler infrastructure.
    * [asmjit_test_x86_sections.cpp](./test/asmjit_test_x86_sections.cpp) - Multiple sections test.
  * Visit our [Official Chat](https://gitter.im/asmjit/asmjit) if you need a quick help.

Project Organization
--------------------

  * **`/`**        - Project root.
    * **src**      - Source code.
      * **asmjit** - Source code and headers (always point include path in here).
        * **core** - Core API, backend independent except relocations.
        * **arm**  - ARM specific API, used only by ARM and AArch64 backends.
        * **x86**  - X86 specific API, used only by X86 and X64 backends.
    * **test**     - Unit and integration tests (don't embed in your project).
    * **tools**    - Tools used for configuring, documenting, and generating files.

TODO
----

  * [ ] Core:
    * [ ] Add support for user external buffers in CodeHolder.
  * [ ] Ports:
    * [ ] ARM/Thumb/AArch64 support.

Donors
------

  * [ZehMatt](https://github.com/ZehMatt)


Authors & Maintainers
---------------------

  * Petr Kobalicek <kobalicek.petr@gmail.com>
