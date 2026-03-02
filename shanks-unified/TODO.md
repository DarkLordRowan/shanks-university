* Complex rendering filters (reals, imags rendering)
* ~~CInterval~~
* По умолчанию частичные суммы включить
* Все ряды подрубить
* Остановка и сглаживание
* Сумму операций, с возможностью (потом) добавить веса операциям при суммировании
* Отказаться от json
* Придумать, что бы прога не убивала пк, при чрезмерной нагрузке, а или сама крашилась, либо не давала включить много, либо... хз, считая операционку, которая потребуется перед тем, как будет все делать?
* ВСЕ ПАРАМЕТРЫ JSON ДОЛЖНЫ ДОХОДИТЬ ДО C++ ТОЧНО.
* DONE: ffi/,
* From CSV
* Спросить про window_length
* Lists are currently 100% useless
* Return export
* REVERIFY goddamn View. It's not working as intended.

- name: Setup MSYS2
        uses: msys2/setup-msys2@v2
        with:
          msystem: MINGW64
          release: false
          update: false
          install: >-
            mingw-w64-x86_64-toolchain
            mingw-w64-x86_64-cmake
            mingw-w64-x86_64-ninja
            mingw-w64-x86_64-gmp
            mingw-w64-x86_64-mpfr
            mingw-w64-x86_64-eigen3
            mingw-w64-x86_64-gsl
            mingw-w64-x86_64-ccache

      - name: Setup Rust (GNU)
        uses: dtolnay/rust-toolchain@stable
        with:
          toolchain: stable-x86_64-pc-windows-gnu

      - name: Cache C++ (ccache)
