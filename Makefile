.PHONY: all set-target build clean fullclean size checks format
all: build
set-target:
	. ${IDF_PATH}/export.sh && idf.py set-target esp32s3
build:
	. ${IDF_PATH}/export.sh && idf.py build
clean:
	. ${IDF_PATH}/export.sh && idf.py clean
fullclean:
	. ${IDF_PATH}/export.sh && idf.py fullclean
size:
	. ${IDF_PATH}/export.sh && idf.py size-components
checks:
	bash scripts/checks.sh
format:
	@command -v clang-format >/dev/null 2>&1 || { echo "clang-format missing"; exit 0; }
	@find components main -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" \) -print0 | xargs -0 clang-format -i
